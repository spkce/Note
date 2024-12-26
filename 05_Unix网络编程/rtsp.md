# RTSP
RTSP（Real Time Streaming Protocol 实时流协议）建立并控制一个或几个时间同步的连续流媒体，对媒体流提供了诸如开始、暂停、快进、停止等控制，RTSP的作用相当于流媒体服务器的远程控制，而它本身并不传输数据。

它的语法和运作跟HTTP 1.1类似，但并不特别强调时间同步，所以比较能容忍网络延迟。

RTSP具有重新导向功能，可视实际负载情况来转换提供服务的服务器，以避免过大的负载集中于同一服务器而造成延迟。

### RTSP协议与HTTP协议区别
RTSP协议与HTTP协议的语法非常类似，且都是纯文本协议，但它们也有区别：
1. RTSP是有状态的，它命令总是按照顺序来发送，其中某个命令可能需要总在另外一个命令之前要发送。而http则是无状态，协议在发送一个命令以后，连接就会断开，且命令之间是没有依赖性的。
2. rtsp协议使用554端口，http使用80端口。
3. RTSP的请求服务器和客户端都可以发送，而HTTP请求则只能由客户端发送

## RTSP方法
RTSP常用的方法包括：OPTIONS、DESCRIBE、ANNOUNCE、SETUP、TEARDOWN、PLAY、PAUSE、GET_PARAMETER和SET_PARAMETER等


方法 |报文方向|是否必须| 含义
---------|----------|---------|---------
OPTIONS	| C->S |否 |获在任意时刻由客户端主动发出，取服务器支持的方法。它不影响服务器的状态
DESCRIBE | C->S |否| 向服务器获取URL指定的媒体对象的描述，其中Accept字段指定了描述格式
ANNOUNCE| C<->S |否|当客户端向服务器发送时，表示的是将通过请求URL识别的表示描述或者媒体对象提交给服务器.当服务器向客户端发送时，表示的是通知客户端更新会话信息
SETUP|C->S|是|客户端向服务器请求建立会话并准备传输。请求信息主要包括传输协议和客户端的端口号
PLAY|C->S|是|客户端主动通知服务器以SETUP指定的机制开始发送数据。其中Range字段指定了播放的起止时间，当多个PLAY请求到达时，服务器会将PLAY请求排成队列，顺序执行，即必须等待第一个PLAY的时间完成后，才会继续处理第二个PLAY消息。
PAUSE|C->S|否|客户端请求服务器的媒体流传输临时暂停。可以通过Range参数在指定时间点暂停，也可以指定某股流暂停，例如如果指定音频流暂停，则播放将是无音状态
RECORD|C->S|否|RECORD通知服务器方法客户端将会根据之前的描述开始记录媒体数据。 其中timestamp 字段反映开始和结束时间 (UTC)。如果该字段不存在，则会使用媒体描述中的开始或结束时间。 如果会话已经开始，则立即开始录制。服务器决定是将记录的数据存储在 request-URI 下还是另一个 URI 下。 如果服务器不使用 request-URI，则响应应该是 201（已创建）并包含描述请求状态并引用新资源的实体和 Location 标头。
REDIRECT|S->C|否|重定向请求通知客户端它必须连接到另一个服务器位置。 它包含强制标头 Location，它指示客户端应该发出对该 URL 的请求。 它可能包含参数Range，表示重定向何时生效。 如果客户端想要继续发送或接收此 URI 的媒体，客户端必须为当前会话发出 TEARDOWN 请求，并在指定主机上为新会话发出 SETUP。
GET_PARAMETER|C<->S|否|GET_PARAMETER 请求检索 URI 中指定的表示或流的参数值。 回复和响应的内容留给实现。 没有实体主体的 GET_PARAMETER 可用于测试客户端或服务器的活跃度（“ping”）。
SET_PARAMETER|C<->S|否|这个方法请求设置演示或URL指定流的参数值。请求仅应包含单个参数，允许客户端决定某个特殊请求为何失败。如请求包含多个参数，所有参数可成功设置，服务器必须只对该请求起作用。服务器必须允许参数可重复设置成同一值，但不让改变参数值。注意：媒体流传输参数必须用SETUP命令设置。将设置传输参数限制为SETUP有利于防火墙。
TEARDOWN|C->S|是|TEARDOWN请求停止给定URL流发送，释放相关资源。

# RTSP报文
RTSP有两类报文：请求报文和响应报文。
RTSP报文由三部分组成，即开始行、首部行和实体主体
## 请求报文
```
方法｜空格｜URL｜空格｜版本｜CRLF（回车换行）            <- 开始行
首部字段名:｜空格｜值｜CRLF（回车换行）                |
......                                           | <- 首部行
首部字段名:｜空格｜值｜CRLF（回车换行）                |
CRLF（回车换行）
实体主体                                            <-实体主体
```

## 响应报文
```
版本｜空格｜状态码｜空格｜短语｜CRLF（回车换行）          <- 状态行
首部字段名:｜空格｜值｜CRLF（回车换行）                |
......                                           | <- 首部行
首部字段名:｜空格｜值｜CRLF（回车换行）                |
CRLF（回车换行）
实体主体                                            <-实体主体
```

### RTSP常用字段含义
**Accept**: 用于指定客户端通知服务器自己可以接受的实体数据结构类型。例如: Accept: application/sdp，之后服务器通过Content-Type字段返回其实体数据结构类型
**Accept-Encoding**:用于客户端通知服务器自己可以接受的数据压缩格式，例如:Accept-Encoding: gzip, compress, br，之后服务器将通过Content-Encoding字段通知客户端它的选择。
**Accept-Language**: 用于客户端通知服务器自己可以理解的语言及其接受度，例如:Accept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5  ，之后服务器将通过Content-Language字段通知客户端它的选择
**Authorization**:客户端请求消息头含有服务器用于验证用户代理身份的凭证
**Bandwidth**: 用于描述客户端可用的带宽值。例如: Bandwidth: 4000
**Blocksize**：此字段由客户端发送到媒体服务器，要求服务器提供特定的媒体包大小，服务器可以自由使用小于请求的块大小。 此数据包大小不包括 IP、UDP 或 RTP 等低层标头
**CSeq**： 指定了RTSP请求响应对的序列号，对每个包含一个给定序列号的请求消息，都会有一个相同序列号的回应消息，且每个请求或回应中都必须包括这个头字段。
**Cache-Control**：通过指定指令来实现缓存机制。缓存指令是单向的，这意味着在请求中设置的指令，不一定被包含在响应中
**Conference**：通知服务器不得更改同一 RTSP 会话的会议 ID
**Connection**：该字段决定当前的事务完成后，是否会关闭网络连接。如果该值是“keep-alive”，网络连接就是持久的，不会关闭，使得对同一个服务器的请求可以继续在该连接上完成或者Connection: close。
**Content-Length**：该字段指明在RTSP协议最后一个标头之后的双 CRLF 之后的内容长度。例如在服务器响应DESCRIBE中，指明sdp信息长度
**Content-Type**:告诉客户端实际返回的内容的内容类型
**User-Agent**: 该字段用来让网络协议的对端来识别发起请求的用户代理软件的应用类型、操作系统、软件开发商以及版本号。
**Expires**：指明过期的时间
**Rang**： 用于指定一个时间范围，可以使用SMPTE、NTP或clock时间单元。
**Session**: Session头字段标识了一个RTSP会话。Session ID 是由服务器在SETUP的回应中选择的，客户端一当得到Session ID后，在以后的对Session 的操作请求消息中都要包含Session ID.例如：Session: 411B4161;timeout=65
**Transport**: Transport头字段包含客户端可以接受的传输选项列表，包括传输协议，地址端口，TTL等。服务器端也通过这个头字段返回实际选择的具体选项。如: Transport: RTP/AVP/TCP;unicast;destination=192.168.31.222;source=192.168.31.222;interleaved=0-1

## RTSP错误码
应码的首位数字区别，可以分为以下五个类别：
1xx: 提示- 请求已经收到，正在处理中
2xx: 成功- 请求已经被成功处理
3xx: 重定向- 必须采取进一步行动才能完成请求
4xx: 客户端错误 - 请求中包含错误的参数或语法导致请求无法被满足
5xx: 服务器错误 - 服务器无法满足客户端正确的请求

错误码	|原因短语	|方法
---------|----------|---------|---------
100|	Continue	| All
200|	Success	| All 
201|	Created 	|RECORD
250|	Low on Storage Space	|RECORD
300|	Multiple Choices 	|All
301|	Moved Permanently	|All
302|	Moved Temporarily 	|All
303|	See Other 	|All
305|	Use Proxy	|All
400|	Bad Request	|All
401|	Unauthorized	|All
402|	Payment Required	|All
403|	Forbidden	|All
404|	Not Found 	|All
405|	Method Not Allowed 	|All
406|	Not Acceptable  	|All
407|	Proxy Authentication Required	|All
408|	Request Timeout	|All
410|	Gone	|All
411|	Length Required	|All
412|	Precondition Failed	DESCRIBE, |SETUP
413|	Request Entity Too Larg	|All
414|	Request-URI Too Long	|All
415|	Unsupported Media Type	|All
451|	Invalid parameter	|SETUP
452|	Illegal Conference Identifier	|SETUP
453|	Not Enough Bandwidth	|SETUP
454|	Session Not Found 	|All
455|	Method Not Valid In This State	|All
456|	Header Field Not Valid 	|All
457|	Invalid Range	|PLAY
458|	Parameter Is Read-Only	|SET_PARAMETER
459|	Aggregate Operation Not Allowed	|All
460|	Only Aggregate Operation Allowed	|All
461|	Unsupported Transport  	|All
462|	Destination Unreachable 	|All
500|	Internal Server Error	|All
501|	Not Implemented 	|All
502|	Bad Gateway    	|All
503|	Service Unavailable 	|All
504|	Gateway Timeout 	|All
505|	 RTSP Version Not Supported	|All
551|	Option not support   	|All

## RTSP中的多播与单播模式

在RTSP中，数据传输可以通过多播或单播两种模式进行。

单播模式（Unicast）：

在单播模式下，服务器为每个客户端发送单独的数据流，传输的媒体数据只能被特定的客户端接收。单播适合点播视频或视频监控等场景，每个客户端都可以独立控制播放的进度和流媒体内容。
优点：提供个性化的播放体验，每个客户端都可以根据自己的需要控制播放（如暂停、快进）。
缺点：如果同时有大量客户端请求流媒体，服务器压力较大，因为它需要为每个客户端分别发送流数据，带宽消耗较高。
多播模式（Multicast）：

在多播模式下，服务器向一组特定的客户端发送同一个流媒体数据包，适合同时向多个用户广播相同的内容，例如直播场景。多播通过节省带宽来提高传输效率，因为服务器只需要发送一次数据，而多个客户端可以同时接收。
优点：节省带宽，适合大规模广播，如IPTV、直播等应用场景。
缺点：客户端无法独立控制播放进度，所有接收多播流的客户端只能同步观看，无法实现个性化的控制。
在RTSP会话建立时，客户端可以通过SETUP请求中的Transport字段来指定传输方式，选择使用单播或多播模式。根据网络环境和应用需求，服务器会决定哪种传输模式最合适。

# RTP
全名是Real-time Transport Protocol（实时传输协议），对应文档RFC3550。也包含配套协议RTCP（Real-time Transport Control Protocol，即实时传输控制协议），提供服务质量。RTP基于UDP，分组传输，接收端数据包往往有延迟和乱序。在发送端，存在丢包；为降低延迟，往往对传输数据进行预处理（降低质量和高效压缩）。在接收端为了恢复时序，采用了接收缓冲；而为了实现媒体的流畅播放，则采用了播放缓冲。使用接收缓冲，可以将接收到的数据包缓存起来，然后根据数据包的封装信息（如包序号和时戳等），将乱序的包重新排序，最后将重新排序了的数据包放入播放缓冲播放。

视频会议同时使用了音频和视频会议，将分别在不同的RTP会话中传送，每一个会话使用不同的传输地址（IP地址＋端口），并且每个会话对应的RTCP包都使用规范化名字CNAME（Canonical Name）。与会者可以根据RTCP包中的CNAME来获取相关联的音频和视频，然后根据RTCP包中的计时信息(Network time protocol)来实现音频和视频的同步。

### RTP与RTSP的关系

RTSP本身并不用于实际传输媒体数据，而是负责控制媒体流的播放、暂停、停止等操作，媒体的传输通常由RTP（Real-time Transport Protocol，实时传输协议）来完成。

## RTP 报文格式
```
0                 8     16     31
｜version｜P｜X｜CC｜M｜PT｜序列号｜
｜              时间戳          ｜
｜            同步源标识符       ｜
｜         分信源标识符（可选）   ｜
```
首先是版本号（V）字段，占2位，用于标识RTP的版本，目前常用版本为2。
填充位（P）占1位，如果设置为1，表示该RTP包末尾包含填充字节。
扩展位（X）占1位，当设置为1时，表示RTP头部后面跟有一个扩展头部。
参与源计数（CC）字段占4位，它指示了CSRC（Contributing Source Identifiers）列表中的源数目。
标记位（M）占1位，它的含义根据具体的应用场景而定，例如在音频流中可能用于标记音频帧的边界。
载荷类型（PT）字段占7位，用于标识RTP载荷的类型，不同的载荷类型对应不同的音视频编码格式，如载荷类型96 127是动态分配的，可用于新出现的编码格式。
序列号占16位，用于对每个RTP包进行编号，接收端可以通过序列号来检测包的丢失和恢复包的顺序。
时间戳占32位，它反映了RTP数据包中载荷数据的采样时刻，对于音频和视频等连续媒体的同步非常重要。
同步源标识符（SSRC）占32位，用于标识RTP流的发送源，在同一个RTP会话中，每个发送源都有一个唯一的SSRC。

# RTCP
服务质量的监视与反馈、媒体间的同步，以及多播组中成员的标识。在RTP会话期 间，各参与者周期性地传送RTCP包。RTCP包中含有已发送的数据包的数量、丢失的数据包的数量等统计资料，因此，各参与者可以利用这些信息动态地改变传输速率，甚至改变有效载荷类型。RTP和RTCP配合使用，它们能以有效的反馈和最小的开销使传输效率最佳化，因而特别适合传送网上的实时数据。
