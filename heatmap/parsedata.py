import collections

f_w = open("data/ble.json",'r+')

data = {}

kid = '1'

f = open("data0.txt",'r')
lines = f.readlines()
loc0 = [1230,1100]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          print value
          data[long(a[0])] = [loc0[0],loc0[1]-value]  
f.close()

f = open("data1.txt",'r')
lines = f.readlines()
loc1 = [1230,0]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc1[0],loc1[1]+value]  
f.close()

f = open("data2.txt",'r')
lines = f.readlines()
loc2 = [2350,0]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc2[0],loc2[1]+value]  
f.close()

f = open("data3.txt",'r')
lines = f.readlines()
loc3 = [2350,1100]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc3[0],loc3[1]-value]  
f.close()

f = open("data4.txt",'r')
lines = f.readlines()
loc4 = [3400,0]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -90:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc4[0],loc4[1]+value]  
f.close()

f = open("data5.txt",'r')
lines = f.readlines()
loc5 = [0,1100]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc5[0],loc5[1]-value]  
f.close()


od = collections.OrderedDict(sorted(data.items()))
f_w.write('{')
timeCounter = 0
for k, v in od.iteritems():
  timeCounter = timeCounter + 1
  f_w.write('"' + str(timeCounter) + '":[[' + str(v[0]) + ', ' + str(v[1]) + ']],')

f_w.write('}')
f_w.close()