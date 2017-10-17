
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os


DEVNULL = open(os.devnull, 'wb')

processors = [1,2, 4, 8, 16, 32, 64, 128, 256]
Q = range(12, 12)



p1 = Popen(["mkdir","../Results/serial_bitonic_results"], stdout=PIPE)
'p1out = p1.communicate()'
p2 = Popen(["rm","../Results/serial_bitonic_results/* >dev/null"], stdout=PIPE)
'p2out = p2.communicate()'
for q in Q:
	filename = "bsortresults{0}.txt".format(q)
	command = "./serial_bitonic"
	arguments = "{0}",format(q)
	p = subprocess.Popen([command,arguments], stdout=subprocess.PIPE)
	pout = p.communicate()[0]
        print  pout

