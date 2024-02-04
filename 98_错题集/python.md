不可变参数问题

以下两个赋值语句不同
```python
csum = 0
csum += (104 << 8) 
csum += 101 & 0x00FF
```
```python
csum = (104 << 8) + 101 & 0x00FF
```
