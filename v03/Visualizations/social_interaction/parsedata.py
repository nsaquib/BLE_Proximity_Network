# Open file with data source
f = open("data13.txt",'r')
# Set the maximum number of devices
MAX_DEVICES = 15

# Get data from file
lines = f.readlines()
# Array for data consistent in time
timeBlock = [];
# Initialize current device
currentDevice = '0'
# Iterate over lines in file
for line in lines:
    # Split line along commas
    a = line.split(',')
    # Check line is valid data
    if len(a) > 1 and a[0] != '0' and a[2] != '0' and a[1].rstrip() != "10" and a[1].rstrip() != "11" and a[1].rstrip() != "12" and a[1].rstrip() != "13" and a[1].rstrip() != "14":
        if currentDevice < a[1]:
            # Track curent device
            currentDevice = a[1]
            # Add data to time block
            timeBlock.append(a[0].rstrip())
            timeBlock.append(a[2].rstrip())
        else:
            # Print time
            print(*timeBlock, sep = '\t')
            # Reset time block
            timeBlock = []
            # Track curent device
            currentDevice = a[1]
            # Add data to time block
            timeBlock.append(a[0].rstrip())
            timeBlock.append(a[2].rstrip())

f.close()
