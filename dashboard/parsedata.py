import re

f = open("Wildflower/3-2-16/4.txt",'r')
f_w = open("Wildflower/3-2-14/formatted/4.tsv", 'r+')
data = {}

expr_zero = re.compile('\n\d{8}\n')
expr_single = re.compile('\n\d{9}\n')

lines = f.readlines()
for line in lines:
  if expr_zero.match(line):
    kid = 0
    rsval = line[:2]
    hour = line[2:4]
    minute = line[4:6]
    sec = line[6:8]
    timestamp = hour + "-" + minute + "-" + sec
    value = []
    value[0] = kid
    value[1] = rsval
    data[timestamp] = data[timestamp].append(value)
  if expr_single.match(line):
    kid = line[0]
    rsval = line[1:3]
    hour = line[3:5]
    minute = line[5:7]
    sec = line[7:9]
    timestamp = hour + "-" + minute + "-" + sec
    value = []
    value[0] = kid
    value[1] = rsval
    data[timestamp] = data[timestamp].append(value)
  else:
    kid = line[:2]
    rsval = line[2:4]
    hour = line[4:6]
    minute = line[6:8]
    sec = line[8:10]
    timestamp = hour + "-" + minute + "-" + sec
    value = []
    value[0] = kid
    value[1] = rsval
    data[timestamp] = data[timestamp].append(value)
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

od = collections.OrderedDict(sorted(data.items()))
for k, v in od.iteritems():
  rsvalues = [128] * 11
  for pair in v:
    if pair[0] > 4:
      pair[0] = pair[0] - 1
    rsvalues[pair[0]] = pair[1]
  f_w.write(k + "\t" + rsvalues[0] + "\t" + rsvalues[1] + "\t" + rsvalues[2] + "\t" + rsvalues[3] + "\t" + rsvalues[4] + "\t" + rsvalues[5] + "\t" + rsvalues[6] + "\t" + rsvalues[7] + "\t" + rsvalues[8] + "\t" + rsvalues[9] + "\t" + rsvalues[10])

f_w.close()