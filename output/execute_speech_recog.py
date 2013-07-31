#!/bin/python3.3
# -*- coding: utf-8 -*-

# This script serves the purpose of :
# Calling julius & SS with different SS config files
# We have twenty sentences and each sentence will have the 200 kinds of noise / SNR applied
# Orga :
# /EvalNoise/original/
# /EvalNoise/noisy/whitenoise_20db/file2.wav
# /EvalNoise/noisy/whitenoise_20db/file1.wav...
# Computing the average word recognition rate

# Output it to file.

############NOTICE : JULIUS RAW AUDIO IS IN BIG ENDIAN##############
import subprocess as sp
import os
import re
import shutil
from compute_word_rate import *

def is_non_zero_file(fpath):
    return True if os.path.isfile(fpath) and os.path.getsize(fpath) > 0 else False


def GenerateFileLists():
	for root, subFolders, files in os.walk("/media/doom/Data/EvalNoise/noisy/"):
		outfileName = os.path.join(root, "py-outfile.txt")
		print ("outfileName is " + outfileName)
		with open( outfileName, 'w' ) as folderOut:
			for folder in subFolders:
				print ("%s has subdirectory %s" % (root, folder))

			for filename in files:
				filePath = os.path.join(root, filename)

				with open( filePath, 'r' ) as f:
					folderOut.write("%s\n" % (filePath))



algo_scripts = ["ss_conf/std", "ss_conf/el", "ss_conf/ga", "ss_conf/std_martin", "ss_conf/ga_martin"]

# 1. Load the original sentences
original = open("/media/doom/Data/JNAS/Vol. 1/ORG_TEXT/KANJI/NP/001.KAN", "r", encoding='iso-2022-jp')
orig_sents = [re.findall(r"[\w']+", l.split(' ', 3)[3][:-1]) for l in original.readlines()[0:20]]
original.close()

output_results = open("complete_results", "w")
# for each folder in EvalNoise/noisy
for dirname, dirnames, filenames in os.walk("/media/doom/Data/EvalNoise/noisy/"):

	for filename in filenames:
		filepath = os.path.join(dirname, filename)
		if filename == "py-outfile.txt" and is_non_zero_file(filepath):
			print("Calling with wordlist: " + filepath)
			julius_cmd = "./juliusSub -C ~/stuff/japanese-models/jconf/standard.jconf -charconv EUC-JP iso-2022-jp -filelist " + filepath + " -nolog > plain_results"

			output_results.write(dirname)
			for algo_file in algo_scripts:
				# Copy the good script
				shutil.copy(algo_file, "subtraction.conf")
				# Call julius with good SS configuration ?
				sp.call(julius_cmd, shell=True)

				# Extract sentences and words
				results = open("plain_results", "r", encoding='iso-2022-jp')
				res_sents = [re.findall(r"[\w']+", l[12:-1]) for l in results.readlines() if (l.startswith("sentence1:") or l.startswith("<search failed>"))]
				results.close()

				WRR = ComputeWRR(orig_sents, res_sents)
				output_results.write("\t" + str(WRR))

			output_results.write("\n")
output_results.close()
