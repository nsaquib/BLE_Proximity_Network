import csv
import serial
import time
import datetime

baudRate = 9600
##ser = serial.Serial("/dev/cu.usbserial-DN0073UM", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00CW36", baudRate)
ser = serial.Serial("/dev/cu.usbserial-DN00B462", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00B1WO", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00BOCJ", baudRate)


deviceID = ser.readline().rstrip().decode("utf-8")
writeFile = deviceID + ".csv"
with open(writeFile, "w") as file:
  writer = csv.writer(file)
  while True:
      writer.writerow([ser.readline().rstrip().decode("utf-8")])
      #row = ["Timestamp", "Device ID", "RSSI"]

print("Data transmission complete")
