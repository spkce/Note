# mp4 文件格式简介
mp4文件格式又被称为MPEG-4 Part 14，出自MPEG-4标准第14部分 。它是一种多媒体格式容器，广泛用于包装视频和音频数据流、海报、字幕等。(顺便一提，目前流行的视频编码格式AVC/H264定义在MPEG-4 Part 10)。

# mp4 文件三种组织方式
(1) 普通MP4，General MP4，box模式为ftyp-mdat-moov。
(2) 快速MP4，Faststart MP4，box模式为ftyp-moov-mdat。
(3) 切片MP4或fMP4，Fragmented MP4，box模式为ftyp-moov-moof-mdat。

### 1. 普通 MP4（General MP4）
**组织方式**： ftyp-mdat-moov
 
**特点**： box的顺序是 ftyp（文件类型）在开头，mdat（媒体数据）紧接其后，最后是 moov（元数据）。 在播放视频时，由于 moov box位于文件尾部，需要在文件完全下载后才能开始播放。
 
**适用场景**： 适用于本地存储和播放场景，例如在存储于硬盘或 USB 设备上的视频文件。 不适合流媒体或点播服务，因为需要在传输完所有数据后才能解码播放。

### 2. 快速 MP4（Faststart MP4）
**组织方式**： ftyp-moov-mdat
**特点**： box的顺序是 ftyp（文件类型）在开头，紧接其后的是 moov（元数据），最后才是 mdat（媒体数据）。 这种配置方式允许视频播放器在下载文件的初期就可以访问到 moov box，从而提前开始播放，提供更快的启动时间。
**适用场景**： 适用于需要更快启动时间的场景，如网络流媒体和点播服务。 可以在视频文件尚未完全下载的情况下就开始播放，提升用户体验。

### 3. 切片 MP4 或 fMP4（Fragmented MP4）
**组织方式**： ftyp-moov-moof-mdat
**特点**： 多个 moof 和 mdat 片段存在于文件中，每个片段包括它自己的元数据和媒体数据。 支持动态打包和传输，适合流媒体传输。 允许在各种网络条件下实现更灵活的文件下载和播放。
**适用场景**： 主要用于自适应流媒体传输。 适合需要频繁切换码率和分辨率的网络环境，例如移动网络。 提供更加平滑和连续的播放体验，适应不同带宽和播放条件。

以上三种组织方式都是基于 MP4容器格式，只是 box 的排列顺序不同，目的是为了适应不同的使用场景。它们都有相同的基本元素（ftyp、moov、mdat），只是添加了特定的box（如moof）来增强某些功能。

# mp4 文件格式概览
MP4⽂件的基本组成单元是box，也就是说MP4⽂件是由各种各样的box组成的，这些box之间存在⼀定的层次关系，box之间是树状结构。box中可以包含另一个box，这种box称为container box。可以将box理解为一个数据对象块

```
| box | box | box (box) |
```

```
|                            file                        |
| fytp |         moov                      | free | mdat |
       |        trak        |     trak     |
       | mdia | tkhd | edts |
       .......
```


一个mp4文件包含的box类型有很多，下面是3个比较重要的顶层box：
**ftyp**：File Type Box，描述文件遵从的MP4规范与版本，有且只有一个，一般在文件的开始位置。
**moov**：Movie Box，媒体数据的metadata信息，有且仅有一个，一般位于ftyp之后，也有的视频moov放在文件末尾。注意，当改变moov位置时，内部一些值需要重新计算。
**mdat**：Media Data Box，存放实际的媒体数据，可以有多个。

# mp4 box解析
box由header和body组成。
## header
包含了整个box的长度size和类型type。

**size**：整个box长度，占4个字节(32位)。当size = 0 时，代表这是文件中最后一个Box，通常包含在mdat box中； 当size=1时，意味着box长度需要更多bits来描述，在后面会定义一个64bits的largesize描述box的长度 (一般只有装载媒体数据的mdat box会用到largesize)；

**type**：box类型，包括 “预定义类型”、“自定义扩展类型”，占4个字节(32位)。
预定义类型：比如ftyp、moov、mdat等预定义好的类型；
自定义扩展类型：当type是uuid时，代表box中的数据是用户自定义扩展类型，size (或largesize)随后的16字节，为自定义类型的值 (extended_type)；

在部分box header中，还存在version、flags字段，这样的box叫做Full Box。相比Box，FullBox 多了 version、flags 字段。version：当前box的版本，为扩展做准备，占1个字节；flags：标志位，占24位，含义由具体的box自己定义。

### body
是box的实际数据，可以是纯数据也可以是更多的子box

# ftyp (File Type Box)
File Type Box，一般在文件的开始位置，描述文件的版本、兼容协议等。
“ftyp” body依次包括1个32位的major brand（4个字符），1个32位的minor version（整数）和1个以32位（4个字符）为单位元素的数组compatible brands。这些都是用来指示文件应用级别的信息

ftyp 内容：

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 major brand | 4 | 推荐兼容性的版本
 minor version | 4 | 最低兼容性的版本
 compatible brands[] | 4 | 所有的兼容性的版本

# mdat (Media Data Box)
存放具体的媒体数据。音视频数据的编解码信息都在其它box中保存，解析的时候先读取其它box的信息获取编解码信息以及音视频数据在mdat中的位置，然后再读取mdat获取音视频数据。

mdat的位置比较灵活，可以位于moov之前，也可以位于moov之后，但必须和stbl中的信息保持一致。

# free (Free Space Box)
free box 是可选的，如果存在，则通常出现在moov与mdat之间。
ree中的数据通常为全0，其作用相当于占位符，其内容是无关紧要的，可以被忽略。该box被删除后，不会对播放产生任何影响。

在实时拍摄视频，moov数据增多时，将free分配给moov使用。因为设备录制视频时并不能预先知道视频数据大小，如果moov在mdat之前，随着拍摄mdat的数据会增加，moov数据也会增多，如果没有free预留的空间，则要不停的向后移动mdat数据以腾出moov空间

# moov (Movie Box)
Movie Box包含了mp4文件媒体数据的metadata信息，“moov”是一个container box，具体内容信息由子box诠释。Movie Box，包含本文件中所有媒体数据的宏观描述信息以及每路媒体轨道的具体信息。"moov"⼀般位于放在⽂件末尾，但如果为了⽀持http边下载边播放则需要将moov提前。

一般情况下，“moov"中会包含1个"mvhd"和若干个"trak”。其中"mvhd"为Movie Header Box，记录mp4文件的整体信息，比如创建时间、文件时长等，一般作为“moov”的第一个子box出现（对于其他container box来说，header box都应作为首个子box出现）。“trak”为Track Box，是一个container box，一个mp4可以包含一个或多个轨道（比如视频轨道、音频轨道），轨道相关的信息就在trak里。

# mvhd (Movie Header Box)
Movie Header Box，记录整个mp4媒体文件的描述信息，如创建时间、修改时间、时间度量标尺、可播放时长等。

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。（以下字节数均按version=0）
 creation_time | 4 | 创建时间
 modification_time | 4 | 修改时间
 timescale | 4 | 表示时间粒度，如果是1000，表示1秒被分成1000个粒度
 duration | 4 | 媒体文件的总时长，该值需要除以timescale，转换成真正的秒数
 rate | 4 | 推荐播放速率，高16位和低16位分别为小数点整数部分和小数部分，即[16.16] 格式，该值为1.0（0x00010000）表示正常前向播放
 volume | 2 | 与rate类似，[8,8]格式，前8位表示整数，后8位表示浮点数，0x0100 = 1.0，最大音量播放
 reserved | 2 | 保留位
 matrix | 36 | 视频变换矩阵
 pre-defined | 24 | 保留位
 next_track_id | 4 | 下一个track使用的id号

# trak (Track Box)
"trak"是一个container box，其子box包含了该track (轨道) 的媒体数据引用和描述 (hint track除外)。

一个MP4文件可以包含多个track (比如视频轨道、音频轨道)，且至少有一个track，这些track之间彼此独立，有自己的时间和空间信息，track (轨道) 的相关的信息就在trak里。

“trak"必须包含一个"tkhd"和一个"mdia”，此外还有很多可选的box（略）。其中"tkhd"为track header box。“mdia"为media box，该box是一个包含一些track媒体数据信息box的container box。 我们只分析必的"tkhd”，“mdia”，至于其他例如"edts"有兴趣的同学可以自行了解。

track有三种类型：video track、audio track、hint track
video track：描述视频流
audio track：描述音频流
hint track：这个特殊的track并不包含媒体数据，而是包含了一些将其它数据track打包成流媒体的指示信息

# tkhd (Track Header Box)
Track Header Box，包含关于媒体流的头信息。
字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。（以下字节数均按version=0）
 creation_time | 8 or 4 | 创建时间（当version=0时字节数是4
 modification_time | 8 or 4 | 修改时间，当version=0时字节数是4
 track_id | 4 | id号，不能重复且不能为0
 reserved | 4 | 保留位
 duration | 8 or 4 | 媒体文件的总时长，该值需要除以timescale，当version=0时字节数是4
 reserved | 2 | 保留位
 layer | 2 | 视频层，默认为0，值小的在上层
 alternate_group | 2 | track分组信息，默认为0表示该track不与其他任何track相关
volume | 2 | 与rate类似，[8,8]格式，前8位表示整数，后8位表示浮点数，0x0100 = 1.0，最大音量播放
 matrix | 36 | 视频变换矩阵
 width | 4 | 视频的宽；音频track为0
 height | 4 | 视频的高【16.16】的格式存储；音频track为0


flags : 按位或操作结果值，预定义如下：
0x000001 track_enabled，否则该track不被播放；
0x000002 track_in_movie，表示该track在播放中被引用；
0x000004 track_in_preview，表示该track在预览时被引用。
一般该值为7，如果一个媒体所有track均未设置track_in_movie和track_in_preview，将被理解为所有track均设置了这两项；对于hint track，该值为0

# edts（edit Box）
不是所有的 MP4文件有这个 Box，子Box为elst（Edit List Box），它的作用是使某个track的时间戳产生偏移。

# mdia (Media Box)
在代表一流音频或视频流的track中，除tkhd外，主要的媒体配置信息在mdia中保存。描述了这条音视频轨/流(trak)的媒体数据样本的主要信息，对播放器来说是一个很重要的box。

Media Box，这是一个包含track媒体数据信息的container box。子box包括：
mdhd：Media Header Box，存放视频流创建时间，长度等信息。
hdlr：Handler Reference Box，媒体的播放过程信息。
minf：Media Information Box，解释track媒体数据的handler-specific信息。minf同样是个container box，其内部需要关注的内容是stbl，这也是moov中最复杂的部分。stbl包含了媒体流每一个sample在文件中的offset，pts，duration等信息。想要播放一个mp4文件，必须根据stbl正确找到每个sample并送给解码器。

# minf (Media Information Box)
minf box 顾名思义，是专门用来描述当前track下媒体数据的box，它是一个container box，其内容由其子box来表达

一般情况下，minf包含了一个header box，一个dinf和一个stbl，其中header box的数据类型有vmhd（视频轨）、smhd（音轨）、hmhd（hint track）、nmhd（null track），dinf为data information box，stbl为sample table box。
(1) vmhd：Video Media Information Header，定义颜色和图形模式信息，只有video track才有此box
(2) smhd： Sound Media Information Header，定义了声音媒体的控制信息，只有audio track才有此box
(3) stbl：Sample Table，包含了音视频数据的索引以及时间信息，也就是音视频数据的映射


# stbl (Sample Table Box)

“stbl”几乎是普通的MP4文件中最复杂的一个box了，首先需要回忆一下sample的概念。在mp4文件中，sample是一个媒体流的基本单元，例如视频流的一个sample代表实际的nal数据。chunk是一系列sample数据的集合。在MP4文件中，媒体数据是由一个或多个track组成，而一个track是由一个或多个chunk组成，每个chunk可包含一个或多个sample。如下图所示。

“stbl”包含了关于track中sample所有时间和位置的信息，以及sample的编解码等信息。stbl用来描述每个sample的信息，利用这个表，可以解释sample的时序、类型、大小以及在各自存储容器中的位置。

“stbl”是一个container box，其子box包括：sample description box（stsd）、time to sample box（stts）、sample size box（stsz或stz2）、sample to chunk box（stsc）、chunk offset box（stco或co64）、composition time to sample box（ctts）、sync sample box（stss）等。

下面是这几个box概要的介绍：

stsd：sample description box，样本的描述信息，存储了编码类型和初始化解码器需要的信息，并与具体编解码器类型有关。给出视频、音频的编码、宽高、音量等信息，以及每个sample中包含多少个frame。
* stco/co64：chunk offset box，chunk在文件中的偏移；
* stsc：sample to chunk box，sample-chunk映射表，每个chunk中包含几个sample；
* stsz/stz2：sample size box，每个sample的size（单位是字节）；
* stts：time to sample box，存储了该 track 每个 sample 到 dts 的时间映射关系。
* stss：sync sample box，针对视频，关键帧的序号；
* ctts：composition time to sample box，sample的CTS与DTS的时间差的压缩表；

## stsd (Sample Description Box)
Sample Description Box，存储了编码类型和初始化解码器需要的信息，视频的编码类型、宽高、长度，音频的声道、采样等信息都会出现在这个box中。

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。（以下字节数均按version=0）
 entry_count |  4 | entry 个数

version字段后会有一个entry count字段，根据entry的个数，每个entry会有type信息，如“vide”、“sund”等，根据type不同sample description会提供不同的信息，例如对于video track，会有“VisualSampleEntry”类型信息，对于audio track会有“AudioSampleEntry”类型信息。

字段 | bytes |意义
---------|----------|---------
 AudioSampleEntry |  - | 子 box，当 handler_type=soun 时才有
 VisualSampleEntry |  - | 子 box，当 handler_type=vide 时才有
 HintSampleEntry |  - | 子 box，当 handler_type=hint 时才有
 MetadataSampleEntry |  - | 子 box，当 handler_type=meta 时才有

 video track的stsd body内容（不同的编码方式，stsd的子box的名称会有不同，但是box中的字段都是相同的）：

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 data_reference_index | 2 | 当MP4文件的数据部分，可以被分割成多个片段，每一段对应一个索引，并分别通过URL地址来获取，此时，data_reference_index 指向对应的片段（比较少用到）
 width | 2 | 视频的宽，单位是像素
 height |  2 | 视频的高，单位是像素
 horizresolution | 2 | 水平方向的分辨率（像素/英寸），默认是0x00480000（72dpi）
 vertresolution |  2 | 垂直方向的分辨率（像素/英寸），默认是0x00480000（72dpi）
 frame_count |  2 | 一个sample中包含多少个frame，对video track来说，默认是1
 compressorname |  32 | 仅供参考的名字，通常用于展示，占32个字节，比如 AVC Coding。第一个字节，表示这个名字实际要占用N个字节的长度。第2到第N+1个字节，存储这个名字。第N+2到32个字节为填充字节。compressorname 可以设置为0
 depth |  2 | 位图的深度信息，比如 0x0018（24），表示不带alpha通道的图片

 audio track的stsd body内容（不同的编码方式，stsd的子box的名称会有不同，但是box种的字段都是相同的）：
字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 data_reference_index | 2 | 当MP4文件的数据部分，可以被分割成多个片段，每一段对应一个索引，并分别通过URL地址来获取，此时，data_reference_index 指向对应的片段（比较少用到）
 channel_count |  | 声道数，取值为1或2
 samplesize |   | 采样位宽，一般为8bit或16bit
 samplerate |  | 采样率

## stts (Time To Sample Box)

“stts”记录着该sample到下一个sample的DTS间隔，存储了sample的duration，包含了DTS到sample number的映射表，主要用来推导每个帧的时长，描述了sample时序的映射方法，我们通过它可 以找到任何时间的sample。

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。
 entry_count |  4 | entry 个数

每个条目
 字段 | bytes |意义
---------|----------|---------
 sample_count |  4 | 单个entry中，具有相同时长（duration 或 sample_delta）的连续sample的个数
 sample_delta |  4 | 单个 sample 的播放时长，单位为 timescale，也可以说是相邻两个sample之间dts的差值

## stss (Sync Sample Box)
"stss"是视频track独有，它包含关键帧所在的 sample 序号。
关键帧是为了支持随机访问。如果没有stss的话，所有的sample中都是关键帧。

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。
 entry_count |  4 | entry 个数

每个条目
 字段 | bytes |意义
---------|----------|---------
 sample_number |  4 | sample 计数，从 1 开始

若有 4个条目 且 sample_number= 1,100,200,400
则第1帧、第100帧、第200帧、第400帧，是关键帧

## ctts（Composition Time to Sample Box）
存储了每个 sample 从解码（dts）到渲染（pts）之间的差值。
对于只有I帧、P帧的视频来说，解码顺序、渲染顺序是一致的，此时，ctts没必要存在。
对于存在B帧的视频来说，ctts就需要存在了。当PTS、DTS不相等时，就需要ctts了，公式为 CT(n) = DT(n) + CTTS(n) 。

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。
 sample_counts |  4 | 单个entry中，具有相同差值（pts-dts）的连续sample的个数
sample_offsets |  4 | 从解码（dts）到渲染（pts）之间的差值

## stsc (Sample To Chunk Box)
stsc中用一个表描述sample与chunk映射表，每个chunk中包含几个sample，查看这张表就可以找到包含指定sample的chunk，从而找到这个sample。

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。
 entry_count |  4 | entry 个数

每个条目
 字段 | bytes |意义
---------|----------|---------
 first_chunk |  4 | 当前表项中，对应的第一个chunk的序号
 samples_per_chunk |  4 | 同一个entry中每个chunk包含的sample数
 sample_description_index |  4 | chunk使用的stsd的序号，即不同chunk可以使用不同编解码信息

## stsz/stz2 (Sample Size Box)
“stsz” 定义了每个sample的大小，包含了媒体中全部sample的数目和一张给出每个sample大小的表。这个box相对来说体积是比较大的。

有两种不同的box类型：stsz、stz2

stsz

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。
 sample_size |  4 | 默认的sample大小（单位是byte），通常为0。如果sample_size不为0，那么，所有的sample都是同样的大小。如果sample_size为0，那么，sample的大小可能不一样
 sample_count |  4 | 当前track里面的sample数目。如果 sample_size==0，那么 等于下面entry_size(sample_sizes)中的数据个数
entry_size (sample_sizes) |  4 | 单个sample的大小 (如果sample_size==0的话)

stz2
字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0。
 field_size |  1 | entry表中，每个entry_size占据的位数（bit），可选的值为4、8、16。4比较特殊，当field_size等于4时，一个字节上包含两个entry，高4位为entry[i]，低4位为entry[i+1]
 sample_count |  4 | 等于下面entry_size中的数据个数
entry_size  |  4 | 单个sample的大小

## stco/co64 (Chunk Offset Box)
stco 定义了每个chunk在媒体流中的偏移量(offsets)。这个位置是在整个文件中的，而不是在任何box中的，这样做就可以直接在文件中找到媒体数据，而不用解析box。需要注意的是一旦前面的box有了任何改变，这张表都要重新建立，因为位置信息已经改变了。

字段 | bytes |意义
---------|----------|---------
 size | 4 | 
 type | 4 | 
 flags | 3 | 
 version | 1 | box版本，0或1，一般为0
 chunk_offsets | 4 | 在文件本身中的 offset，而不是某个box内部的偏移