import collections

f_w = open("data/wildflower_2/json/beatrice.json",'r+')

data = {}

kid = '0'

f = open("data/wildflower_2/host10.txt",'r')
lines = f.readlines()
loc0 = [3400,0]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          print value
          data[long(a[0])] = [loc0[0],loc0[1]-value]  
f.close()

f = open("data/wildflower_2/host11.txt",'r')
lines = f.readlines()
loc1 = [3200,1100]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc1[0],loc1[1]+value]  
f.close()

f = open("data/wildflower_2/host12.txt",'r')
lines = f.readlines()
loc2 = [1500,800]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc2[0],loc2[1]+value]  
f.close()

f = open("data/wildflower_2/host13.txt",'r')
lines = f.readlines()
loc3 = [1230,0]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc3[0],loc3[1]-value]  
f.close()

f = open("data/wildflower_2/host14.txt",'r')
lines = f.readlines()
loc4 = [1230,1100]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -90:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc4[0],loc4[1]+value]  
f.close()

f = open("data/wildflower_2/host15.txt",'r')
lines = f.readlines()
loc5 = [1230,0]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc5[0],loc5[1]-value]  
f.close()

f = open("data/wildflower_2/host17.txt",'r')
lines = f.readlines()
loc6 = [100,1100]
for line in lines:
    a = line.split(',')
    if len(a) > 2:
      if a[1] == kid: 
        if a[2] > -80:
          value = (long(a[2]) / -128.0) * 100
          data[long(a[0])] = [loc6[0],loc6[1]-value]  
f.close()


od = collections.OrderedDict(sorted(data.items()))
f_w.write('{')
timeCounter = 0
for k, v in od.iteritems():
  timeCounter = timeCounter + 1
  f_w.write('"' + str(timeCounter) + '":[[' + str(v[0]) + ', ' + str(v[1]) + ']],')

f_w.write('}')
f_w.close()