import math

f = open("data/wildflower/data0.txt",'r')
f_w = open("data/data0.json",'r+')
deviceNumber = '0'
hostLocationX = 1230
hostLocationY = 735
timeCounter = 0
lines = f.readlines()
f_w.write('{')
for line in lines:
    a = line.split(',')
    if len(a) > 1:
        #if a[1] == hostNumber:
    	rsval = a[2]
    	#rsval_radius = float(rsval) / math.sqrt(2)
    	#deviceLocationX = hostLocationX - rsval_radius
    	#deviceLocationY = hostLocationY - rsval_radius
    	timeCounter = timeCounter + 1
    	if rsval > -128:
    		f_w.write('"' + str(timeCounter) + '":[[' + str(hostLocationX) + ', ' + str(hostLocationY) + ']],')

f.close()
f_w.write('}')
f_w.close()
