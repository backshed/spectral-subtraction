#!/usr/bin/python3.3
import os, re, shutil
import subprocess as sp
import textwrap
import configuration

def createdir(path):
	if os.access(path, os.R_OK):
		shutil.rmtree(path)
	os.mkdir(path)

def write_to_file(path, data):
	f = open(path, 'w')
	print(data, file=f)
	f.close()

def Resample(path, dirs):
	for d in dirs:
		spath = path + '/' + d
		for dirname, dirnames, filenames in os.walk(spath):
			for filename in filenames:
				if('.WAV'in filename):
					filepath = os.path.join(dirname, filename)
					newpath = filepath[:-3] + "nist"
					call = "sox -t nist {0} -r 16000 -t nist {1}".format(filepath, newpath)
					sp.call(call, shell=True )
					os.rename(newpath, filepath)


def CreateTriphoneHED():
	outf = open('mktri.hed', 'w')
	inf = open('monophones')

	print('CL triphones', file=outf)
	for line in inf:
		phone = line[:-1]
		s = "TI T_%s {(*-%s+*,%s+*,*-%s).transP}" % (phone, phone, phone, phone)
		print(s, file=outf)

	inf.close()
	outf.close()

def CreateTiedTriphone():
	outf = open('tied.hed', 'w')
	inf = open('monophones')

	print('RO 20 stats_file', file=outf)
	for line in inf:
		phone = line[:-1]
		s = textwrap.dedent('''\
			TC 100.0 "{0}S2" {{({0},*-{0},{0}+*,*-{0}+*).state[2]}}
			TC 100.0 "{0}S3" {{({0},*-{0},{0}+*,*-{0}+*).state[3]}}
			TC 100.0 "{0}S4" {{({0},*-{0},{0}+*,*-{0}+*).state[4]}}'''.format(phone))
		print(s, file=outf)

	inf.close()
	outf.close()

def CreateScript(path, dirs):
	'''Generates the scp file needed to convert TIMIT Wav to HTK MFCC

	path should be the directory containing TRAIN, TEST, and DOC.
	ex.: ./TIMIT/

	ex. line of output :
	/home/doom/timit/test/DR3/MMWH0/SX189.WAV	/home/doom/phoneRecogDetailed/features/test/timit_test_DR3_MMWH0_SX189.WAV

	'''
	createdir('mfcc')


	for smpset in dirs:
		outf = open(smpset + '_NIST2MFC.scp', 'w')
		mfclist = open(smpset + '.scp', 'w')
		spath = path + '/' + smpset + '/'
		os.mkdir('mfcc/' + smpset)
		for dirname, dirnames, filenames in os.walk(spath):
			# print path to all filenames.
			for filename in filenames:
				if '.WAV' in filename:

					fullpath = os.path.join(dirname, filename)

					splittedpath = re.split("\/", fullpath)
					correctpath = './mfcc/' + smpset + '/' + splittedpath[2] + '_' + splittedpath[3] + '_' + splittedpath[4] + '_' + splittedpath[5]
					correctpath = correctpath[:-3] + 'mfc'
					print (fullpath, correctpath, file=outf)
					print (correctpath, file=mfclist)

		outf.close()
		mfclist.close()

def ExtractMonophones(mlf_file):
	sp.call("awk '/[\.!]/{next;}{print $3}' " + mlf_file + " | sort | uniq > monophones", shell=True)

def CreateMLF(path, dirs):
	''' Creates the master label file with phoneme data '''

	for d in dirs:
		ph_outf = open('phone{}.mlf'.format(d), 'w')
		wd_outf = open('word{}.mlf'.format(d), 'w')

		print("#!MLF!#", file=ph_outf)
		print("#!MLF!#", file=wd_outf)
		for dirname, dirnames, filenames in os.walk('.'):
			# print path to all filenames.
			for filename in filenames:
				if '.PHN' in filename:
					fullpath = os.path.join(dirname, filename)
					f = open(fullpath, 'r')
					splittedpath = re.split("\/", fullpath)

					if(splittedpath[2] == d):
						correctpath = splittedpath[2] + '_' + splittedpath[3] + '_' + splittedpath[4] + '_' + splittedpath[5]

						print('"*/' + (correctpath[:-3]) + 'lab"', file=ph_outf)
						for line in f:
							data = re.split(" +", line[:-1])

							if data[2] in configuration.associations:
								data[2] = configuration.associations[data[2]]

							if data[2] != 'q' and data[2] != 'epi':
								print (str(int(data[0]) * 625) + ' ' + str(int(data[1]) * 625) + ' ' + data[2], file=ph_outf)

						print(".", file=ph_outf)
					f.close()

				elif '.WRD' in filename:
					fullpath = os.path.join(dirname, filename)
					f = open(fullpath, 'r')
					splittedpath = re.split("\/", fullpath)

					if(splittedpath[2] == d):
						correctpath = splittedpath[2] + '_' + splittedpath[3] + '_' + splittedpath[4] + '_' + splittedpath[5]

						print('"*/' + (correctpath[:-3]) + 'lab"', file=wd_outf)
						for line in f:
							data = re.split(" +", line[:-1])
							# for 'em
							if(data[2][0] == "'"):
								data[2] = "\\" + data[2]

							print (str(int(data[0]) * 625) + ' ' + str(int(data[1]) * 625) + ' ' + data[2], file=wd_outf)

						print(".", file=wd_outf)
					f.close()
		ph_outf.close()
		wd_outf.close()

def ConvertDictionary(path):
	''' Converts the dictionary from TIMIT to HTK format.

	Also reduces the number of phonemes,
	and adds vcl in front of closing consonants.

	'''

	consonants = { 'b', 'd', 'g', 'p', 't', 'k'}

	inf = open(path)
	outf = open('dictionary', 'w')
	for line in inf:
		data = re.split(" +", line.replace('1', '').replace('2', '').replace("/", "")[:-1])

		outstr = data[0] + ' '

		for i in range(1, len(data)):
			if data[i] in configuration.associations:
				outstr += configuration.associations[data[i]] + ' '
			elif data[i] in consonants:
				outstr += 'vcl ' + data[i] + ' '
			else:
				outstr += data[i] + ' '

		outstr += 'sp'
		print (outstr, file=outf)

	print("silence sil", file=outf)
	outf.close()




def main():
	timitdir = './TIMIT'
	traindir = 'TRAIN'
	testdir = 'TEST'

	phoneMLF = "phone" + traindir + ".mlf"   #Normally phoneTRAIN.mlf
	wordMLF = "word" + traindir + ".mlf"   #Normally wordTRAIN.mlf
	trainSCP = traindir + ".scp" #Normally TRAIN.scp
	dirs = [traindir]

	########
	# Careful : resample only once! And backup original (it overwrites)
	# Resample('./TIMIT', ['/TRAIN/', '/TEST/'])
	########

	#Creation of basic data needed afterwards
	CreateScript(timitdir, dirs)
	CreateMLF(timitdir, dirs)
	ExtractMonophones(phoneMLF)
	ConvertDictionary('/CORRECTED_TIMITDIC.TXT')

	# Converting from NIST to MFCC_E
	write_to_file('cfg', configuration.first_hcopy)
	for sdir in dirs:
		sp.call('HCopy -C cfg -S {}_NIST2MFC.scp'.format(sdir), shell=True)

	# HMM prototype
	createdir('hmm')
	write_to_file('hmm/proto', configuration.proto_file)
	write_to_file('cfg', configuration.first_hcompv)
	sp.call('HCompV -C cfg -f 0.01 -m -S {} -M hmm hmm/proto'.format(trainSCP), shell=True)

	for sdir in dirs:
		sp.call('paste {0}.scp {0}.scp > tmp.scp'.format(sdir), shell=True)
		sp.call('HCopy -C cfg -S tmp.scp -A', shell=True) # a faire passer en e_d_a

	# Make hmmdefs and macros from proto and vFloors
	proto = open('hmm/proto', 'r')
	macros = open('hmm/macros', 'w')
	hmmdefs = open('hmm/hmmdefs', 'w')
	for i in range(3):
		print(proto.readline()[:-1], file=macros)
	shutil.copyfileobj(open('hmm/vFloors', 'r'), macros)
	macros.close()
	for line in proto:
		print(line[:-1], file=hmmdefs)

	# Initialize the model
	createdir('hmm1')
	createdir('hmm2')
	createdir('hmm3')
	createdir('hmm4')
	for phn in open('monophones'):
		sp.call('HInit -I {} -S {} -H hmm/macros -i 100 -C cfg -M hmm -l {} hmm/proto'.format(phoneMLF, trainSCP, phn[:-1]), shell=True)
		sp.call('sed "s/proto/{0}/" hmm/proto > hmm1/{0}'.format(phn[:-1]), shell=True)
		sp.call('HRest -I {0} -S {1} -H hmm/macros -M hmm2 -l {2} -i 100 hmm1/{2}'.format(phoneMLF, trainSCP, phn[:-1]), shell=True)

	# Concatenate the different models
	sp.call('cat hmm2/? hmm2/?? hmm2/??? > hmm3/hmmdefs', shell=True)
	shutil.copyfile('hmm2/macros', 'hmm3/macros')


	# HERest call : main monophonic estimation process
	for i in range(20):
		cmdline = 'HERest -I {0} -S {1} -H hmm3/macros  -H hmm3/hmmdefs -C cfg -t 250.0 150.0 1000.0 -M hmm3 monophones'.format(phoneMLF, trainSCP)
		sp.call(cmdline, shell=True)

	# Single-pass reestimation
	write_to_file('spr_cfg', configuration.spr_conf)
	for sdir in dirs:
		sp.call('paste {0}.scp {0}.scp > tmp.scp'.format(sdir), shell=True)
		sp.call('HERest -r -C spr_cfg -S tmp.scp -I phone{}.mlf -H hmm3/macros -H hmm3/hmmdefs -M hmm3 monophones'.format(sdir), shell=True)

	# Making a probabilist grammar from the dictionary and training files
	sp.call("awk '{print $1}' dictionary | sort | uniq > wordlist", shell=True)
	f = open('wordlist', 'r+')
	s = f.read()
	f.seek(0)
	f.write("!ENTER\n!EXIT\n" + s)
	f.close()
	sp.call('HLStats -A -b bigfn -o wordlist wordTRAIN.mlf', shell=True)
	sp.call('HBuild -A -z -n bigfn wordlist grammar.slf', shell=True)

	# Short pause model
	f = open('monophones', 'a')
	f.write('sp\n')
	f.close()

	f = open('hmm3/hmmdefs', 'r')
	hmmdefs = f.readlines()
	f.close()
	sil_index = hmmdefs.index('~h "sil"\n')

	sp_model = re.sub(r"\[GCONST\]", hmmdefs[sil_index+14][:-1], re.sub(r"\[VARIANCES\]", hmmdefs[sil_index+13][:-1], re.sub(r"\[MEANS\]", hmmdefs[sil_index+11][:-1], configuration.sp_model)))
	f = open('hmm3/hmmdefs', 'w')
	for l in hmmdefs:
		f.write(l)
	f.write(sp_model)
	f.close()

	# Creation of word-boundary markers
	write_to_file("sil.hed", configuration.sil_hed)
	sp.call('HHEd -A -H hmm3/macros -H hmm3/hmmdefs -M hmm3 sil.hed monophones', shell=True)
	write_to_file('cfg', configuration.last_conf)

	sp.call("HVite -A -l '*' -o SWT -b silence -C cfg -a -H hmm3/macros -H hmm3/hmmdefs -i aligned.mlf -m -t 250.0 150.0 1000.0 -y lab -I {} -S {} dictionary monophones".format(wordMLF, trainSCP), shell=True)

	sp.call("HERest -C cfg -I aligned.mlf -t 250.0 150.0 3000.0 -S TRAIN.scp -H hmm3/macros -H hmm3/hmmdefs -M hmm4 monophones", shell=True)
	sp.call("HERest -C cfg -I aligned.mlf -t 250.0 150.0 3000.0 -S TRAIN.scp -H hmm4/macros -H hmm4/hmmdefs -M hmm4 monophones", shell=True)

	# Triphones
	write_to_file('mktri.led', configuration.mktri_led)
	sp.call("HLEd -A -n triphones -l '*' -i wintri.mlf mktri.led aligned.mlf", shell=True)

	CreateTriphoneHED()
	sp.call("HHEd -A -H hmm4/macros -H hmm4/hmmdefs -M hmm4 mktri.hed monophones", shell=True)

	# Reestimation
	for i in range(20):
		cmdline = 'HERest -A -I wintri.mlf -S {0} -H hmm4/macros -H hmm4/hmmdefs -s stats_file -C cfg -t 250.0 150.0 3000.0 -M hmm4 triphones'.format(trainSCP)
		sp.call(cmdline, shell=True)

	# Tying the triphones
	CreateTiedTriphone()
	sp.call("HHEd -A -H hmm4/macros -H hmm4/hmmdefs -M hmm4 tied.hed triphones", shell=True)

	for i in range(20):
		cmdline = 'HERest -A -I wintri.mlf -S {0} -H hmm4/macros -H hmm4/hmmdefs -C cfg -t 250.0 150.0 3000.0 -M hmm4 triphones'.format(trainSCP)
		sp.call(cmdline, shell=True)

	createdir('Model')
	sp.call('HERest -A -I wintri.mlf -S {0} -H hmm4/macros -H hmm4/hmmdefs -C cfg -t 250.0 150.0 3000.0 -M Model triphones'.format(trainSCP), shell=True)

	shutil.copyfile('dictionary', 'Model/timit.htkdic')
	shutil.copyfile('grammar.slf', 'Model/timit.slf')
main()
