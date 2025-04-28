# -pg
gcc自带， 最好-static
```
-pg -g -Og -fno-omit-frame-pointer
```

运行完成后可以在当前目录生成gmon.out, 生成报告
```
gprof a.out gmon.out > report.txt
```                