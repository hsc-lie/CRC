### 一、简介
- CRC校验工具
- 最高支持到32位
- 可设置LSB或MSB（一些网上的算法可能是设置Refin(输入数据反转)或Refout(输出校验值反转)。LSB与相当于Refin=true,Refin=true,MSB相当于Refin=false,Refin=false)
- 可设置不同的多项式
- 可设置初始值和输出异或值

### 二、移植说明
- 1.只需要将crc文件夹中的文件移动到自己的工程中即可
- 2.crc/crc_table.c和crc/crc_table.h这两个文件为存放CRC表的数组可根据需要删减,数组的生成请看[4.5](#45-生成crc表的c代码数组)小节
- 3.如下示例为计算字符串123456的CRC校验和，CRC参数为32位LSB，多项式为0x4C11DB7(x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+1)，初始值为0xFFFFFFFF，异或值为0xFFFFFFFF，计算结果最后存在checksum变量中


    CRC_t crc;
    uint32_t checksum;
    CRCInit(&crc, 32, 0x4C11DB7, TRUE);
    CRCStart(&crc, 0xFFFFFFFF);
    CRCUpdate(&crc, "123456", 6);
    checksum = CRCGetChecksum(&crc, 0xFFFFFFFF);

### 三、编译当前工程的示例
输出的可执行文件和静态库均在./build/out文件夹内
#### 3.1 cmake构建
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

#### 3.2 使用make构建

    make


### 四、工具使用说明
#### 4.1 工具命令预览

    -l, --lsb               设置为LSB
    -m, --msb               设置为MSB
    -w, --wide              设置CRC位宽(例如设置为32位: -w 32)
    -p, --poly              设置多项式的值(例: -p 4C11DB7)
    --init                  设置CRC校验初始值(例: --init FFFFFFFF)
    --xor                   设置CRC异或值(例: --xor FFFFFFFF)
    -h, --hex               设置对16进制的数据校验(例如计算0x12345678的Checksum: -h 12345678)
    -s, --string            设置对字符串数据校验(例如计算"123456"的Checksum: -s 123456)
    -f, --file              设置对文件进行校验(例如计算fliename文件的校验: -f filename)
    --out-bin               设置将校验和以2进制形式写入文件中(例如将校验和输出到out.bin中: --out-bin out.bin)
    --out-txt               设置将校验和以文本形式写入到文件中(例如将校验和输出到out.txt中: --out-bin out.txt)
    -g, --generate-table    生成用于CRC计算的C代码数组(例如将代码输出到table.c中: -g table.c)
    -v, --version           打印版本信息
    --help                  打印帮助文档

#### 4.2 CRC参数配置及对输入值进行CRC校验
如下以计算字符串的CRC校验和示例如何配置参数,下面的命令配置依次为设置LSB、设置为32位、设置多项式为0x4C11DB7、初始值为0xFFFFFFFF、异或值为0xFFFFFFFF、计算字符串"123456"

    crc -l -w 32 -p 04C11DB7 --init FFFFFFFF --xor FFFFFFFF -s 123456

在未配置参数时，上述的配置就是默认的配置，如下命令计算结果同上

    crc -s 123456

除字符串外，还可以对16进制值进行校验，如下命令则为计算0x12345678的CRC校验和

    crc -h 12345678

#### 4.3 对文件的校验示例
如下命令可以计算filename.txt文件的CRC校验和

    crc -f filename.txt

#### 4.4 将校验的结果写入到文件
如下命令可以将对"123456"字符串的CRC校验和以2进制形式写入到out.bin文件中

    crc -s 123456 --out-bin out.bin

也可以文本的形式写入到out.txt文件中

    crc -s 123456 --out-txt out.txt

#### 4.5 生成CRC表的C代码数组
为加快计算效率，一般会提前计算好一个数组，用于CRC校验，如下示例为生成一份CRC校验的表到table.c文件中，CRC参数配置的说明请看[4.2](#42-crc参数配置及对输入值进行crc校验)

    crc -l -w 32 -p 04C11DB7 --init FFFFFFFF --xor FFFFFFFF -g table.c

同4.2上述参数也为默认参数，使用如下命令也可生成一样的结果

    crc -g table.c

