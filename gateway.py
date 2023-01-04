from random import randrange
import time
import serial.tools.list_ports
import  sys
import requests
import json

def getPort():
    ports = serial.tools.list_ports.comports()
    N = len(ports)
    commPort = "None"
    for i in range(0, N):
        port = ports[i]
        strPort = str(port)
        if "USB Serial Device" in strPort:
            splitPort = strPort.split(" ")
            commPort = (splitPort[0])
    return commPort

isMicrobitConnected = False
if getPort() != "None":
    print(getPort())
    ser = serial.Serial( port=getPort(), baudrate=9600) # changed 
    print("Connect successful!")
    isMicrobitConnected = True
    
temp = 0
humi = 0
metan =0    
ambient_temp = 0
def processData(data):
    data = data.replace("!", "")
    data = data.replace("#", "")
    splitData = data.split(":")
    print(splitData) 
    try:
        if splitData[1] == "TEMP":
            global temp
            temp = splitData[2]
        elif splitData[1] == "HUMI":
            global humi
            humi = splitData[2]
        elif  splitData[1] == "METAN":
            global metan
            metan = splitData[2] 
        else:
            global ambient_temp
            ambient_temp = splitData[2];  
    except:
        pass    

mess = ""    
def readSerial():
    bytesToRead = ser.inWaiting()
    
    if (bytesToRead > 0):
        global mess
        mess = mess + ser.read(bytesToRead).decode("UTF-8")
        while ("#" in mess) and ("!" in mess):
            start = mess.find("!")
            end = mess.find("#")
            processData(mess[start:end + 1])
            if (end == len(mess)):
                mess = ""
            else:
                mess = mess[end+1:]    

while True:
    if isMicrobitConnected:
        readSerial()
    url = 'http://localhost/IOT/collection.php'
    
    data = {'temp': temp, 'humi': humi, 'metan': metan, 'ambient_temp': ambient_temp}
    try:
        x = requests.post(url, data)
    except:
        print('gui du lieu that bai');
    else:
        print(x.text)
         
    url = 'http://localhost/IOT/response.php'
    try:
        x = requests.get(url)  
    except:
        print('false');
    else:
        action = json.loads(x.text)
        ser.write((action["action"]+"#").encode()) 
        print(action['action'])          
    time.sleep(10)


        
