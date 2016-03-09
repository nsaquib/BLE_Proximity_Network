import serial
import time
import datetime

baudRate = 57600
connected = False
##ser = serial.Serial("/dev/cu.usbserial-DN0073UM", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00B1WO", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00CW36", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00B462", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN006JNJ", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00B3V1", baudRate)
ser = serial.Serial("/dev/cu.usbserial-DN00BGL7", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00BOCJ", baudRate)
##ser = serial.Serial("/dev/cu.usbserial-DN00BH7F", baudRate)

while not connected:
    serIn = ser.read()
    connected = True

ser.write(bytes("T" + datetime.datetime.now().strftime('%m%d%y%w%H%M%S%f')[0:16])) #<200 ms to write
##ser.write(bytes("T" + datetime.datetime.now().strftime('%m%d%y%w%H%M%S%f')[0:16], 'ascii')) #<200 ms to write
print(datetime.datetime.now().strftime('%m/%d/%y %w %H:%M:%S:%f')[0:23]);
print("Sent parameters")
