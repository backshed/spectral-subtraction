import os, re, shutil
import subprocess as sp
import textwrap

def ChangeEndianness(path):
	for dirname, dirnames, filenames in os.walk(path):
		for filename in filenames:
			if('.raw'in filename):
				filepath = os.path.join(dirname, filename)
				newpath = filepath[:-3] + "raw_bis"
				call = "sox -t raw --endian little -e signed -r 16000 -c 1 -b 16 {0} --endian big -t raw {1}".format(filepath, newpath)
				sp.call(call, shell=True )
				os.rename(newpath, filepath)

ChangeEndianness("/media/doom/Data/EvalNoise/noisy/")
