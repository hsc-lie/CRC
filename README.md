### CRC校验
- 最高支持到32位
- 可设置LSB或MSB（一些网上的算法可能是设置Refin(输入数据反转)或Refout(输出校验值反转)。LSB与相当于Refin=true,Refin=true,MSB相当于Refin=false,Refin=false)
- 可设置不同的多项式
- 可设置初始值和输出异或值

### 使用说明
- 1.只需要将crc文件夹中的文件移动到自己的工程中即可
- 2.可参考main.c中的Test()函数中的例程来使用
- 3.crc/crc_table.c和crc/crc_table.h这两个文件为存放CRC表的数组可根据需要删减,数组的生成在main.c中的GenereteCRCTableFile()函数中生成

### 编译当前工程的示例
#### cmake构建
Linux环境下

    mkdir build
    cd build
    cmake ..
    cmake --build .

Windows环境下，以MinGW64为例

    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ..
    cmake --build .

#### make构建

    make

输出的可执行文件和静态库均在./build/out文件夹内
