# coding=utf-8
import socket
from time import sleep
from pymodbus.client import ModbusTcpClient
from pymodbus.exceptions import ModbusIOException

# 先将电源校准板的脚本进行优化
CAL_IP = "192.168.2.199" # 磁力计校准板IP地址
CAL_PORT = 5000          # 磁力计校准板端口号

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


