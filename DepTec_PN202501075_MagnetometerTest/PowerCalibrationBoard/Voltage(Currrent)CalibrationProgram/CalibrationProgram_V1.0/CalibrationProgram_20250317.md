```python
# coding=utf-8
from time import sleep
from pymodbus.client import ModbusTcpClient
from pymodbus.exceptions import ModbusIOException

# DMM_IP = "192.168.2.66"  # 数字万用表IP地址
# DMM_PORT = 5025          # 数字万用表端口号
CAL_IP = "192.168.2.199" # 磁力计校准板IP地址
CAL_PORT = 5000          # 磁力计校准板端口号
# PWR_IP = "192.168.2.199" # 磁力计电源板IP地址
# PWR_PORT = 5000          # 磁力计电源板端口号


class MySocketDev: #定义了一个类MySocketDev,它用于表示一个自定义的套接字设备.
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

    # 读取线圈寄存器(01功能码)方法
    def ModBusTCP_Read_CoilData(self, reg, reglen, DeviceID):
        try:
            Response = self.client.read_coils(address = reg, count = reglen, slave = DeviceID)
            if not Response.isError():
                return [hex(register) for register in Response.registers]
            else:
                return "Failed to read registers"
        except Exception as e:
            return f"Error occurred: {e}"

    # 读取保持寄存器(03功能码)方法
    def ModBusTCP_Read_HoldData(self, reg, reglen, DeviceID):
        try:
            Response = self.client.read_holding_registers(address = reg, count = reglen, slave = DeviceID)
            if not Response.isError():
                return [hex(register) for register in Response.registers]
            else:
                return f"Failed to read input registers starting from address {reg} for {reglen} registers"
        except Exception as e:
            return f"Error occurred while reading input registers starting from address {reg}: {e}"

    # 读取输入寄存器(04功能码)方法
    def ModBusTCP_Read_InputData(self, reg, reglen, DeviceID):
        try:
            Response = self.client.read_input_registers(address = reg, count = reglen, slave = DeviceID)
            if not Response.isError():
                return [hex(register) for register in Response.registers]
            else:
                return f"Failed to read input registers starting from address {reg} for {reglen} registers"
        except Exception as e:
            return f"Error occurred while reading input registers starting from address {reg}: {e}"

    # 写多个(单个)线圈寄存器(05,0F功能码)方法
    def ModBusTCP_Write_CoilData(self, reg, value, DeviceID):
        try:
            Response = self.client.write_coils(address = reg, values = value, slave = DeviceID)
            if not Response.isError():
                return True
            else:
                return False
        except Exception as e:
            print(f"Error writing coil:{e}")
            return True

    # 写多个(单个)保持寄存器(06,10功能码)方法
    def ModBusTCP_Write_HoldData(self, reg, values, DeviceID):
        try:
            Response = self.client.write_registers(address = reg, values = values, slave = DeviceID)
            if not Response.isError():
                return True
            else:
                return False
        except Exception as e:
            print(f"Error writing holding register:{e}")
            return True

    def CloseSocket(self):
        print("SocketDev Close OK")
        self.client.close()


class CALSocket(MySocketDev):
    def GetID(self):
        result = self.ModBusTCP_Read_InputData(0, 2, 1) # 读取校准板程序固件号
        if isinstance(result, list):
            id_string = ''.join(chr(int(register, 16) >> 8) + chr(int(register, 16) & 0xFF) for register in result)
            print("Firmware ID:", id_string)
        else:
            print(result)

    """
    def InputRegister_CTRL(self, reg, reglen, DeviceID):
        if 0x10 <= reg <= 0x1F:
            result = self.ModBusTCP_Read_InputData(reg, reglen, DeviceID) # 读取输入寄存器中的数据
            if isinstance(result, list):
                print("Register(s) Values:", result)
            else:
                print(result)
        else:
            print("Address Error")


    def HoldRegister_CTRL(self, flag, reg, values, DeviceID):
        if not (0x20 <= reg <= 0x2F):
            print("Address Error")
            return
        if flag:
            if self.ModBusTCP_Write_HoldData(reg, values, DeviceID):
                print("Writing data successfully.")
            else:
                print("Writing data failed.")
        elif values <= 0x2F and (reg + values) <= 0x2F:
            result = self.ModBusTCP_Read_HoldData(reg, values, DeviceID)
            print(result if isinstance(result, str) else "Register(s) Values:", result)
        else:
            print("Address Error")

    def CoilRegister_CTRL(self, reg, value, DeviceID):
    """


    def DMM_VOL_CTRL(self, state):
        result = self.ModBusTCP_Write_HoldData(0x20, state, 1)
        if result:
            print("DMM_VOL_CTRL OK.")
        else:
            print("DMM_VOL_CTRL Failed.")


    def DMM_CUR_CTRL(self, state):
        result = self.ModBusTCP_Write_HoldData(0x21, state, 1)
        if result:
            print("DMM_CUR_CTRL OK.")
        else:
            print("DMM_CUR_CTRL Failed.")


    def RES_CTRL(self, num):
        result = self.ModBusTCP_Write_HoldData(0x22, num, 1)
        if result:
            print("RES_CTRL OK.")
        else:
            print("RES_CTRL Failed.")


def main():
    calboard = CALSocket(CAL_IP,CAL_PORT)
    sleep(1)
    calboard.GetID()
    sleep(1)
    calboard.DMM_VOL_CTRL([0x0100])
    sleep(2)
    calboard.DMM_VOL_CTRL([0x0000])
    sleep(2)
                        
    for i in range(4):
        calboard.RES_CTRL([0x0100 * (i + 1)])
        sleep(1)

    calboard.RES_CTRL([0x0000])

    calboard.CloseSocket()

# 只有在直接运行该文件时才调用 main 函数
if __name__ == "__main__":
    main()

```

