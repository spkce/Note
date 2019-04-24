
##Network Time Protocol（NTP）

协议是用来使计算机时间同步化 的一种协议，它可以使计算机对其服务器或时钟源（如石英钟，GPS等）做同步化，它可以提供高精确度的时间校正（LAN上与标准时间差小于1毫秒，WAN上几十毫秒），且可用加密确认的 方式来防止协议攻击。

 NTP提供准确时间，首先要有准确的时间来源，这一时间应该是国际标准时间UTC。 NTP获得UTC的时间来源可以是原子钟、天文台、卫星，也可以从Internet上获取。这样就有了准确而可靠的时间源。时间是按NTP服务器的等级传 播。按照距离外部UTC 源的远近将所有服务器归入不同的Stratun（层）中。Stratum-1在顶层，有外部UTC接入，而Stratum-2则从Stratum-1获取 时间，Stratum-3从Stratum-2获取时间，以此类推，但Stratum层的总数限制在15以内。所有这些服务器在逻辑上形成阶梯式的架构并 相互连接，而Stratum-1的时间服务器是整个系统的基础。

##NTP数据格式

进行网络协议实现时最重要的是了解协议数据格式。NTP数据包有48个字节，其中NTP包头16字节，时间戳32个字节。


<span><img height="277" src="http://www.eefocus.com/data/09-09/111_1254021565/1270450736.jpg" width="554" alt=""></span><span><img height="277" src="http://www.eefocus.com/data/09-09/111_1254021565/1270450736.jpg" width="554" alt=""></span>
