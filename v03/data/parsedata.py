f = open("host3.txt",'r')

lines = f.readlines()

for line in lines:
    a = line.split(',')
    if len(a) > 1:
        if a[1] == '3':
            print a[0], '\t', a[2]

f.close()
