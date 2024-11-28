# rtmp
RTMP协议是Real Time Message Protocol(实时信息传输协议)的缩写，它是由Adobe公司提出的一种应用层的协议，用来解决多媒体数据传输流的多路复用（Multiplexing）和分包（packetizing）的问题。随着VR技术的发展，视频直播等领域逐渐活跃起来，RTMP作为业内广泛使用的协议也重新被相关开发者重视起来。

RTMP协议是应用层协议，是要靠底层可靠的传输层协议（通常是TCP）来保证信息传输的可靠性的。一般是使用端口1935，并且无论传输何种信息包括音视频数据都只有一个socket通道即只有一个物理通道。

RTMP协议传输时会对数据做自己的格式化，这种格式的消息我们称之为RTMP Message，而实际传输的时候为了更好地实现多路复用、分包和信息的公平性，发送端会把Message划分为带有Message ID的Chunk，每个Chunk可能是一个单独的Message，也可能是Message的一部分，在接受端会根据chunk中包含的data的长度，message id和message的长度把chunk还原成完整的Message，从而实现信息的收发。

## 客户端与服务端整体交互流程
```
                            client                                    server
1. 握手协议                      ｜ <--------------handshake------------> |
2. connect协议                  ｜ ----------------connect-------------> |
                               ｜ --------NetConnection.connect-------> |
3. createSteam协议              ｜ --------------createSteam-----------> |
                               ｜ <--------createSteam response--------- |
4. play协议                     ｜ -----------------play----------------> |
5. 接收服务器信息开始接收数据流     ｜ <-------------SetChunkSzie------------ |
                                ｜ <--------------SteamBegin------------ |
                                ｜ <--------NetSteam.Play.Start--------- |
                                ｜ <--------audio or video data--------- |
6. 播放停止                      ｜ <--------NetSteam.Play.Stop---------- |
```

rtmp播放流程：
RTMP协议规定，播放一个流媒体有两个前提步骤：第一步，建立一个网络连接（NetConnection）；第二步，建立一个网络流（NetStream）。其中，网络连接代表服务器端应用程序和客户端之间基础的连通关系。网络流代表了发送多媒体数据的通道。服务器和客户端之间只能建立一个网络连接，但是基于该连接可以创建很多网络流。


## 握手连接


客户端通过发送 C0 和 C1 消息来启动握手过程。客户端必须接收到 S1 消息，然后发送 C2 消息。客户端必须接收到S2 消息，然后发送其他数据。
服务端必须接收到 C0 或者 C1 消息，然后发送 S0 和 S1 消息。服务端必须接收到 C2消息，然后发送其他数据

```
  client       network          server
    ｜            ｜              ｜
uninitiated       ｜        uninitiated
    ｜-----c0---->｜              ｜
    ｜            ｜------c0----->｜
    ｜-----c1---->｜              ｜
    ｜            ｜<-----s0------｜
version sent      ｜<-----s1------｜
    ｜<-----s0----｜              ｜
    ｜<-----s1----｜        version sent
    ｜            ｜------c1----->｜
    ｜-----c2---->｜              ｜
    ｜            ｜<-----s2------｜
 ack sent         ｜           ack sent
    ｜<-----s2----｜              ｜
    ｜            ｜------c2----->｜
handshake done            handshake done
```

实际实现中为了在保证握⼿的身份验证功能的基础上尽量减少通信的次数，⼀般的发送顺序如下所示:
```
client           server
｜－－－C0+C1－－－> |
｜<－－S0+S1+S2－－ |
｜－－－－C2－－－－> |
```
握手一般分为简单握手和复杂握手
### 简单握手：

```
++++++++++++++++++++++++++++++++++++++++++++++++++++++
|      c0     |              c1                      |
++++++++++++++++++++++++++++++++++++++++++++++++++++++
| version(1B) | time (4B) | zero(4B) | random(1528B) |
++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|      s0     |              s1                      |              s2                      |
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| version(1B) | time (4B) | zero(4B) | random(1528B) |             (1536B)                  |
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++
|              c2                      |
++++++++++++++++++++++++++++++++++++++++
|             (1536B)                  |
++++++++++++++++++++++++++++++++++++++++
S2是C1的复制，C2是S1的复制。
C0 和 S0 包由一个字节组成，version（1 byte）：RTMP 的版本，一般为 3。
```

### 复杂握手：
**c1 和 s1的格式：**
复杂握手将简单握手中1528比特的随机数部分平分为两部分:
1. 764比特的public key(公共密匙); 
2. 764比特的digest(32字节的密文).

复杂握手的Version部分不为0，服务器可以由此判断握手类型
```
C1和S1包含两部分数据：key和digest，key 和 digest 的顺序是不确定的
+++++++++++++++++++++++++++++++++++++   +++++++++++++++++++++++++++++++++++++
|             time (4B)             |   |             time (4B)             |
+++++++++++++++++++++++++++++++++++++   +++++++++++++++++++++++++++++++++++++
|            version (4B)           |   |            version (4B)           |
+++++++++++++++++++++++++++++++++++++   +++++++++++++++++++++++++++++++++++++
|           digest (764B)           |   |             key (764B)            |
+++++++++++++++++++++++++++++++++++++   +++++++++++++++++++++++++++++++++++++
|             key (764B)            |   |           digest (764B)           |
+++++++++++++++++++++++++++++++++++++   +++++++++++++++++++++++++++++++++++++
```
key 结构：
```
+++++++++++++++++++++++++++++++++++++ 
|            (offset B)             | 
+++++++++++++++++++++++++++++++++++++ 
|         key-data (128B)           | 
+++++++++++++++++++++++++++++++++++++ 
|            random-data            | 
+++++++++++++++++++++++++++++++++++++ 
|            offset (4B)            | 
+++++++++++++++++++++++++++++++++++++ 
```
digest 结构：
```
+++++++++++++++++++++++++++++++++++++ 
|            offset (4B)            | 
+++++++++++++++++++++++++++++++++++++ 
|        random-data (offset B)     | 
+++++++++++++++++++++++++++++++++++++ 
|          digest-data (32B)        | 
+++++++++++++++++++++++++++++++++++++ 
|            random-data            | 
+++++++++++++++++++++++++++++++++++++ 
random-data: (764 - 4 - offset - 32) bytes
```

**C2和S2 格式**
```
++++++++++++++++++++++++++++++++++++++++
|      random-data (1504B)             |
++++++++++++++++++++++++++++++++++++++++
|          digest-data (32B)           | 
++++++++++++++++++++++++++++++++++++++++
```

## 消息
RTMP 协议为了维持稳定连续传递，避免单次传输数据量问题，采用了传输层封包，数据流切片的实现形式。被用来对当前带宽进行划分和复用的最小传输单位，被称为 Chunk 即消息块。通常情况下，一个有效的消息，如果数据量超出当前 Chunk Size 的话，则会被拆分成多个分块来分批传输。通过指定首个Chunk 和后续 Chunk 类型，以及 Chunk Header 其他标志性数据，来使当前被切割的消息，能够在对端得到有效的还原和执行。
### 消息块格式
```
|                chunk format                        | 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| Basic Header | Message Header | Extended Timestamp |            chunk data                |
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```
块的基本头（1-3字节）：这个字段包含块流ID和块类型。块类型决定了编码过的消息头的格式。这个字段是一个变长字段，长度取决于块流ID。
消息头（0,3,7,11字节）：这个字段包含被发送的消息信息（无论是全部，还是部分）。字段长度由块头中的块类型来决定。
扩展时间戳（0,4字节）：这个字段是否存在取决于块消息头中编码的时间戳。
块数据（可变大小）：当前块的有效数据，上限为配置的最大块大小。

**Basic Header**
包含 chunk stream ID（流通道id）和chunk type（即fmt），chunk stream id 一般被简写为CSID，用来唯一标识一个特定的流通道，同一个Chunk Stream ID必然属于同一个信道，chunk type决定了后面Message Header的格式
Basic Header的长度可能是 1，2，或 3 个字节，其中 chunk type 的长度是固定的（占2位，单位是bit），Basic Header 的长度取决于 CSID 的大小，在足够存储这两个字段的前提下最好用尽量少的字节从而减少由于引入Header增加的数据量。
RTMP最多可支持65597个流，CSID范围在3-65599 内,CSID 为0, 1, 2 是协议保留的;
CSID=0表示Basic Header块基本头占用 2 个字节，并且CSID范围在64-319 之间(第二个字节+64(2-63使用1字节表示法，2字节表示法就不需要再表示这些))；
CSID=1 表示Basic Header块基本头占用3个字节，并且ID范围在64-65599之间(第三个字节*256 + 第二个字节 + 64(2-63使用1字节表示法，3字节表示法就不需要再表示这些))。
CSID=2 表示该 chunk 是控制信息和一些命令信息，为低版本协议保留的。
CSID=3-63 范围内的值表示整个流ID(有效ID)。

```
1 byte , csid = 2-63
+++++++++++++++++++++++++++
|   fmt  |      csid      |
+++++++++++++++++++++++++++

2 byte , csid = 0
CSID占14bit，此时协议将于chunk type所在字节的其他bit都置为0，
剩下的一个字节表示CSID - 64，这样共有8个bit来存储 CSID，8 bit 可以表示 [0,255] 个数，
因此这种情况下 CSID 在 [64,319]，其中 319 = 255 + 64。
+++++++++++++++++++++++++++++++++++++++++++++
|   fmt  |      0        |     csid -64     |
+++++++++++++++++++++++++++++++++++++++++++++

 3 bytes , csid = 1
 CSID占22bit，此时协议将第一个字节的[2,8]bit置1，余下的16个bit表示CSID - 64，
 这样共有16个bit来存储CSID，16bit可以表示[0,65535]共 65536 个数，因此这种情况下 CSID 在 [64,65599]，其中65599=65535+64.
 需要注意的是，Basic Header是采用小端存储的方式，越往后的字节数量级越高，因此通过3个字节的每一个bit的值来计算CSID时，
 应该是: <第三个字节的值> * 256 + <第二个字节的值> + 64.
 +++++++++++++++++++++++++++++++++++++++++++++
|   fmt  |      1        |     csid -64     |
+++++++++++++++++++++++++++++++++++++++++++++
```

**Message Header**
包含了要发送的实际信息（可能是完整的，也可能是一部分）的描述信息。Message Header的格式和长度取决于Basic Header的chunk type，即fmt，共有四种不同的格式。其中第一种格式可以表示其他三种表示的所有数据，但由于其他三种格式是基于对之前chunk的差量化的表示，因此可以更简洁地表示相同的数据，实际使用的时候还是应该采用尽量少的字节表示相同意义的数据。

1. Chunk Type(fmt) = 0：11 bytes
```
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|                 timestamp(3B)                        |      message length(1B)            |
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| message length(coutinue) (2B) | message type id (1B) |         msg stream id              |
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|                        msg stream id                 |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```
type=0时Message Header占用11个字节，其他三种能表示的数据它都能表示，但在chunk stream 的开始第一个chunk和头信息中的时间戳后退（即值与上一个chunk相比减小，通常在回退播放的时候会出现这种情况）的时候必须采用这种格式。
timestamp（时间戳）：占用3个字节，因此它最多能表示到16777215=0xFFFFFF=2^24-1，当它的值超过这个最大值时，这三个字节都置为1，这样实际的timestamp会转存到 ExtendedTimestamp 字段中，接收端在判断timestamp字段24个位都为1时就会去Extended Timestamp中解析实际的时间戳。
message length（消息数据长度）：占用3个字节，表示实际发送的消息的数据如音频帧、视频帧等数据的长度，单位是字节。注意这里是Message的长度，也就是chunk属于的Message的总长度，而不是chunk本身data的长度。
message type id(消息的类型id)：1个字节，表示实际发送的数据的类型，如8代表音频数据，9代表视频数据。
message stream id(消息的流id)：4个字节，表示该chunk所在的流的ID，和Basic Header的CSID一样，它采用小端存储方式。

2. Chunk Type(fmt) = 1：7 bytes
```
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|                 timestamp delta                      |      message length(1B)            |
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| message length(coutinue) (2B) | message type id (1B) |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```
type为1时占用7个字节，省去了表示message stream id的4个字节，表示此chunk和上一次发的 chunk 所在的流相同，如果在发送端和对端有一个流链接的时候可以尽量采取这种格式。
timestamp delta：3 bytes，这里和type=0时不同，存储的是和上一个chunk的时间差。类似上面提到的timestamp，当它的值超过3个字节所能表示的最大值时，三个字节都置为1，实际的时间戳差值就会转存到Extended Timestamp字段中，接收端在判断timestamp delta字段24个bit都为1时就会去Extended Timestamp 中解析实际的与上次时间戳的差值。

3. Chunk Type(fmt) = 2：3 bytes

```
++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|                 timestamp delta                      |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```
type 为 2 时占用 3 个字节，相对于 type = 1 格式又省去了表示消息长度的3个字节和表示消息类型的1个字节，表示此 chunk和上一次发送的 chunk 所在的流、消息的长度和消息的类型都相同。余下的这三个字节表示 timestamp delta，使用同type=1。

4. Chunk Type(fmt) = 3: 0 byte

type=3时，为0字节，表示这个chunk的Message Header和上一个是完全相同的。当它跟在type=0的chunk后面时，表示和前一个 chunk 的时间戳都是相同。
什么时候连时间戳都是相同呢？就是一个 Message 拆分成多个 chunk，这个 chunk 和上一个 chunk 同属于一个 Message。而当它跟在 type = 1或 type = 2 的chunk后面时的chunk后面时，表示和前一个 chunk的时间戳的差是相同的。
比如第一个 chunk 的 type = 0，timestamp = 100，第二个 chunk 的 type = 2，timestamp delta = 20，表示时间戳为 100 + 20 = 120，第三个 chunk 的 type = 3，表示 timestamp delta = 20,时间戳为 120 + 20 = 140。

### Extended Timestamp
Extended Timestamp(扩展时间戳), 在 chunk 中会有时间戳 timestamp 和时间戳差 timestamp delta，并且它们不会同时存在，只有这两者之一大于3字节能表示的最大数值 0xFFFFFF ＝ 16777215 时，才会用这个字段来表示真正的时间戳，否则这个字段为 0扩展时间戳占 4 个字节，能表示的最大数值就是 0xFFFFFFFF ＝ 4294967295。当扩展时间戳启用时，timestamp字段或者timestamp delta要全置为1，而不是减去时间戳或者时间戳差的值。


### ChunkStreamID与MessageStreamID区别
ChunkStreamID与MessageStreamID是协议分层的概念(各自层次的id即标识或者说各自层次的消息归类)

目前看只有一个socket通道，而且ChunkStreamID只是块流中用于组包消息的，即一条消息拆分chunk后通过ChunkStreamID组装回来，还是标识(区分)逻辑通道的功能，注意不是物理通道。MessageStreamID是对MessageStreamtype消息类型的归类
message stream id就是指该次媒体流的唯一标识，用于指明属于哪个流，用于区分不同流(NetConnection网络连接流或者是(某个)NetStream网络流)的消息。一个StreamId通常用以完成某些特定的工作. 如使用Id为0的Stream来完成客户端和服务器的连接和控制,用Id为1的Stream来完成Stream的控制和播放等工作.

chunk header中Chunk stream ID 是用来区分消息信道的，因为 RTMP 协议，所有的通信都是通过同一个 TCP 来完成的，因此所有类型的通信信道需要由 Chunk stream ID 来进行区分，从而判断当前收到的消息所属的信道类型，此是由用户定义的，Adobe 建议采用如下的分类:

chunk stream 名称 | csid | 作用
---------|----------|---------
 RTMP_CONTROL_CHANNEL | 0x02 | 控制流通道
 RTMP_COMMAND_CHANNEL | 0x03 | 命令信道
 RTMP_STREAM_CHANNEL | 0x05 | 数据流通道
 RTMP_VIDEO_CHANNEL | 0x06 | 视频信道
 RTMP_AUDIO_CHANNEL | 0x07 | 音频信道

其中obs推流、ffmpeg推流都有自己的定义方式，与这个略有不同，CSID再解析式可只作为一路流的通道，由Message Type来标识音视频，消息类型定义如下所示：

### 消息分类
消息主要分为三类: 协议控制消息、数据消息、命令消息等

Message Type|Msg Type id|MessageStreamID|作用
---------|----------|---------|---------
Set Chunk Size|1|0|通知对端，更新最大可接受的Chunk大小，默认为128 (单位: Bytes) ，最小为1 (单位: Bytes)
About Message|2||通知另一方，终止处理指定cs_ id信道后续的其他消息
Acknowledgement|3||由数据接收方(receiver) 发送，当首次收到有效数据大小等于Window Ack Size消息设置的窗口大小时，发送此消息给数据发送方(sender)以表示链接稳定。
User Control Message|4|0|用户控制消息，是-系列用于控制消息流的消息的总称。用来作为控制对端用户操作事件的一种手段
Window Ack Size|5|0|由数据发送方(sender) 发送，用来设定接收方(receiver)首次有效数据传输到来之后，用于等待确定传输稳定的窗口大小(单位: Bytes)
Set Peer Bandwidth|6|0|由数据接收方(receiver) 发送，根据已收到但未确认的消息的数据量，来通知约束发送方(sender) 的输出带宽(单位: Bytes)，收到消息需要发送Window AckSize做应答
Audio|8|1|RTMP音频数据包
Video|9|1|RTMP视频数据包
Data AMF3|15||AMF3编码，音视频MetaData风格|配置 详情包
Shared Object AMF3|16||AMF3编码，共享对象消息(携带用户详情)
Command AMF3|17||AMF3编码，RTMP 命令消息，可能涉及用户数据
Data AMFO|18||AMFO编码，音视频MetaData风格| 配置详情包
Shared Object AMFO|19||AMFO编码，共享对象消息(携带用户详情)
Command AMFO|20(0x14)|Connect 0，play 1|AMFO编码，RTMP 命令消息，可能涉及用户数据
Aggregate Message|22||整合消息


**1. 协议控制消息**
Message Type ID = 1 2 3 5 6和Message Type ID = 4两大类，主要用于协议内的控制
**2. 数据消息**
Message Type ID = 8 9 18
8: Audio 音频数据
9: Video 视频数据
18: Metadata 包括音视频编码、视频宽高等信息。
**3. 命令消息**
Message Type ID = 20 17。此类型消息主要有NetConnection和NetStream两个类
 下面的类对象被用来发送各种命令(其实还是分层的概念，分为两层)：

NetConnection：服务器和客户端之间进行网络连接的一种高级表示形式。网络连接允许使用以下的命令：连接 connect,调用 call,停止 close,创建流 createStream
NetStream：代表了发送音频流，视频流，或其他相关数据的频道。当然还有一些像播放，暂停之类的命令，用来控制数据流

### connect 内容
Field Name	| Type	| Description
---------|----------|---------
Command Name	|String	|Name of the command. Set to "connect".
Transaction ID	|Number	|Always set to 1.
Command Object	|Object	|Command information object which has the name-value pairs.
Optional User Arguments	|Object	|Any optional information

Command Object里包含
Property|	Type	|Description	|Example Value
---------|----------|---------|---------
app	String	|The Server application name the client is connected to.|	testapp
flashver	|String	Flash Player version. It is the same string as returned by the ApplicationScript getversion () function.|	FMSc/1.0
swfUrl	|String	URL of the source SWF file making the connection.	|file://C:/FlvPlayer.swf
tcUrl	|String	URL of the Server. It has the following format. protocol://servername:port/appName/appInstance	|rtmp://localhost:1935/testapp/instance1
fpad	|Boolean	True if proxy is being used.	|true or false
audioCodecs	|Number	Indicates what audio codecs the client supports.	|SUPPORT_SND_MP3
videoCodecs|	Number	Indicates what video codecs are supported.	|SUPPORT_VID_SORENSON
videoFunction	|Number	Indicates what special video functions are supported.	|SUPPORT_VID_CLIENT_SEEK
pageUrl	|String	URL of the web page from where the SWF file was loaded.	|http://somehost/sample.html
object |Encoding	Number	AMF encoding method.	|AMF3

音频编码属性的可选值：
原始 PCM，ADPCM，MP3，NellyMoser（5，8,11，16，22，44kHz），AAC，Speex。

Codec |Flag	Usage	|Value
---------|----------|---------
SUPPORT_SND_NONE	|Raw sound, no compression	|0x0001
SUPPORT_SND_ADPCM	|ADPCM compression	|0x0002
SUPPORT_SND_MP3	mp3 |compression	|0x0004
SUPPORT_SND_INTEL	|Not used	|0x0008
SUPPORT_SND_UNUSED	|Not used	|0x0010
SUPPORT_SND_NELLY8	|NellyMoser at 8-kHz compression	|0x0020
SUPPORT_SND_NELLY	|NellyMoser compression (5, 11, 22, and 44 kHz)	|0x0040
SUPPORT_SND_G711A	|G711A sound compression (Flash Media Server only)	|0x0080
SUPPORT_SND_G711U	|G711U sound compression (Flash Media Server only)	|0x0100
SUPPORT_SND_NELLY16	|NellyMouser at 16-kHz compression	|0x0200
SUPPORT_SND_AAC	|Advanced audio coding (AAC) codec	|0x0400
SUPPORT_SND_SPEEX	|Speex Audio	|0x0800
SUPPORT_SND_ALL	All| RTMP-supported audio codecs	|0x0FFF


### call内容
网络连接对象中包含的 call 方法，会在接收端执行远程过程调用（RPC）。被调用的 RPC 方法名作为 call 方法的参数传输。

从发送端到接收端的命令结构如下：


Field Name	|Type	|Description
---------|----------|---------
Procedure Name	|String	|Name of the remote procedure that is called.
Transaction ID	|Number	|If a response is expected we give a transaction Id. Else we pass a value of 0
Command Object	|Object	|If there exists any command info this is set, else this is set to null type.
Optional Arguments	|Object	|Any optional arguments to be provided

应答的命令结构如下：


Field Name	|Type	|Description
---------|----------|---------
Command Name	|String|	Name of the command.
Transaction ID	|Number	|ID of the command, to which the response belongs.
Command Object	|Object|	If there exists any command info this is set, else this is set to null type.
Response	|Object	|Response from the method that was called.


### createStream
客户端通过发送此消息给服务器来创建一个用于消息交互的逻辑通道。音频，视频，和元数据都是通过 createStream 命令创建 的流通道发布出去的。
NetConnection 是默认的交互通道，流 ID 为 0. 协议和一部分命令消息，包含 createStream，都是使用默认的交互通道发布 的。

从客户端发送给服务器的命令结构如下：


Field Name	|Type	|Description
---------|----------|---------
Command Name|	String	|Name of the command. Set to "createStream".
Transaction ID|	Number	|Transaction ID of the command.
Command Object	|Object	|If there exists any command info this is set, else this is set to null type.

从服务器发送给客户端的命令结构：

Field Name	|Type	|Description
---------|----------|---------
Command Name	|String	|_result or _error; indicates whether the response is result or error.
Transaction ID	|Number	|ID of the command that response belongs to.
Command Object	|Object	|If there exists any command info this is set, else this is set to null type.
Stream ID	|Number	|The return value is either a stream ID or an error information object.

### NetStream

网络流定义了通过网络连接把音频，视频和数据消息流在客户端和服务器之间进行交换的通道。一个网络连接对象可以有多个 网络流，进而支持多个数据流。

客户端可以通过网络流发送到服务器的命令如下：

播放 play
播放 2 play2
删除流 deleteStream
关闭流 closeStream
接收音频 receiveAudio
接收视频 receiveVideo
发布 publish
定位 seek
暂停 pause

服务器通过发送 onStatus 命令给客户端来通知网络流状态的更新。

```
+--------------+----------+----------------------------------------+
| Field Name   |   Type   |             Description                |
+--------------+----------+----------------------------------------+
| Command Name |  String  | The command name "onStatus".           |
+--------------+----------+----------------------------------------+
| Transaction  |  Number  | Transaction ID set to 0.               |
| ID           |          |                                        |
+--------------+----------+----------------------------------------+
| Command      |  Null    | There is no command object for         |
| Object       |          | onStatus messages.                     |
+--------------+----------+----------------------------------------+
| Info Object  | Object   | An AMF object having at least the      |
|              |          | following three properties: "level"    |
|              |          | (String): the level for this message,  |
|              |          | one of "warning", "status", or "error";|
|              |          | "code" (String): the message code, for |
|              |          | example "NetStream.Play.Start"; and    |
|              |          | "description" (String): a human-       |
|              |          | readable description of the message.   |
|              |          | The Info object MAY contain other      |
|              |          | properties as appropriate to the code. |
+--------------+----------+----------------------------------------+
```