### set
定义一个变量
```cmake
set(KERNEL_MAKE_SAVE_DEFCONFIG cp ${BUILD_DIR}/.config ${SRC_DIR}/arch/arm64/configs/${KERNEL_CFG} -v)
```
### add_custom_command
```cmake
dd_custom_command(  
    OUTPUT <output1> [<output2>...]  # 必选：命令生成的目标文件  
    COMMAND <command1> [<args1>...]   # 必选：执行的命令（可多条）  
    [MAIN_DEPENDENCY <file>]           # 主依赖文件（变化时强制重新执行）  
    [DEPENDS <dep1> <dep2>...]        # 附加依赖文件/目标（变化时触发重新执行）  
    [IMPLICIT_DEPENDS <lang> <file>]   # 隐式依赖（如语法分析生成的依赖）
    [WORKING_DIRECTORY <dir>]          # 命令执行的工作目录（默认当前源目录）
    [COMMENT "<message>"]              # 执行时显示的提示信息  
    [VERBATIM]                         # 保留命令原始格式（避免CMake转义）
    [USES_TERMINAL]                    # 在终端中执行命令（Windows适用）
)
```



# 交叉编译参数
创建arm-toolchain.cmake文件，定义交叉编译参数 arm-toolchain.cmake
```
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm64)

# 设置交叉编译器路径
set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)

# 指定Qt路径（需提前编译ARM版Qt）
set(Qt5_DIR /opt/Qt-arm/lib/cmake/Qt5)  # ARM架构的Qt CMake配置路径:ml-citation{ref="4,7" data="citationList"}
set(QT_QMAKE_EXECUTABLE /opt/Qt-arm/bin/qmake)  # ARM版qmake路径:ml-citation{ref="7" data="citationList"}

```
编译时
```
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..  # 应用交叉编译配置:ml-citation{ref="7" data="citationList"}
```

# sysroot
set(CMAKE_SYSROOT /xxx)
或者
cmake -DCMAKE_SYSROOT=/path/to/sysroot ..


# -S -B

cmake -S VTK-7.1.1 -B build