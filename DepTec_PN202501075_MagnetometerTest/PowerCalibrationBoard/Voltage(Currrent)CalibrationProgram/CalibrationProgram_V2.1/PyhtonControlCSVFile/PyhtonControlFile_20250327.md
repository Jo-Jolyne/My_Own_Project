```python
# coding=utf-8
import csv

file_instance = open("ConfigFile.csv", encoding = "utf-8-sig")

csv_reader = csv.DictReader(file_instance)

ConfigData = [line for line in csv_reader]

file_instance.close()

DMM_IP = ConfigData[2]['IP']       # 数字万用表IP地址
DMM_PORT = ConfigData[2]['PORT']   # 数字万用表端口号
print(f"DMM_IP = {DMM_IP}, DMM_PORT = {DMM_PORT}")
CAL_IP = ConfigData[0]['IP']       # 磁力计校准板IP地址
CAL_PORT = ConfigData[0]['PORT']   # 磁力计校准板端口号
print(f"CAL_IP = {CAL_IP}, CAL_PORT = {CAL_PORT}")
PWR_IP = ConfigData[1]['IP']       # 磁力计电源板IP地址
PWR_PORT = ConfigData[1]['PORT']   # 磁力计电源板端口号
print(f"PWR_IP = {PWR_IP}, PWR_PORT = {PWR_PORT}")
```