f = open("data0.txt",'r')
deviceNumber = '7'

lines = f.readlines()

for line in lines:
    a = line.split(',')
    if len(a) > 1:
        if a[1] == deviceNumber:
            print(a[0], '\t', a[2])

f.close()
