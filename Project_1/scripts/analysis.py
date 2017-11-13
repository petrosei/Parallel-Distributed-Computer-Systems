import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os
import matplotlib.pyplot as plt

DEVNULL = open(os.devnull, 'wb')

processors = [1, 2, 4, 8, 16, 32, 64, 128, 256]
proc = ['1', '2', '4', '8', '16', '32', '64', '128', '256']
Q = range(12,25 )
nt = range(1,10)

path = "../Results/openMP_parallel_results/"

#os.chdir(arg1)
imp = []
rec = []
load = []

for q in Q:
	imp = []
	rec = []
	for n_proc in processors:
		filename = "{0}/openMPresults_{1}_{2}.txt".format(path,n_proc,q)		
		file = open(filename, "r") 
		res =  file.read().splitlines()
		res1 = res[0].split()
		res2 = res[1].split()
		imp.append(float(res1[1]))
		rec.append(float(res2[1]))
	plt.plot(nt,imp)
	#plt.xticks(processors)
	plt.xticks(nt,proc)
	print imp
plt.legend(['16', '17', '18', '19','20','21','22','23','24'], loc='upper left')
plt.show()
		
#print imp
#print rec
#print proc
#print load		
#p1 = Popen(["python",arg2], stdout=DEVNULL, stderr=DEVNULL)
#p1.wait()
