
#BSC(Broadcom Serial Controller:博通串行控制器)</br>
----
### IIC Adddress Map

|	Adddress offset	|	Register	|		Description		|	size	|
|:---------------:|:---------------:|:---------------------:|:---------:|
|		0x00		|		c		|		Control			|	32		|
|		0x04		|		s		|		Status			|	32		|
|		0x08		|		DLEN	|		Data Length		|	32		|
|		0x0C		|		A		|		Slave Adddress	|	32		|
|		0x10		|		FIFO	|		Data FIFO		|	32		|
|		0x14		|		DIV		|		Clock Divider	|	32		|
|		0x18		|		DEL		|		Data Delay		|	32		|
|		0x1C		|		CLKT	|		Clock Stretch	|	32		|

### C Register
这个寄存器的作用：启动中断，清除FIFO，定义读、写操作并，开始传输等。永远读为0</br>

|	Bit(s)	|	Field Name	|		Description			|	Type	|	Reset	|
|:---------:|:-------------:|:-------------------------:|:---------:|:---------:|
|	31-16	|				|			保留				|	-		|	-		|
|	15		|		I2CEN	|		I2C Enable			|	RW		|	0x00	|
|	14-11	|				|			保留				|	-		|	-		|
|	10		|		INTR	|	INTR Interrupt on RX	|	RW		|	0X00	|
|	9		|		INTT	|	INTT Interrupt on TX	|	RW		|	0X00	|
|	8		|		INTD	|	INTD Interrupt on DONE	|	RW		|	0X00	|
|	7		|		ST		|	ST Start Transfer		|	RW		|	0x00	|
|	6		|				|			保留				|	RW		|	0x00	|
|	5-4		|		CLEAR	|			FIFO Clear		|	RW		|	0x00	|
|	3-1		|				|			保留				|	RW		|	0x00	|
|	0		|		READ	|			Read Transfer	|	RW		|	0x00	|

**I2CEN**是I2C的允许位。0：不允许，1：允许。</br>
**INTR**读取中断允许位。0：不允许，1：允许。</br>
**INTT**写入中断允许位。0：不允许，1：允许。</br>
**INTD**传输位中断允许位。0：不允许，1：允许</br>
**ST**启动一个传输。0：不允许，1：允许。</br>
**CLEAR**清除位，用于清除FIFO。可以同时设置为开始，并将导致FIFO在传输开始前被清除。传输过程中清除FIFO将导致传输被中止。</br>
**READ**指定传输的类型。</br>

### S Register
这个寄存器是状态状态寄存器用于记录活动状态、错误和中断请求</br>

|	Bit(s)	|	Field Name	|		Description			|	Type	|	Reset	|
|:---------:|:-------------:|:-------------------------:|:---------:|:---------:|
|	31-10	|				|			保留				|	-		|	-		|
|	9		|		CLKT	|CLKT Clock Stretch Timeout |	RW		|	0x00	|
|	8		|		ERR		|		ERR ACK Error		|	RW		|	0X00	|
|	7		|		RXF		|		FIFO Full			|	RO		|	0x00	|
|	6		|		TXE 	|		FIFO Empty			|	RO		|	0x00	|
|	5		|		RXD 	|	FIFO contains Data		|	RO		|	0x00	|
|	4		|		TXD 	|	FIFO can accept Data 	|	RO		|	0x00	|
|	3		|		RXR 	|FIFO needs Reading(full)	|	RO		|	0x00	|
|	2		|		TXW 	|FIFO needs Writing(full)	|	RO		|	0x00	|
|	1		|		DONE 	|	DONE Transfer Done		|	RW		|	0x00	|
|	0		|		TA	 	|	TA Transfer Active		|	RO		|	0x00	|


**DONE**INTD中断重置,与**C Register**的INTD一起使用。在INTD中断发生时，写入1重置中断。写0无效果</br>
**TA**记录活动状态。只读。1：表示正在传输，0：空闲</br>
**TXW**写传输过程中使用，只读。表示需要将更多的数据写入到FIFO中。 **C Register**的INTT设置后可产生中断。</br>
**RXR**读取传输过程中使用，只读。表示FIFO数据需要读取。 **C Register**的INTR设置后可产生中断。</br>
**TXD**在FIFO中有至少有一个字节的数据时被置位。在FIFO装满后，TXD被清空。</br>
**RXD**当FIFO包含至少一个字节的数据时，将设置只读的RXD字段。当FIFO变为空时，RXD被清除。</br>
**TXE**在FIFO为空时被置位</br>
**RXD**在FIFO为满的时候被置位。在没有更多的数据写入FIFO之前。不会再产生时钟信号</br>
**ERR**从机不响应地址和数据时，将置位</br>
**CLKT**从机长时间拉高SCL信号时( *开始传输？*)，将置位，CLKT字段通过写1来重置，写0无效果。

###DLEN Register

|	Bit(s)	|	Field Name	|		Description			|	Type	|	Reset	|
|:---------:|:-------------:|:-------------------------:|:---------:|:---------:|
|	31-16	|				|			保留				|	-		|	-		|
|	15-0	|	  DLEN		|		Data Length			|	RW		|	0x00	|

**DLEN**指定传输的字节数。

### A Register

|	Bit(s)	|	Field Name	|		Description			|	Type	|	Reset	|
|:---------:|:-------------:|:-------------------------:|:---------:|:---------:|
|	31-7		|				|			保留				|	-		|	-		|
|	6-0		|		ADDR	|		Slave Address		|	RW		|	0x00	|

**ADDR**为从机在IIC总线上的地址</br>

### FIFO Register

|	Bit(s)	|	Field Name	|		Description			|	Type	|	Reset	|
|:---------:|:-------------:|:-------------------------:|:---------:|:---------:|
|	31-8		|				|			保留				|	-		|	-		|
|	7-0		|		DATA	|			Data			|	RW		|	0x00	|

**Data**为要传送的数据</br>

###DIV Register

|	Bit(s)	|	Field Name	|		Description			|	Type	|	Reset	|
|:---------:|:-------------:|:-------------------------:|:---------:|:---------:|
|	31-16	|				|			保留				|	-		|	-		|
|	15-0	|	CDIV 		|		Clock Divider		|	RW		|	0x5dc	|

**CDIV** 时钟分频器,SCL = core clock / CDIV。core clock一般是150MHZ。如果CDIV设置为0。SCL为32.768k
如果CDIV选择默认的0x5dc，则SCL是100kHz</br>

###DEL Register

该寄存器主要是设置时钟的延时

|	Bit(s)	|	Field Name	|		Description			|	Type	|	Reset	|
|:---------:|:-------------:|:-------------------------:|:---------:|:---------:|
|	31-16	|	FEDL 		|	FEDL Falling Edge Delay	|	RW		|	0x30	|
|	15-0	|	REDL 		|	REDL Rising Edge Delay	|	RW		|	0x30	|

**FEDL**在输出一个比特数据之前的下降沿后等待的时钟周期(core clock)数</br>
**REDL**在读取一个比特数据之前的上升沿后等待的时钟周期(core clock)数</br>

###CLKT Register

|	Bit(s)	|	Field Name	|			Description		   	 |	Type	|	Reset	|
|:---------:|:-------------:|:------------------------------:|:--------:|:---------:|
|	31-16	|				|				保留				 |		-	|	-		|
|	15-0	|		TOUT  	|TOUT Clock Stretch Timeout Value|		RW	|	0x40	|

**TOUT**SCL上升沿后等待从机的应答的时间数(SCL时钟)</br>



#IIC
--- 

###特殊的状态 

+ **空闲状态** </br>
	IIC总线的SDA和SCL同时处于高电平时，就为空闲状态
+ **起始位**</br>
	当SCL为高时，SDA由高到低跳变。先改变SCL，再改变SDA。在数据传输过程中先改变SDA，再改变SCL。
+ **停止位**</br>
	当SCL为高时，SDA由低到高跳变。先改变SCL，再改变SDA。在数据传输过程中先改变SDA，再改变SCL。
+ **ACK**</br>
	发送器每发送一个字节，就再脉冲9期间释放数据线，由接收器反馈一个应答信号。 应答信号为低电平时，规定为有效应答位（ACK简称应答位），表示接收器已经成功地接收了该字节。应答信号为高电平时，规定为非应答位（NACK），一般表示接收器接收该字节没有成功。对于反馈有效应答位ACK的要求是，接收器在第9个时钟脉冲之前的低电平期间将SDA线拉低，并且确保在该时钟的高电平期间为稳定的低电平。 如果接收器是主控器，则在它收到最后一个字节后，发送一个NACK信号，以通知被控发送器结束数据发送，并释放SDA线，以便主控接收器发送一个停止信号P。

###数据的传输
 
IIC的设备地址是7bit的，而IIC总线上一次传输是8bit。在传输设备地址时，设备地址放在传输的高7位，最后的一个bit是读写位。
所以，一般一个设备在IIC总线上有两个设备地址，分为读地址和写地址，这是因为把读写位算了进去，使用8bit表示的原因。

 **主机向从机写入数据**

1. 主机发送起始位，这会通知IIC总线上的所有设备传输开始。
2. 接下来主机发送从机的设备地址(写地址)。
3. 总线上各个设备对从机的设备地址进行匹配，不匹配的进行忽略后面的传输。匹配的从机发送ACK应答
4. 主机发送从机的寄存器地址
5. 从机应答一个ACK
6. 主机发送数据
7. 从机接受到数据，反馈一个ACK
8. 主机发送结束位

**主机读取从机数据**

1. 主机发起始位。
2. 发送从机的设备地址(写地址)
3. 发送从机的内部寄存器地址
4. 重新发送起始位信号。
5. 发从机的设备地址(读地址)
6. 读取数据，发送NACK应答，使从机释放SDA线(为了发送结束位)
7. 发送结束位
