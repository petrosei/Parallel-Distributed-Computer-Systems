
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os


DEVNULL = open(os.devnull, 'wb')

processors = [1,2, 4, 8, 16, 32, 64, 128, 256]
Q = range(12, 16)


command = "./bitonic_serial"

pmk1 = Popen(["mkdir","../Results"], stdout=DEVNULL, stderr=DEVNULL)
p1 = Popen(["mkdir","../Results/serial_bitonic_results"], stdout=DEVNULL,stderr=DEVNULL)
p2 = Popen(["rm","../Results/serial_bitonic_results/*"], stdout=DEVNULL,stderr=DEVNULL)

for q in Q:
#	filename = "bsortresults{}.txt".format(q)
	arguments = "{}".format(q)
	p = Popen([command,arguments], stdout=PIPE)
       	output = p.communicate()[0]
	print output
