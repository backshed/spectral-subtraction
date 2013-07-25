# This file holds HTK configuration and script files


first_hcopy = """
SOURCEFORMAT=NIST
TARGETKIND=MFCC_E
TARGETRATE=100000.0
SAVECOMPRESSED=T
SAVEWITHCRC=T
WINDOWSIZE=250000.0
USEHAMMING=T
PREEMCOEF=0.97
NUMCHANS=26
CEPLIFTER=22
NUMCEPS=12
"""

first_hcompv = """
TARGETKIND=MFCC_E_D_A
TARGETRATE=100000.0
SAVECOMPRESSED=T
SAVEWITHCRC=T
WINDOWSIZE=250000.0
USEHAMMING=T
PREEMCOEF=0.97
NUMCHANS=26
CEPLIFTER=22
NUMCEPS=12
"""


spr_conf = """
# Single pass retraining
HPARM1: TARGETKIND = MFCC_E_D_A
HPARM2: TARGETKIND = MFCC_E_D_A_Z
"""

last_conf = """
TARGETKIND=MFCC_E_D_A_Z
TARGETRATE=100000.0
SAVECOMPRESSED=T
SAVEWITHCRC=T
WINDOWSIZE=250000.0
USEHAMMING=T
PREEMCOEF=0.97
NUMCHANS=26
CEPLIFTER=22
NUMCEPS=12
"""


associations = { "pau" : "sil",
				 "h#" : "sil",
				 "tcl" : "sil",
				 "pcl" : "sil",
				 "kcl" : "sil",
				 "epi" : "sil",
				 "bcl" : "vcl",
				 "dcl" : "vcl",
				 "gcl" : "vcl",
				 "el" : "l",
				 "em" : "m",
				 "zh" : "sh",
				 "ao" : "aa",
				 "ax-h" : "ah",
				 "axh": "ah",
				 "ax" : "ah",
				 "eng" : "ng",
				 "axr" : "er",
				 "ix" : "ih",
				 "ux" : "uw",
				 "nx" : "n",
				 "en" : "n",
				 "hv" : "hh"
}



associations_bak = { "pau" : "sil", "h#" : "sil", "tcl" : "sil", "pcl" : "sil",
		  "kcl" : "sil", "bcl" : "vcl", "dcl" : "vcl", "gcl" : "vcl",
		  "ax-h" : "ax", "axh": "ax", "dx" : "t", "axr" : "er", "ix" : "ih", "ux" : "uw",
		  "nx" : "n", "hv" : "hh"}


proto_file = """
~o <VecSize> 39 <MFCC_E_D_A>
~h "proto"
<BeginHMM>
 <NumStates> 5
 <State> 2
  <Mean> 39
    0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
  <Variance> 39
    1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
 <State> 3
  <Mean> 39
    0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
  <Variance> 39
    1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
 <State> 4
  <Mean> 39
    0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
  <Variance> 39
    1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
 <TransP> 5
  0.0 1.0 0.0 0.0 0.0
  0.0 0.6 0.4 0.0 0.0
  0.0 0.0 0.6 0.4 0.0
  0.0 0.0 0.0 0.7 0.3
  0.0 0.0 0.0 0.0 0.0
<EndHMM>
"""


sp_model = """~h "sp"
<BEGINHMM>
<NUMSTATES> 3
<STATE> 2
<MEAN> 39
[MEANS]
<VARIANCE> 39
[VARIANCES]
[GCONST]
<TRANSP> 3
 0.0 1.0 0.0
 0.0 0.9 0.1
 0.0 0.0 0.0
<ENDHMM>
"""


sil_hed = """
AT 2 4 0.2 {sil.transP}
AT 4 2 0.2 {sil.transP}
AT 1 3 0.3 {sp.transP}
TI silst {sil.state[3],sp.state[2]}
"""

mktri_led = """
WB sp
WB sil
TC
"""
