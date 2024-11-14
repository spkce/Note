# MQTT简介
MQTT(Message Queuing Telemetry Transport, 消息队列遥测传输协议)，是一种基于发布/订阅(publish/subscribe)模式的"轻量级"通讯协议，该协议构建于TCP/IP协议上，由IBM在1999年发布。MQTT最大优点在于，可以以极少的代码和有限的带宽，为远程连接设备提过实时可靠的消息服务，作为一种低开销、低带宽占用的即时通讯协议，使其在物联网、小型设备、移动应用等方面有较广泛的应用


# MQTT协议原理
实现MQTT协议需要客户端和服务器端通讯完成， 在通讯过程中, MQTT协议中有三种身份: 发布者(Publish)、代理(Broker)(服务器)、订阅者(Subscribe)。 其中，消息的发布者和订阅者都是客户端，消息代理是服务器，消息发布者可以同时是订阅者。


MQTT传输的消息分为: 主题(Topic) 和 负载(payload)两部分:
* Topic: 可以理解为消息的类型，订阅者订阅(Subscribe)后，就会收到该主题的消息内容(payload)\
* payload: 可以理解为消息的内容，是指订阅者具体要使用的内容

主题(Topic)以’/‘为分隔符区分不同的层级，包含通配符’+’ 或 ‘#’的主题又称为主题过滤器(Topic Filters); 不含通配符的成为主题名(Topic Names) 例如:
```
sensor/10/temperature
sensor/+/temperature
$SYS/broker/metrics/packets/received
$SYS/broker/metrics/#

'+' : 表示通配一个层级， 例如a/+，匹配a/x, a/y
```
发布者(Publisher)只能向主题名发布消息，订阅者(Subscriber)则可以通过订阅主题过滤器来通配多个主题名称
会话(Session)
每个客户端与服务器建立连接后就是一个会话，客户端和服务器之间有状态交互。会话存在于一个网络之间，也可能在客户端和服务器之间跨越多个连续的网络连接。

# MQTT协议中的方法：
```
CONNECT: 客户端连接到服务器
CONNACK: 连接确认
PUBLISH: 发布消息
PUBACK: 发布消息确认
PUBREC: 发布的消息已接收
PUBREL: 发布的消息已释放
PUBCOMP: 发布完成
SUBSCRIBE: 订阅请求
SUBACK: 订阅确认
UNSUBSCRIBE: 取消订阅
UNSUBACK: 取消订阅确认
PINGREQ: 客户端发送心跳
PINGRESP: 服务端心跳响应
DISCONNECT: 断开连接
AUTH: 认证
```
# MQTT协议数据包结构
在MQTT协议中，一个MQTT数据包由: 固定头(Fixed header)、可变头(Variable header)、消息体(payload)三部分构成。


固定头 | 可变头 | 消息体
---------|----------|---------
 Fixed header | Variable header | payload


### 固定头报文格式
  |  | 7-4bit | 3-0bit
---------|----------|---------
 Byte1 | MQTT数据包类型 | 不同MQTT数据包具体标识

   |  | 7-0bit
---------|----------|---------
 Byte2 | 剩余长度

其中Bit[3]为DUP字段，如果该值为1，表明这个数据包是一条重复的消息；否则该数据包就是第一次发布的消息
Bit[2-1]为QoS字段:
如果Bit 1 和 Bit 2都为0，表示QoS 0: 至多一次；
如果Bit 1为1, 表示QoS 1: 至少一次；
如果Bit 2为1，表示QoS 2：只有一次；
如果同时将Bit 1 和 Bit 2都设置成1，那么客户端或服务器认为这是一条非法的消息，会关闭当前连接。
MQTT消息QoS
MQTT发布消息服务质量保证(QoS)不是端到端的，是客户端与服务端之间的。订阅者收到MQTT消息的QoS级别，最终取决于发布消息的QoS和主题订阅的QoS
QoS: 服务质量是指 客户端和服务端之间的服务质量

### 固定头报文类型

 报文类型 | 方向| 描述 | 7-4bit值 | 
---------|----------|---------|---------
CONNECT | C->S | 客户端连接到服务器 | 0001
CONNACK | S->C | 连接确认 | 0010
PUBLISH | C<->S | 发布消息 | 0011
PUBACK | C<->S | 发布消息确认(Qos1) | 0100
PUBREC | C<->S | 消息已接收(Qos2第一阶段) | 0101
PUBREL | C<->S | 消息已释放(QoS2第二阶段) | 0110
PUBCOMP | C<->S | 发布完成(QoS2第二阶段) | 0111
SUBSCRIBE | C->S | 订阅请求 | 1000
SUBACK | S->C | 订阅确认 | 1001
UNSUBSCRIBE | C->S | 取消订阅 | 1010
UNSUBACK | S->C | 取消订阅确认 | 1011
PINGREQ | C->S | 客户端发送心跳 | 1100
PINGRESP | S->C | 服务端心跳响应 | 1101
DISCONNECT | C->S | 断开连接 | 1110

## 可变头(Variable Header)

### CONNECT 可变头
 2Byte | 1Byte | 1Byte | 2Byte
---------|----------|---------|---------
 协议名“MQTT” | 协议级别 | 连接标志 | 心跳时长

负载
 2Byte | 1Byte | 1Byte | 2Byte
---------|----------|---------|---------
 客户端标识符（必须第一个字段） | 主题 | 消息 | 用户名 | 密码



### CONNACK 可变头
 1Byte | 
---------|
 连接返回码 |

 ### PUBLISH 可变头
 3～32767 | 2Byte | 
---------|----------
 主题名 | 消息ID | 
 负载

   | 
---------|
 应用消息 |


# MQTT官方文档
 http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html