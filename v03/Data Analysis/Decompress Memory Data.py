import csv
import math
import os

def decompressFile(readFile, writeFile):
    with open(readFile, "r", errors = 'ignore') as readF:
        with open(writeFile, "a") as writeF:
            for line in readF:
                writer = csv.writer(writeF)
                value = line.rstrip()
                if value != '' and value.isdigit():
                    value = int(value)
                    if value > 1000:
                        deviceID = math.floor(value / 100000000)
                        value -= deviceID * 100000000
                        rssi = math.floor(value / 1000000)
                        value -= rssi * 1000000
                        time = value
                        if rssi > 0:
                            row = [time, deviceID, rssi]
                            writer.writerow(row)

for readFile in os.listdir(os.getcwd()):
    if readFile.endswith(".txt"):
      writeFile = readFile.split('.')[0] + ".csv"
      with open(writeFile, "w") as file:
          writer = csv.writer(file)
          row = ["Timestamp", "Device ID", "RSSI"]
          writer.writerow(row)
      decompressFile(readFile, writeFile)
