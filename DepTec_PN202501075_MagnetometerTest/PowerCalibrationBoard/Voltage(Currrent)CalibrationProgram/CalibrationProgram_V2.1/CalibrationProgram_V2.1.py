# coding=utf-8
#声明全局变量DMM_IP, DMM_PORT, CAL_IP, CAL_PORT, PWR_IP, PWR_PORT
DMM_IP = ""
DMM_PORT = ""
CAL_IP = ""
CAL_PORT = ""
PWR_IP = ""
PWR_PORT = ""

import csv
from time import sleep
from pymodbus.client import ModbusTcpClient
from pymodbus.exceptions import ModbusIOException

class DataInit:
    @staticmethod
    def GetDevConfigData():
        global DMM_IP, DMM_PORT, CAL_IP, CAL_PORT, PWR_IP, PWR_PORT  # 声明为全局变量

        file_instance = open("ConfigFile.csv", encoding = "utf-8-sig")

        csv_reader = csv.DictReader(file_instance)

        ConfigData = [line for line in csv_reader]

        file_instance.close()

        DMM_IP = ConfigData[2]['IP']       # 数字万用表IP地址
        DMM_PORT = ConfigData[2]['PORT']   # 数字万用表端口号

        CAL_IP = ConfigData[0]['IP']       # 磁力计校准板IP地址
        CAL_PORT = ConfigData[0]['PORT']   # 磁力计校准板端口号

        PWR_IP = ConfigData[1]['IP']       # 磁力计电源板IP地址
        PWR_PORT = ConfigData[1]['PORT']   # 磁力计电源板端口号


class CalibrationBoardProcess:
    def __init__(self, ip, port):
        self.client = ModbusTcpClient(host = ip, port = port)
        try:
            if self.client.connect():
                print(f"Connect to {ip} on port {port} OK")
            else:
                raise ConnectionError(f"Failed to connect to {ip} on port {port}")
        except ModbusIOException as e:
            print(f"Modbus connection error: {e}")
        except Exception as e:
            print(f"Error occurred: {e}")

    # 读取校准板固件号函数
    def GetID(self):
        try:
            Response = self.client.read_input_registers(address = 0, count = 2, slave = 1)
            if not Response.isError():
                id_string = ''.join(chr(register >> 8) + chr(register & 0xFF) for register in Response.registers)
                print("Firmware ID:", id_string)
            else:
                print("Failed to read Firmware ID.")
        except Exception as e:
            print(f"Error occurred while reading Firmware ID: {e}")

    # 校准板各档位开关切换函数
    def SwitchControl(self, type, currentrang = None):
        try:
            # 先执行初始化操作,关闭所有开关
            self.client.write_coils(address = 0, values = [False, False, False, False, False, False], slave = 1)
            
            if (type == 0):
                self.client.write_coil(address = 0, value = True, slave = 1)
                self.client.write_coil(address = 5, value = True, slave = 1)
            elif (type == 1):
                # 确保传入了currentrang
                if currentrang is None:
                    raise ValueError("currentrang must be provided when type is not 0.") 
                if (currentrang == 1):
                    # 小电流量程μA档位开关控制
                    self.client.write_coil(address = 1, value = True, slave = 1)
                    self.client.write_coil(address = 5, value = True, slave = 1)
                elif (currentrang == 2):
                    # 小电流量程40mA档位开关控制
                    self.client.write_coil(address = 1, value = True, slave = 1)
                    self.client.write_coil(address = 4, value = True, slave = 1)
                elif (currentrang == 3):
                    # 小电流量程400mA档位开关控制
                    self.client.write_coil(address = 1, value = True, slave = 1)
                    self.client.write_coil(address = 3, value = True, slave = 1)
                elif (currentrang == 4):
                    # 小电流量程1000mA档位开关控制
                    self.client.write_coil(address = 1, value = True, slave = 1)
                    self.client.write_coil(address = 2, value = True, slave = 1)
                else:
                    print(f"Invalid currentrang value:{currentrang}")
            else:
                print(f"Invalid type value:{type}")
        except Exception as e:
            print(f"Exception occurred:{e}") 

    # 结束校准板控制函数
    def Close(self):
        # 断开所有开关
        self.client.write_coils(address = 0, values = [False, False, False, False, False, False], slave = 1)
        sleep(0.5)
        self.client.close()


def main():
    DataInit.GetDevConfigData()
    print(f"CAL_IP = {CAL_IP}, CAL_PORT = {CAL_PORT}")
    CalBoard = CalibrationBoardProcess(CAL_IP, CAL_PORT)
    sleep(1)
    CalBoard.GetID()
    sleep(1)
    CalBoard.SwitchControl(0)
    sleep(1)
    for i in range(4):
        CalBoard.SwitchControl(1, (i + 1))
        sleep(1)
    CalBoard.Close()

if __name__ == "__main__":
    main()



