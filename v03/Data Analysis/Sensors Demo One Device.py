import serial
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import animation

connected = False;
ser = serial.Serial("/dev/cu.usbserial-DN0073UM", 9600)
#ser = serial.Serial("/dev/cu.usbserial-DN00B462", 9600)
#ser = serial.Serial("/dev/cu.usbserial-DN00B1WO", 9600)

while not connected:
    reponse = ser.read()
    connected = True

win = 300
yData = []

fig, ax = plt.subplots()
line, = ax.plot([], [], 'k-')
ax.margins(0.05)

def init():
    return line,

def animate(i):
    # Read line from serial port
    data = ser.readline().rstrip()
    # Strip newline charcter and split along commas
    data = data.decode("ascii").split(',')
    # Check to process DEVICE0
    if len(data) == 3 and data[0] == '0':
        # Add data point to list
        yData.append(int(data[1]))
        # Use numpy array for manipulation
        y = np.asarray(yData);
        # X axis is length of y data
        x = np.arange(y.size) + 1
        # Function for gating the amount of data to plot
        imin = min(max(0, i - win), x.size - win)
        # Resize x data
        xdata = x[imin:i]
        # Resize y data
        ydata = y[imin:i]
        # Set data for the line
        line.set_data(xdata, ydata)
        # Rescale axes limits
        ax.relim()
        # Rescale axes view
        ax.autoscale()
        # Return the calculated line
        return line,

# Animation and plot handler
anim = animation.FuncAnimation(fig, animate, init_func = init, interval = 50)
plt.show()
