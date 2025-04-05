# coding=utf-8
import serial
import csv
import os, sys, time
import pyvisa
import pyvisa as visa
import socket
import struct
from time import sleep
from pickle import TRUE
from unittest import result
from numpy import int16
import re
import math
import pandas as pd


# Socket设备
class MySocketDev:
    # print("instant my class")
    def __init__(self, ip, port, timeout=5):
        self.SocketDev = socket.socket()
        self.SocketDev.settimeout(timeout)
        try:
            self.SocketDev.connect((ip, port))
        except socket.error as e:
            print("Socket Connect Error")
        print("Connect %s OK" % ip)
      
    def SocketSendStrCmd(self, string):
        self.SocketDev.send(string.encode())#

    def SocketReadStrAck(self):
        try:
            ReadData = self.SocketDev.recv(1024).decode()
        except socket.error as e:
            print("Socket Ack Error")
            return False
    
    def SocketStrSndRcv(self, string):
        self.SocketDev.send(string.encode())
        # self.SocketDev.flush()
        sleep(0.5)
        try:
            ReadData = self.SocketDev.recv(1024).decode()
        except socket.error as e:
            print("Socket Ack Error")
            return False, None
        return True, ReadData

    def SocketSendByteCmd(self, bytecmd):
        print(bytecmd)
        self.SocketDev.send(bytecmd)

    def SocketReadByteAck(self):
        ReadData = self.SocketDev.recv(1024)
        if(ReadData==None):
            print("no data")
            return False, None
        else:
            print(ReadData)
            return True, ReadData

    def SocketByteSndRcv(self, bytecmd):
        # print(bytecmd)
        self.SocketDev.send(bytecmd)
        ReadData = self.SocketDev.recv(1024)
        if(ReadData==None):
            print("no data")
            return False, None
        else:
            # print(ReadData)
            if(bytecmd[7]==ReadData[7]):
                return True, ReadData
            else:
                return False, ReadData

    def DeInit(self):
        print("SocketDev Close OK")
        self.SocketDev.close()


class DMMSocket(MySocketDev):
    def GetIDN(self):
        ret, id = self.SocketStrSndRcv("*IDN?\r\n")#*IDN?
        if(True==ret):
            print(id)
        else:
            print("get id error")

    def Read(self):
        ret, value = self.SocketStrSndRcv("Read?\r\n")#*IDN?
        if(True==ret):
            return value
        else:
            print("get value error")
            return 0

    def DC_Volt_Config(self, range):
        rangstr = "CONF:VOLT:DC %s\r\n" %range
        self.SocketSendStrCmd(rangstr)

    def DC_Curr_Config(self, term, range):
        self.SocketSendStrCmd("CONF:CURR:DC\r\n")
        termstr = "CURR:DC:TERM %d\r\n" %term
        rangstr = "CONF:CURR:DC %s\r\n" %range
        self.SocketSendStrCmd(termstr)
        self.SocketSendStrCmd(rangstr)
        # self.SocketSendStrCmd("CURR:DC:TERM 3\r\n") #10
        # self.SocketSendStrCmd("CURR:DC:RANG:AUTO ON\r\n")
        # self.SocketSendStrCmd("CONF:CURR:DC 100 mA\r\n") #1 uA/100 uA/1 mA/100 mA 1A 3A

class POWRSocket(MySocketDev):
    # work or calibration
    def setworkmode(self, mode):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x26, 0x00, 0x00]
        cmd[10] = int(mode)
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
              return True
        else:
            return False

   # fv or fi
    def setpowrmode(self, mode):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x20, 0x00, 0x00]
        cmd[10] = int(mode)
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
            return True
        else:
            return False

    # open/40u/80u/40m/3A
    def setcurrange(self, rang):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x21, 0x00, 0x00]
        cmd[10] = int(rang)
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
            return True
        else:
            return False

    # enable or disable output
    def setEnableOutput(self, flag):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x2A, 0x00, 0x00]
        cmd[10] = int(flag)
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
            return True
        else:
            return False

    # fv=set volt;fi=set curr
    def setvoltcurr(self, fvfi):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x01, 0x10, 0x00, 0x22, 0x00, 0x02, 0x04]
        float_bytes = struct.pack('f', fvfi)
        list_data = list(float_bytes)
        cmd.extend(list_data)
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
            return True
        else:
            return False

    # read volt
    def getvoltage(self):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x04, 0x00, 0x12, 0x00, 0x02]
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
            fdata = struct.unpack('<f', struct.pack('4B', ReadData[9], ReadData[10], ReadData[11], ReadData[12]))
            return True, fdata[0]
        else:
            return False, 0

    #read curr
    def getcurrent(self):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x04, 0x00, 0x14, 0x00, 0x02]
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
            fdata = struct.unpack('<f', struct.pack('4B', ReadData[9], ReadData[10], ReadData[11], ReadData[12]))
            return True, fdata[0]
        else:
            return False, 0

    #set calibration para
    def setcalpara(self, type, para):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x01, 0x10, 0x00, 0x30, 0x00, 0x10, 0x20]
        cmd[9] = 0x30 + int(type)*16    # force 4 float data;measure 4 float data   list 2 methods:1,+;2,extend
        for i in range(len(para)):
            float_bytes = struct.pack('f', para[i])
            list_data = list(float_bytes)
            cmd.extend(list_data)
        # print(cmd)
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
            return True
        else:
            return False

    # set cal enable flag
    def setcalflag(self, flag):
        cmd = [0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x27, 0x00, 0x00]
        if(1==flag):
            cmd[10] = 0x12
            cmd[11] = 0x34
        Result,ReadData = self.SocketByteSndRcv(bytes(cmd))
        if(True==Result):
            return True
        else:
            return False

def LSM_for_Polynomial(degree, Xbuffer, Ybuffer, number):
    Xarray = [] # 赋值x_array[]
    for n in range(2 * degree + 1):
        Xarray.append(0)    # x_array数组变量清零

    for n in range(2 * degree + 1):
        for k in range(number):
            t = math.pow(Xbuffer[k], n)
            Xarray[n] += t

    #赋值y_array[]
    Yarray = []
    for n in range(degree + 1):
        Yarray.append(0)    # y_array数组变量清零

    for n in range(degree + 1):
        for k in range(number):
            t = math.pow(Xbuffer[k], n)
            Yarray[n] += t*Ybuffer[k]

    # 列出正规方程式
    matrix = [([0] * (degree+2)) for i in range(degree + 1)]
    # matrix = [[0] * 100] * 100
    for n in range(degree + 1):
        matrix[n][degree + 1] = Yarray[n]
        for k in range(degree + 1):
            matrix[n][k] = Xarray[n + k]

    # 消元
    for n in range(0, degree, 1):
        for c in range(n+1, degree + 1, 1):
            m = -1*matrix[c][n] / matrix[n][n]
            for k in range(n, degree + 2, 1):
                matrix[c][k] = matrix[c][k] + m * matrix[n][k]

    # 迭代
    coeff = [0] * (degree + 1)
    coeff[degree] = matrix[degree][degree + 1] / matrix[degree][degree]
    for n in range (degree -1, -1, -1):
        m = 0
        for k in range(n+1, degree + 1):
            m += coeff[k] * matrix[n][k]
        coeff[n] = (matrix[n][degree + 1] - m) / matrix[n][n]

    print(coeff)
    return coeff

# CalVoltData = [40, 90, 150, 3780, 3790, 3800, 3810, 3820, 3850, 3900, 3950, 4000]
CalVoltData = [10000, 11000, 12000, 13500, 14500, 15500, 16500, 17500, 18000, 18500, 19000, 19500]
CalCurrData1 = [0.01, 0.015, 0.02, 0.025, 0.03, 0.035, 0.04, 0.045, 0.05, 0.06, 0.075, 0.09]
CalCurrData2 = [0.1, 0.13, 0.15, 0.18, 0.2, 0.23, 0.25, 0.27, 0.3, 0.32, 0.35, 0.37]
CalCurrData3 = [0.5, 1, 3, 5, 7, 10, 15, 20, 23, 26, 30, 35]
CalCurrData4 = [50, 80, 120, 160, 200, 230, 260, 280, 300, 320, 350, 380]
CalCurrData5 = [400, 500, 600, 800, 1000, 1200, 1400, 1600, 1800, 2100, 2300, 2500]

def CalProcess(usrsocket:POWRSocket, dmmsocket:DMMSocket, type, currentrange, batname):
    # set cal mode
    result = usrsocket.setworkmode(1)
    if(False==result):
        return False
    # set power mode[fv/fi]
    if(0==type):
        result = usrsocket.setpowrmode(0)
        if(False==result):
            return False
    else:
        result = usrsocket.setpowrmode(1)
        if(False==result):
            return False
    # set current range
    result = usrsocket.setcurrange(currentrange)
    if(False==result):
        return False
    # set output enable
    usrsocket.setEnableOutput(1)

    # init the calibration array
    if(0==type):
        DataArray = CalVoltData
        # init the dmm
        dmmsocket.DC_Volt_Config("100V")
    else:
        if(1==currentrange):
            DataArray = CalCurrData1
            # init the dmm
            dmmsocket.DC_Volt_Config("10V")
        if(2==currentrange):
            DataArray = CalCurrData2
            # init the dmm
            #dmmsocket.DC_Curr_Config(3, "100 mA")
            dmmsocket.DC_Volt_Config("100V")
        if(3==currentrange):
            DataArray = CalCurrData3
            # init the dmm
            dmmsocket.DC_Curr_Config(3, "100 mA")
        if(4==currentrange):
            DataArray = CalCurrData4
            # init the dmm
            dmmsocket.DC_Curr_Config(3, "1A")
        if(5==currentrange):
            DataArray = CalCurrData5
            # init the dmm
            dmmsocket.DC_Curr_Config(3, "3A")

    # loop the cal
    usrReadData = []
    dmmReadData = []
    for i in range(len(DataArray)):
        result = usrsocket.setvoltcurr(DataArray[i])
        if(False==result):
            return False
        sleep(1)
        # board read
        if(0==type):
            result,tempdata = usrsocket.getvoltage()
            if(False==result):
                return False
        else:
            result,tempdata = usrsocket.getcurrent()
            if(False==result):
                return False
        # print(tempdata)
        usrReadData.append(tempdata)

        #dmm read
        tempdata = float(dmmsocket.Read())
        if((1==currentrange) or (2==currentrange)):
            tempdata /= 50000
        dmmReadData.append(tempdata*1000)

    print(usrReadData)
    print(dmmReadData)
    #erchentgfa
    coffdac = LSM_for_Polynomial(3, dmmReadData, DataArray, len(DataArray))
    coffadc = LSM_for_Polynomial(3, usrReadData, dmmReadData, len(DataArray))
    # coffdac.extend(coffadc)
    calcoff = coffdac + coffadc
    print(calcoff)

    # enable write cal para
    result = usrsocket.setcalflag(1)
    if(False==result):
        return False

    if(0==type):
        caltype = 0
    else:
        caltype = currentrange
    result = usrsocket.setcalpara(caltype, calcoff)
    if(False==result):
        return False
    # disable write cal para
    result = usrsocket.setcalflag(0)
    if(False==result):
        return False

    # close output
    usrsocket.setEnableOutput(0)
    usrsocket.setvoltcurr(0)
    usrsocket.setcurrange(0)
    usrsocket.setworkmode(0)

    caldacdata = []
    caladcdata = []

    for j in range(len(DataArray)):
        fcalVolt = 0.0
        for i in range (len(coffdac)):
            fcalVolt += coffdac[i]*math.pow(dmmReadData[j], i)
        caldacdata.append(fcalVolt)
        fcalVolt = 0.0
        for i in range (len(coffadc)):
            fcalVolt += coffadc[i]*math.pow(usrReadData[j], i)
        caladcdata.append(fcalVolt)

    DACERR = []
    DACERR_R = []
    ADCERR = []
    ADCERR_R = []
    CALDACERR = []
    CALDACERR_R = []
    CALADCERR = []
    CALADCERR_R = []

    for i in range(len(DataArray)):
        DACERR.append(DataArray[i] - dmmReadData[i])
        DACERR_R.append(DACERR[i]/DataArray[i])
        ADCERR.append(dmmReadData[i] - usrReadData[i])
        ADCERR_R.append(ADCERR[i]/dmmReadData[i])
        CALDACERR.append(DataArray[i] - caldacdata[i])
        CALDACERR_R.append(CALDACERR[i]/DataArray[i])
        CALADCERR.append(dmmReadData[i] - caladcdata[i])
        CALADCERR_R.append(CALADCERR[i]/dmmReadData[i])

    df = pd.DataFrame({'SET':DataArray, 'BOARD':usrReadData, 'DMM':dmmReadData, 'DACERR':DACERR, 'DACERR_R':DACERR_R, \
        'ADCERR':ADCERR, 'ADCERR_R':ADCERR_R, 'caldacdata':caldacdata, 'caladcdata':caladcdata, 'CALDACERR':CALDACERR, \
            'CALDACERR_R':CALDACERR_R, 'CALADCERR':CALADCERR, 'CALADCERR_R':CALADCERR_R})
    t1 = time.strftime("_%m%d%H%M%S", time.localtime())
    df.to_csv(batname + t1 + ".csv",index=False)

def CheckProcess(usrsocket:POWRSocket, dmmsocket:DMMSocket, type, currentrange, batname):
    # set normal mode
    result = usrsocket.setworkmode(0)
    if(False==result):
        return False
    # set power mode[fv/fi]
    if(0==type):
        result = usrsocket.setpowrmode(0)
        if(False==result):
            return False
    else:
        result = usrsocket.setpowrmode(1)
        if(False==result):
            return False
    # set current range
    result = usrsocket.setcurrange(currentrange)
    if(False==result):
        print("set curr error")
        return False
    # set output enable
    usrsocket.setEnableOutput(1)

    # init the calibration array
    if(0==type):
        DataArray = CalVoltData
        # init the dmm
        dmmsocket.DC_Volt_Config("100V")
    else:
        if(1==currentrange):
            DataArray = CalCurrData1
            # init the dmm
            dmmsocket.DC_Volt_Config("10V")
        if(2==currentrange):
            DataArray = CalCurrData2
            # init the dmm
            dmmsocket.DC_Volt_Config("100V")
        if(3==currentrange):
            DataArray = CalCurrData3
            # init the dmm
            dmmsocket.DC_Curr_Config(3, "100 mA")
        if(4==currentrange):
            DataArray = CalCurrData4
            # init the dmm
            dmmsocket.DC_Curr_Config(3, "1A")
        if(5==currentrange):
            DataArray = CalCurrData5
            # init the dmm
            dmmsocket.DC_Curr_Config(3, "3A")

    # loop the cal
    usrReadData = []
    dmmReadData = []
    for i in range(len(DataArray)):
        result = usrsocket.setvoltcurr(DataArray[i])
        if(False==result):
            print("set 1 error")
            return False
        sleep(1)
        # board read
        if(0==type):
            result,tempdata = usrsocket.getvoltage()
            if(False==result):
                print("set 2 error")
                return False
        else:
            result,tempdata = usrsocket.getcurrent()
            if(False==result):
                return False
        usrReadData.append(tempdata)
        #dmm read
        tempdata = float(dmmsocket.Read())
        if((1==currentrange) or (2==currentrange)):
            tempdata /= 50000
        dmmReadData.append(tempdata*1000)

    # close output
    usrsocket.setEnableOutput(0)
    usrsocket.setvoltcurr(0)
    usrsocket.setcurrange(0)
    usrsocket.setworkmode(0)

    print(usrReadData)
    print(dmmReadData)

    DACERR = []
    DACERR_R = []
    ADCERR = []
    ADCERR_R = []

    for i in range(len(DataArray)):
        DACERR.append(DataArray[i] - dmmReadData[i])
        DACERR_R.append(DACERR[i]/DataArray[i])
        ADCERR.append(dmmReadData[i] - usrReadData[i])
        ADCERR_R.append(ADCERR[i]/dmmReadData[i])

    df = pd.DataFrame({'SET':DataArray, 'BOARD':usrReadData, 'DMM':dmmReadData, 'DACERR':DACERR, 'DACERR_R':DACERR_R, \
        'ADCERR':ADCERR, 'ADCERR_R':ADCERR_R})
    t1 = time.strftime("_%m%d%H%M%S", time.localtime())
    df.to_csv(batname + t1 + ".csv",index=False)
    
DMM_IP   = "192.168.2.66"
DMM_PORT = 5025
PWR_IP   = "192.168.2.199"
PWR_PORT = 5000

def mode_case(argument):
    switcher = {
        1: "Calibration ",
        0: "Check ",
    }
    return switcher.get(argument, "Invalid argument")

def type_case(argument):
    switcher = {
        0: "Voltage ",
        1: "Current ",
    }
    return switcher.get(argument, "Invalid argument")

def range_case(argument):
    switcher = {
        1: "in 100uA range",
        2: "in 400uA range",
        3: "in 40mA range",
        4: "in 400mA range",
        5: "in 3000mA range",
    }
    return switcher.get(argument, "Invalid argument")

# 主函数
# if __name__ == '__main__':
def main():
    mode = int(sys.argv[1])
    type = int(sys.argv[2])
    currentrange = int(sys.argv[3])
    batname = sys.argv[4]
    while(1):
        infostr = "It will %s%s%s" %(mode_case(mode), type_case(type), range_case(currentrange))
        print(infostr)
        print("输入q退出,其他继续:", end="")
        TempCmd = input()
        if('q'==TempCmd):
            return True

        keysightdmm = DMMSocket(DMM_IP, DMM_PORT)
        keysightdmm.GetIDN()
        pwrboad = POWRSocket(PWR_IP, PWR_PORT)

        if(0==mode):
            CheckProcess(pwrboad, keysightdmm, type, currentrange, batname)
        elif(1==mode):
            CalProcess(pwrboad, keysightdmm, type, currentrange, batname)
    
        keysightdmm.DeInit()
        pwrboad.DeInit()

if __name__ == '__main__':
    main()
    print("执行结束,请查看当前目录下csv文件")