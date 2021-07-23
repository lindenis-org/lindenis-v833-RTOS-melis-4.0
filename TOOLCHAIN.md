# GCC优化选项  
　　GCC有很多优化选项，一般包括O1，O2, O3，Os，Og等， 一般很容易通过在线手册找到对应gcc版本的上述优化选项的细节， 除了开启一些选项之外，GCC的优化也设置一些parmas，各个版本的GCC优化选项细节可以从下面的网站获得:    
  https://gcc.gnu.org/onlinedocs/
  
#获得本机安装的gcc的优化细节
  由于编译的具体细节除了因不同版本有区别外，还与gcc release的时候的编译参数，目标平台（例如AIX，Linux， Solaris等）版本等有关。那么有什么办法获得本机当前安装的gcc的优化细节呢？可以通过以下命令：    
 
  gcc -Q --help=optimizers    
  
  查看开启和关闭了哪些选项。

  通过以下命令：    

  gcc -Q --help=params

  可以查看有哪些params，遗憾的是我仍然不知道每个O优化到底如何设置了这些params。    
 
  gcc -c -Q -O3 --help=optimizers > /tmp/O3-opts    
  gcc -c -Q -O2 --help=optimizers > /tmp/O2-opts    
  diff /tmp/O2-opts /tmp/O3-opts | grep enabled
#Git清除文件历史记录


1. git filter-branch --force --index-filter 'git rm --cached --ignore-unmatch **path/to/your/remove/file**' --prune-empty --tag-name-filter cat -- --all
其中, path-to-your-remove-file 就是你要删除的文件的相对路径(相对于git仓库的跟目录), 替换成你要删除的文件即可. 注意一点，这里的  文件或文件夹，都不能以 '/' 开头，否则文件或文件夹会被认为是从 git 的安装目录开始。
如果你要删除的目标不是文件，而是文件夹，那么请在 `git rm --cached' 命令后面添加 -r 命令，表示递归的删除（子）文件夹和文件夹下的文件，类似于 `rm -rf` 命令。
2. git push origin master --force --all
3. git push origin master --force --tags
4. rm -rf .git/refs/original/
5. git reflog expire --expire=now --all
6. git gc --prune=now
7. git gc --aggressive --prune=now

现在你再看看你的.git目录文件大小是不是变小了.
#对melis3.0 代码进行静态检查
使用cppcheck工具对代码执行动态检查

1. 安装, 执行 sudo apt-get install cppcheck安装cppcheck.
2. 进入待检查源码目录， 执行 cppcheck --enable=all ./ 
3. 分析cppcheck输出日志，对error项目重点清除.
#char, signed char以及unsigned char的区别 
1. C标准中对char是 Impementation Defined，就是未明确定义
2. 具体的编译器明确定义，一般都是用signed char或unsigned char来实现char的，也就是说不同的编译器对char是什么定义不一样
3. 为什么要这样定义？因为这三种类型的对象在存储介质中的表现形式是一样的（都是一个占8bit的01串，只是解析的时候不同）
4. 到底是signed char还是unsigned char？这得看编译器：VC编译器、x86上的GCC都把char定义为signed char，而arm-linux-gcc却把char定义为 unsigned char
所以，Melis3.0使用__s8类型和char类型的时候要非常小心，否则容易发生问题.
在Melis3.0中，__s8的定义为:　
**typedef signed char __s8**
而arm-melis-eabi-gcc则认为char为 unsigned char,　所以在melis3.0上面__s8和char并不等价，不能互相替换。
#objump反汇编raw binary文件
1. arm-melis-eabi-objdump -b binary -m arm --adjust-vma=0Xxxxxxxxx -D fel-sdboot.sunxi
0Xxxxxxxxx是binary的运行地址，可以根据实际情况设置.
