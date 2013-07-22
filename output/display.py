from matplotlib.pylab import *
import numpy, os

def find_max_vert_index(data):
	maxi = 0
	max_index = 0
	i = 0
	for row in data:
		i += 1
		if(numpy.mean(row) > maxi):
			maxi = numpy.mean(row) 
			max_index = i
	return max_index
		
		

def animateBefore(i):
	data = numpy.loadtxt("dataBefore/{}.dat".format(i))
	imshow(data, aspect='auto')
	outfile = 'outBefore/{}.png'.format(i)
	if os.access(outfile, os.F_OK):
		os.remove(outfile)
	#show()
	savefig(outfile, bbox_inches = 'tight')
	clf()
	
def animateAfter(i):
	data = numpy.loadtxt("dataAfter/{}.dat".format(i))
	print(find_max_vert_index(data))
	imshow(data)
	outfile = 'outAfter/{}.png'.format(i)
	if os.access(outfile, os.F_OK):
		os.remove(outfile)
	savefig(outfile, bbox_inches = 'tight')
	clf()

b = 0
for i in range(b, b + 12):
#	animateBefore(i)
	animateAfter(i)

