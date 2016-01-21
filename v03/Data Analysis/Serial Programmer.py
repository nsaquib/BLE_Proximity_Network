import serial
import time
import datetime

baudRate = 9600
connected = False
#ser = serial.Serial("/dev/cu.usbserial-DN0073UM", baudRate)
ser = serial.Serial("/dev/cu.usbserial-DN00B462", baudRate)
#ser = serial.Serial("/dev/cu.usbserial-DN00B1WO", baudRate)
#ser = serial.Serial("/dev/cu.usbserial-DN00BOCJ", baudRate)
while not connected:
    serIn = ser.read()
    connected = True

now = datetime.datetime.now()
month = now.month
date = now.day
year = now.year - 2000
day = (now.weekday() + 1) % 7
hour = now.hour
minute = now.minute
sec= now.second
ms = int(now.microsecond/1000)

print(str(month) + "/" + str(date) + "/" + str(year) + " " + str(day) + " " + str(hour) + ":" + str(minute) + ":" + str(sec) + ":" + str(ms))

ser.write(bytes(str(month), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(date), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(year), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(day), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(hour), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(minute), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(sec), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(ms), 'ascii'))

print("Wrote bits")

