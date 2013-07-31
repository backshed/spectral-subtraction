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



# for each folder in speech_eval/noisy
	# Generate filelist > wavlist

	# apply Julius

	# ./juliusSub -C ~/stuff/japanese-models/jconf/standard.jconf -charconv EUC-JP iso-2022-jp -filelist ~/JAIST/wavlist -nolog > plain_results

	# 1. Only take the lines that begin wtih "sentence1"
	results = open("plain_results", "r", encoding='iso-2022-jp')
	res_sents = [re.findall(r"[\w']+", l[12:-1]) for l in results.readlines() if l.startswith("sentence1:")]
	results.close()

	WRR = ComputeWRR(orig_sents, res_sents)

original.close()
