#include <julius/juliuslib.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "../noisereduction_common/noise_red.h"
static void modifyAudioCallback(Recog *recog, SP16* buf, int len, void *dummy)
{
	std::cout << "Len: " << len << std::endl;
	double* data = new double[len];
	for(int i = 0; i < len; ++i)
	{
		data[i] = buf[i] / 32768.;
	}
	for(int i = 0; i < len; ++i)
	{
		buf[i] = data[i] * 32768.;
	}
}

static void output_result(Recog *recog, void *dummy)
{
	int i, j;
	int len;
	WORD_INFO *winfo;
	WORD_ID *seq;
	int seqnum;
	int n;
	Sentence *s;
	RecogProcess *r;
	HMM_Logical *p;
	SentenceAlign *align;

	/* all recognition results are stored at each recognition process
	   instance */
	for(r=recog->process_list; r; r=r->next)
	{
		/* skip the process if the process is not alive */
		if (! r->live) continue;

		/* result are in r->result.  See recog.h for details */

		/* check result status */
		if (r->result.status < 0)        /* no results obtained */
		{
			/* outout message according to the status code */
			switch(r->result.status)
			{
			case J_RESULT_STATUS_REJECT_POWER:
				printf("<input rejected by power>\n");
				break;
			case J_RESULT_STATUS_TERMINATE:
				printf("<input teminated by request>\n");
				break;
			case J_RESULT_STATUS_ONLY_SILENCE:
				printf("<input rejected by decoder (silence input result)>\n");
				break;
			case J_RESULT_STATUS_REJECT_GMM:
				printf("<input rejected by GMM>\n");
				break;
			case J_RESULT_STATUS_REJECT_SHORT:
				printf("<input rejected by short input>\n");
				break;
			case J_RESULT_STATUS_FAIL:
				printf("<search failed>\n");
				break;
			}
			/* continue to next process instance */
			continue;
		}

		/* output results for all the obtained sentences */
		winfo = r->lm->winfo;

		for(n = 0; n < r->result.sentnum; n++)   /* for all sentences */
		{
			s = &(r->result.sent[n]);
			seq = s->word;
			seqnum = s->word_num;

			/* output word sequence like Julius */
			//printf("sentence%d:", n+1);
			for(i=1; i<seqnum-1; i++) printf("%s ", winfo->woutput[seq[i]]);
			printf("\n");
		}
	}

	/* flush output buffer */
	fflush(stdout);
}


int main(int argc, char *argv[])
{
	Jconf *jconf;
	Recog *recog;
	int ret;
	jlog_set_output(NULL);

	/* if no argument, output usage and exit */
	if (argc == 1)
	{
		return -1;
	}

	jconf = j_config_load_args_new(argc, argv);
	if (jconf == NULL)  		/* error */
	{
		fprintf(stderr, "ERROR");
		return -1;
	}


	recog = j_create_instance_from_jconf(jconf);
	if (recog == NULL)
	{
		fprintf(stderr, "Error in startup\n");
		return -1;
	}

	callback_add_adin(recog, CALLBACK_ADIN_CAPTURED, modifyAudioCallback, NULL);
	callback_add(recog, CALLBACK_RESULT, output_result, NULL);

	if (j_adin_init(recog) == FALSE)      /* error */
	{
		return -1;
	}


	switch(j_open_stream(recog, NULL))
	{
	case 0:			/* succeeded */
		break;
	case -1:      		/* error */
		fprintf(stderr, "error in input stream\n");
		return -1;
	case -2:			/* end of recognition process */
		fprintf(stderr, "failed to begin input stream\n");
		return -1;
	}


	ret = j_recognize_stream(recog);
	if (ret == -1) return -1;	/* error */


	j_close_stream(recog);
	j_recog_free(recog);

	/* exit program */
	return(0);
}
