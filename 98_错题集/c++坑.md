### 1. cin

cin会被空格截断，若需要空格可以
```c++
char str[100];
cin.getline(str, sizeof(str));
```