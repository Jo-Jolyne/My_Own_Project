**CalibrationProgram_V1.0说明**

**1.使用Python_pymodbus库实现开关切换功能,相较于普通的socket程序来说更为简单;**

**2.将ModBusTCP的**

   **01(读取线圈寄存器),03(读取保持寄存器),04(读取输入寄存器),05(写单个线圈寄存器)**

   **0F(写多个线圈寄存器),06(写单个保持寄存器),10(写多个保持寄存器)**

   **功能码进行了函数封装(其实直接调用pymodbus库中的相关函数即可,不必这么麻烦).**



**CalibrationProgram_V2.0说明**

**优化了==CalibrationProgram_V1.0==的脚本程序,去除了多余的函数封装,以较为简洁的代码实现基本功能**



**CalibrationProgram_V2.1说明**

**在==CalibrationProgram_V2.0==脚本程序的基础上,开放了各个设备的==IP地址==和==端口号==,并以CSV文件的形式保存,若==IP地址==或者==端口号==有变动,只需修改并保存配置文件即可**



**CalibrationProgram_V2.2说明**

**在==CalibrationProgram_V2.1==脚本程序的基础上,增加了KeySight34465A数字万用表的控制.已通过以太网成功连接并读取该数字万用表的仪器标识符.**

