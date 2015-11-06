import math

f = open("data/wildflower/data2.txt",'r')

#hostNumber = '0'
#hostLocationX = 3492;
#hostLocationY = 498;

#hostNumber = '1'
#hostLocationX = 3552;
#hostLocationY = 0;

#hostNumber = '2'
#hostLocationX = 1800;
#hostLocationY = 750;

hostNumber = '3'
hostLocationX = 1800;
hostLocationY = 750;

# hostNumber = '4'
# hostLocationX = 3492;
# hostLocationY = 498;

# hostNumber = '5'
# hostLocationX = 3492;
# hostLocationY = 498;
# timeCounter = 0;

lines = f.readlines()

for line in lines:
    a = line.split(',')
    if len(a) > 1:
        if a[1] == hostNumber:
        	rsval = a[2]
        	rsval_radius = float(rsval) / math.sqrt(2)
        	deviceLocationX = hostLocationX
        	deviceLocationY = hostLocationY - int(rsval)
        	print( a[0] + '\t' + str(deviceLocationX) + '\t' + str(deviceLocationY))

f.close()
