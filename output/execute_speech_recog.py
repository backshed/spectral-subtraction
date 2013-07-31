#!/bin/python3.3
# -*- coding: utf-8 -*-

# This script serves the purpose of :
# Calling julius & SS with different SS config files
# We have twenty sentences and each sentence will have the 200 kinds of noise / SNR applied
# Orga :
# /speech_eval/original/
# /speech_eval/noisy/whitenoise_20db/file2.wav
# /speech_eval/noisy/whitenoise_20db/file1.wav...
# Computing the average word recognition rate

# Output it to file.



import re
from compute_word_rate import *

# 1. Load the original sentences
original = open("/media/doom/Data/JNAS/Vol. 1/ORG_TEXT/KANJI/NP/001.KAN", "r", encoding='iso-2022-jp')
orig_sents = [re.findall(r"[\w']+", l.split(' ', 3)[3][:-1]) for l in original.readlines()[0:20]]
original.close()

output_results = open("complete_results", "w")
# for each folder in speech_eval/noisy
	# Generate filelist > wavlist

	print("name of noise", file=output_results)
	# for each noise_red algorithm
		# Call julius with good SS configuration ?
		# ./juliusSub -C ~/stuff/japanese-models/jconf/standard.jconf -charconv EUC-JP iso-2022-jp -filelist ~/JAIST/wavlist -nolog > plain_results

		# Extract sentences and words
		results = open("plain_results", "r", encoding='iso-2022-jp')
		res_sents = [re.findall(r"[\w']+", l[12:-1]) for l in results.readlines() if l.startswith("sentence1:")]
		results.close()

		WRR = ComputeWRR(orig_sents, res_sents)
		print("\t" + str(WRR), file=output_results)

	print("\n", file=output_results)
output_results.close()
