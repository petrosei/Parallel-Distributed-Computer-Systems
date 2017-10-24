
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os


DEVNULL = open(os.devnull, 'wb')


Q = range(12, 25)


command = "./bitonic_serial"
path1 = "../Results"
path2 = "{0}/serial_bitonic_results".format(path1)
p1 = Popen(["mkdir",path1], stdout=DEVNULL, stderr=DEVNULL)
p2 = Popen(["mkdir",path2], stdout=DEVNULL,stderr=DEVNULL)
p3 = Popen(["rm","{0}/*".format(path2)], stdout=DEVNULL,stderr=DEVNULL)

for q in Q:
	filename = "{0}/bserialresults{1}.txt".format(path2,q)
	p4 = Popen(["touch",filename], stdout=DEVNULL,stderr=DEVNULL)
	arguments = "{}".format(q)
	p = Popen([command,arguments], stdout=PIPE)
       	out1 = p.communicate()[0]
	out2 = out1.split()
	out3 = "{0} {1} {2} \n{3} {4} {5}".format(out2[0],out2[5],out2[7],out2[8],out2[13],out2[15])
	f = open(filename,"w")
	f.write(out3)
