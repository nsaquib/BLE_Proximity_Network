import serial
import time
import datetime

baudRate = 9600
connected = False
##ser = serial.Serial("/dev/cu.usbserial-DN0073UM", baudRate)
ser = serial.Serial("/dev/cu.usbserial-DN00CW36", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00B462", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00B1WO", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00BOCJ", baudRate)
while not connected:
    serIn = ser.read()
    connected = True

ser.write(bytes(datetime.datetime.now().strftime('%m%d%y%w%H%M%S%f')[0:16], 'ascii')) #<200 ms to write
print(datetime.datetime.now().strftime('%m/%d/%y %w %H:%M:%S:%f')[0:23]);
print("Sent parameters")
