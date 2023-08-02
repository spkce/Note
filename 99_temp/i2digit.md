```c++
char i2digit(int n)
{
	return "0123456789abcdef"[n & 0x0f];
}

```