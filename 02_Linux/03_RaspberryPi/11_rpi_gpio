#GPIO

<font color=#fff0000 size=3>*本文中的关于树莓派的寄存器来自于BCM2835的datasheet，树莓派3B的控制器并不是BCM2835*</font>

|	Adddress |	Register	|		Description						|	size	|	Type	|
|:----------:|:------------:|:-------------------------------------:|:---------:|:---------:|
|0x7E20 0000 |	GPFSEL0		| 			GPIO Function Select 0		|	32		|	R/W		|
|0x7E20 0004 |	GPFSEL1		| 			GPIO Function Select 1		|	32		|	R/W		|
|0x7E20 0008 |	GPFSEL2		|			GPIO Function Select 2		|	32		|	R/W		|
|0x7E20 000C |	GPFSEL3		| 			GPIO Function Select 3		|	32		|	R/W		|
|0x7E20 0010 |	GPFSEL4		| 			GPIO Function Select 4		|	32		|	R/W		|
|0x7E20 0014 |	GPFSEL5		| 			GPIO Function Select 5		|	32		|	R/W		|
|0x7E20 0018 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 001C |	GPSET0		| 			GPIO Pin Output Set 0 		|	32		|	W		|
|0x7E20 0020 |	GPSET1		| 			GPIO Pin Output Set 1 		|	32		|	W		|
|0x7E20 0024 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 0028 |	GPCLR0		| 			GPIO Pin Output Clear 0 	|	32		|	W		|
|0x7E20 002C |	GPCLR1		| 			GPIO Pin Output Clear 1 	|	32		|	W		|
|0x7E20 0030 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 0034 |	GPLEV0		| 			GPIO Pin Level 0 			|	32		|	R		|
|0x7E20 0038 |	GPLEV1		| 			GPIO Pin Level 1 			|	32		|	R		|
|0x7E20 003C |		-		| 				Reserved				|	-		|	-		|
|0x7E20 0040 |	GPEDS0		| 		GPIO Pin Event Detect Status 0  |	32		|	R/W		|
|0x7E20 0044 |	GPEDS1		| 		GPIO Pin Event Detect Status 1  |	32		|	R/W		|
|0x7E20 0048 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 004C |	GPREN0		| GPIO Pin Rising Edge Detect Enable 0 	|	32		|	R/W		|
|0x7E20 0050 |	GPREN1		| GPIO Pin Rising Edge Detect Enable 1 	|	32		|	R/W		|
|0x7E20 0054 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 0058 |	GPFEN0		| GPIO Pin Falling Edge Detect Enable 0 |	32		|	R/W		|
|0x7E20 005C |	GPFEN1		| GPIO Pin Falling Edge Detect Enable 1 |	32		|	R/W		|
|0x7E20 0060 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 0064 |	GPHEV0		| 	GPIO Pin High Detect Enable 0 		|	32		|	R/W		|
|0x7E20 0068 |	GPHEV1		| 	GPIO Pin High Detect Enable 1 		|	32		|	R/W		|
|0x7E20 006C |		-		| 				Reserved				|	-		|	-		|
|0x7E20 0070 |	GPLEV0		| 	GPIO Pin Low Detect Enable 0 		|	32		|	R/W		|
|0x7E20 0074 |	GPLEV1		| 	GPIO Pin Low Detect Enable 1 		|	32		|	R/W		|
|0x7E20 0078 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 007C |	GPAREN0		| GPIO Pin Async. Rising Edge Detect 0	|	32		|	R/W		|
|0x7E20 0080 |	GPAREN1		| GPIO Pin Async. Rising Edge Detect 1	|	32		|	R/W		|
|0x7E20 0084 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 0088 |	GPAFEN0		| GPIO Pin Async. Falling Edge Detect 0	|	32		|	R/W		|
|0x7E20 008C |	GPAFEN1		| GPIO Pin Async. Falling Edge Detect 1	|	32		|	R/W		|
|0x7E20 0090 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 0094 |	GPPUD		| 	GPIO Pin Pull-up/down Enable 		|	32		|	R/W		|
|0x7E20 0098 |	GPPUDCLK0	| GPIO Pin Pull-up/down Enable Clock 0 	|	32		|	R/W		|
|0x7E20 009C |	GPPUDCLK1	| GPIO Pin Pull-up/down Enable Clock 1 	|	32		|	R/W		|
|0x7E20 00A0 |		-		| 				Reserved				|	-		|	-		|
|0x7E20 00B0 |		-		| 				Test					|	4		|	R/W		|
###GPFSEL0

|	Bit(s)  |	Field Name	|		Description		|	Type	|Reset|
|:---------:|:-------------:|:---------------------:|:---------:|:---:|
|   31-30   |      -        |	Reserved			|	R		|  0  |
|	29-27	|	FSEL9		|	Function Select 9	|	 R/W    |  0  |
|	26-24	|	FSEL8		|	Function Select 8	|	 R/W    |  0  |
|	23-21	|	FSEL7		|	Function Select 7	|	 R/W    |  0  |
|	20-18	|	FSEL6		|	Function Select 6	|	 R/W    |  0  |
|	17-15	|	FSEL5		|	Function Select 5	|	 R/W    |  0  |
|	14-12	|	FSEL4		|	Function Select 4	|	 R/W    |  0  |
|	11-9	|	FSEL3		|	Function Select 3	|	 R/W    |  0  |
|	8-6		|	FSEL2		|	Function Select 2	|	 R/W    |  0  |
|	5-3		|	FSEL1		|	Function Select 1	|	 R/W    |  0  |
|	2-0		|	FSEL0		|	Function Select 0	|	 R/W    |  0  |

FSELn: </br>
 
|:-:|:------:|
|000|输入模式 ||100|复用模式0||110|复用模式2||011|复用模式4|
|001|输出模式 ||101|复用模式1||111|复用模式3||010|复用模式5|
