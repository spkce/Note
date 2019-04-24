#SPI
<font color=#fff0000 size=3>*本文中的关于树莓派的寄存器来自于BCM2835的datasheet，树莓派3B的控制器并不是BCM2835*</font>

BCM2835只有一个SPI接口(SPI0)，另有2个mini SPI(SPI1和SPI2)

SPI0的物理地址是 0x7E204000

###SPI寄存器

|	Adddress offset	|	Register	|		Description			   |	size	|
|:-----------------:|:-------------:|:----------------------------:|:----------:|
|		0x00		|		CS		|SPI Master Control and Status |	32		|
|		0x04		|		FIFO	|	SPI Master TX and RX FIFOs |	32		|
|		0x08		|		CLK		|	SPI Master Clock Divider   |	32		|
|		0x0C		|		DLEN	|	SPI Master Data Length     |	32		|
|		0x10		|		LTOH	|	SPI LOSSI mode TOH 		   |	32		|
|		0x14		|		DC		|	SPI DMA DREQ Controls	   |	32		|

###CS Register

|	Bit(s)	|Field Name	|			Description				|	Type	|	Reset	|
|:---------:|:---------:|:---------------------------------:|:---------:|:---------:|
|	31-26	|			|				Reserved 			|	-		|	-		|
|	25		|   LEN_LONG|			Enable Long data		|	RW		|	0x0		|
|	24		|	DMA_LEN |	Enable DMA mode in Lossi mode	|	RW		|	0x0		|
|	23		|	CSPOL2  |		Chip Select 2 Polarity		|	RW		|	0x0		|
|	22		|	CSPOL1 	|		Chip Select 1 Polarity		|	RW		|	0x0		|
|	21		|	CSPOL0 	|		Chip Select 0 Polarity		|	RW		|	0x0		|
|	20		|	RXF 	| 			RX FIFO Full			|	RO		|	0x0		|
|	19		|	RXR 	|	RX FIFO needs Reading(full)		|	RO		|	0x0		|
|	18		|	TXD 	|	TX FIFO can accept Data			|	RO		|	0x1		|
|	17		|	RXD 	|	RX FIFO contains Data			|	RO		|	0x0		|
|	16		|	DONE 	|			transfer Done			|	RO		|	0x0		|
|	15		|	TE_EN 	|				Unused				|	RW		|	0x0		|
|	14		|	LMONO 	|				Unused				|	RW		|	0x0		|
|	13		|	LEN 	|			LEN LoSSI enable		|	RW		|	0x0		|
|	12		|	REN 	|			REN Read Enable			|	RW		|	0x1		|
|	11		|	ADCS 	|Automatically Deassert Chip Select |	RW		|	0x0		|
|	10		|	INTR 	| 			Interrupt on RXR		|	RW		|	0x0		|
|	9		|	INTD 	| 			Interrupt on Done		|	RW		|	0x0		|
|	8		|	DMAEN 	| 			DMA Enable				|	RW		|	0x0		|
|	7		|	TA 		|			Transfer Active			|	RW		|	0x0		|
|	6		|	CSPOL 	|		Chip Select Polarity		|	RW		|	0x0		|
|	5-4		|	CLEAR 	|		 FIFO Clear					|	RW		|	0x0		|
|	3		|	CPOL 	|		Clock Polarity				|	RW		|	0x0		|
|	2		|	CPHA 	|		Clock Phase					|	RW		|	0x0		|
|	1		|	CS 		|		Chip Select					|	RW		|	0x0		|

**TXD** 0:TX FIFO满了,不能再接受数据;1:能接受至少1个byte的数据。</br>
**DONE**传输完成标志位。0:传输正在进行;1:传输完成，之后可以写更多的数据到FIFO中或者使TA为0</br>
**TA** TA=0时不进行传输。cs线为高（CS,CSPOL决定，假设CSPOL=0）RXR和DONE为0。 </br>
&#8194;&#8194;&#8194;&#8194;TA=1时，传输活动。cs线会被设置。可以认为TA最主要的功能是设置cs线</br>

###FIFO Register

|	Bit(s)	|Field Name	|		Description			|Type|	Reset	|
|:---------:|:---------:|:-------------------------:|:--:|:--------:|
|	31-0	|	DATA	|							| RW |	0X00	|

###CLK Register

|	Bit(s)	|Field Name	|		Description			|Type|	Reset	|
|:---------:|:---------:|:-------------------------:|:--:|:--------:|
|	31-16	|			|		Reserved 			| -  |	-		|
|	15-0	|	CDIV	|		Clock Divider		| RW |	0X00	|

###DLEN Register

|	Bit(s)	|Field Name	|		Description			|Type|	Reset	|
|:---------:|:---------:|:-------------------------:|:--:|:--------:|
|	31-16	|			|		Reserved 			| -  |	-		|
|	15-0	|	LEN		|		Data Length			| RW |	0X00	|

###LTOH Register

|	Bit(s)	|Field Name	|		Description			|Type|	Reset	|
|:---------:|:---------:|:-------------------------:|:--:|:--------:|
|	31-16	|			|		Reserved 			| -  |	-		|
|	15-0	|	TOH		|							| RW |	0X01	|

###DC Register

|	Bit(s)	|Field Name	|		Description			|Type|	Reset	|
|:---------:|:---------:|:-------------------------:|:--:|:--------:|
|	31-24	|	RPANIC	|DMA Read Panic Threshold	| RW |	0X30	|
|	23-16	|	RDREQ 	|DMA Read Request Threshold	| RW |	0X20	|
|	15-8	|	TPANIC 	|DMA Write Panic Threshold  | RW |	0X10	|
|	7-0		|	TDREQ 	|DMA Write Request Threshold| RW |	0X20	|
