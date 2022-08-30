getopt函数
1、定义：

int getopt(int argc, char * const argv[], const char *optstring);
2、描述：

getopt是用来解析命令行选项参数的，但是只能解析短选项: -d 100,不能解析长选项：--prefix
3、参数：

argc：main()函数传递过来的参数的个数
argv：main()函数传递过来的参数的字符串指针数组
optstring：选项字符串，告知 getopt()可以处理哪个选项以及哪个选项需要参数
4、返回：

如果选项成功找到，返回选项字母；如果所有命令行选项都解析完毕，返回 -1；如果遇到选项字符不在 optstring 中，返回字符 '?'；如果遇到丢失参数，那么返回值依赖于 optstring 中第一个字符，如果第一个字符是 ':' 则返回':'，否则返回'?'并提示出错误信息。

5、下边重点举例说明optstring的格式意义：

optstring是一个包含合法选项字符的字符串。如果这样的字符后跟冒号，则该选项需要一个参数。

例子：
char*optstring = “ab:c::”;
单个字符a         表示选项a没有参数            格式：-a即可，不加参数
单字符加冒号b:     表示选项b有且必须加参数      格式：-b 100或-b100,但-b=100错
单字符加2冒号c::   两个冒号表示一个选项带有一个可选参数；
                 
上面这个 optstring 在传入之后，getopt 函数将依次检查命令行是否指定了 -a， -b， -c(这需要多次调用 getopt 函数，直到其返回-1)，当检查到上面某一个参数被指定时，函数会返回被指定的参数名称(即该字母)


optarg —— 指向当前选项有参数值，则是指向参数值的指针。
optind —— argv中待处理的下一个元素的索引。系统将此值初始化为 1。调用者可以将其重置为 1 以重新开始扫描相同的argv
optopt —— 最后一个未知选项。
opterr ­—— 如果不希望getopt()打印出错信息，则只要将全域变量opterr设为0即可。
默认情况下，getopt () 会在扫描时置换argv的内容，以便最终所有非选项都位于末尾。 

如果getopt () 不能识别选项字符，它会向stderr打印一条错误消息，将字符存储在optopt中，并返回“？”。

getopt_long () 函数

1、定义：

getopt_long () 函数的工作方式与getopt () 类似，只是它也接受长选项，以两个破折号开头。如果缩写是唯一的或与某些定义的选项完全匹配，则可以缩写长选项名称。长选项可以采用--arg=param或--arg param形式的参数。

int getopt_long(int argc, char * const argv[],
           const char *optstring,
           const struct option *longopts, int *longindex);
2、参数及描述：

longopts是指向< getopt.h >中 声明的struct option 数组的第一个元素的指针，

数组的最后一个元素必须用零填充。

struct option {
const char  *name;       /* 参数名称 */
int          has_arg;    /* 指明是否带有参数 */
int          *flag;      /* flag=NULL时,返回value;不为空时,*flag=val,返回0 */
int          val;        /* 用于指定函数找到选项的返回值或flag非空时指定*flag的值 */
}; 
选项结构里面指明了一个长参数的名称和相关属性。

其中 has_arg 指明是否带参数值，其数值可选：

no_argument 表明长选项不带参数，如：--name, --help

required_argument 表明长选项必须带参数，如：--prefix /root或 --prefix=/root optional_argument 表明长选项的参数是可选的，如：--help或 –prefix=/root，其它都是错误

longindex   如果longindex非空，它指向的变量将记录当前找到参数符合longopts里的第几个元素的描述，即是longopts的下标值
3、返回：

对于短选项，返回值同getopt函数；对于长选项，如果flag是NULL，返回val，否则返回0；对于错误情况返回值同getopt函数。
参考链接：
