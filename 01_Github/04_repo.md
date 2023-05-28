
# 为什么要用repo
项目模块化/组件化之后各模块也作为独立的 Git 仓库从主项目里剥离了出去，各模块各自管理自己的版本。Android源码引用了很多开源项目，每一个子项目都是一个Git仓库，每个Git仓库都有很多分支版本，为了方便统一管理各个子项目的Git仓库，需要一个上层工具批量进行处理，因此repo诞生。
repo也会建立一个Git仓库，用来记录当前Android版本下各个子项目的Git仓库分别处于哪一个分支，这个仓库通常叫做：manifest仓库(清单库)。


# repo init
```shell
repo init [options] [manifest url]
```
例如：
```shell
repo init -u manifest_git_path -m manifest_file_name -b branch_name --repo-url=repo_url --no-repo-verify
```
效果：
首先当前目录产生一个.repo目录。然后克隆一份repo的源代码到.repo/repo下，里面存放了其他repo子命令，即repo的主体部分。
接着从manifest_git_path仓库地址clone清单库到.repo/manifests和.repo/manifests.git目录。同时.repo目录下还包括manifest仓库(清单库)内容


* -u：指定Manifest库的Git访问路径。唯一必不可少的选项
* -m：指定要使用的Manifest文件。不指定的话，默认为default.xml文件
* -b：指定要使用Manifest仓库中的某个特定分支。
* --repo-url：指定repo的远端repoGit库的访问路径。
* --no-repo-verify：指定不要验证repo源码。
* --mirror：创建远程存储库的副本，而不是客户端工作目录。该选项用于创建版本库镜像。使用该选项则在下一步repo sync同步时，本地按照源的版本库组织方式进行组织，否则会按照 manifest.xml 指定的方式重新组织并检出到本地


# .repo文件夹简介
```shell
$ tree .repo -L 1
.repo
├── manifests
├── manifests.git
├── manifest.xml
└── repo

3 directories, 1 file
```

manifests : manifest仓库(清单库)内容，即repo init的-u选项对应的仓库
manifests.git : manifest仓库(清单库)的.git目录
manifest.xml : 指明当前生效的Manifest文件，即repo init的-m选项对应的参数(没有该选项时默认为default.xml)
repo : repo 命令的主体，包含了最新的 repo 命令


# manifest文件介绍
所谓manifest仓库(清单库)其实就是存放manifest(清单)文件的仓库，实际上可以是任意仓库，只要该仓库中存在repo init命令-m选项指定的manifest文件即可，清单库命名为manifest只不过是一种约定俗成的写法罢了。

manifest仓库一般都会有一个default.xml文件，该文件为默认的manifest文件

# manifest文件格式
manifest文件是用XML文件的格式记录了本项目依赖的各个Git仓库的名称、地址，以及分支等信息。

下面举个实际的例子:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
    <remote fetch="ssh://git@git.software.team/learn-repo" name="origin" review="http://xxx.xxx.xxx:8080"/>
    <remote fetch="https://github.com" name="github" alias="origin"/>
    <default remote="origin" revision="master" sync-j="4" />
    <project name="build" path="build">
        <linkfile dest="build.sh" src="build.sh"/>
    </project>
    <project name="docs" path="docs">
        <copyfile dest="README.md" src="README.md"/>
    </project>
    <project name="third_party/openssl" path="third_party/openssl" revision="OpenSSL_1_1_1l" />
    <project name="curl/curl" path="third_party/curl" remote="github" />
    <project name="src" path="src" revision="release" />
</manifest>
```
### 1. remote元素
remote元素可以有多个，用于存在多个git远程服务器的情况。

fetch：使用此remote的所有项目的Git URL前缀。 每个项目的名称都附加到此前缀以形成用于克隆项目的实际 URL。如果使用此remote的所有项目的前缀和manifest仓库前置一致的话，可以使用..代替。
name：此清单文件唯一的短名称。此处指定的名称用作每个项目的 .git/config 中的远程名称，因此可自动用于 git fetch、git remote、git pull 和 git push 等命令。
review：通过repo upload将评论上传到的 Gerrit 服务器的主机名。 该属性是可选的； 如果未指定，则repo upload将不起作用。
alias：该属性可以省略，当指定了该属性时，可以覆盖name属性设置每个项目的.git/config中的远程名称。不同remote元素的alias属性可以相同，比如不同remote元素的alias属性可以都是origin。
## 2. default元素
default元素只能有一个。

remote：project部分不单独指定remote的话就使用default部分的。
revision：project部分不单独指定revision的话就使用default部分的。
sync-j：同步时(执行repo sync命令时)使用的并行作业数
sync_c：如果设置为true，则只同步指定的分支(revision 属性指定)，而不是所有的ref内容
sync_s：如果设置为true，则会同步git的子项目
## 3. project元素
该部分定义了项目代码由哪些子仓库组成

name：相对于remote部分fetch指定的前缀的相对路径
path：把代码下载下来后在本地的相对于当前路径的相对路径
revision：是指下载下来的代码要checkout到哪个revision上，这里的revision可以是commit id、branch name、tag name，本质上都是commit id。default.xml中通常用branch name做revision，可以下载到并且checkout出该branch上最新的代码，标签和/或commit id在理论上应该有效，但尚未经过广泛测试。如果revision用commit id的话，那后面必须跟上upstream，upstream的值是个branch name。revision部分如果省略的话，就等于使用default部分定义的revision。
remote：前面定义的远程元素的名称。如果未提供，则使用默认元素提供的值。
## 4. copyfile元素
project元素的子元素，每个元素描述了一对 src-dest 文件对。同步时(即执行repo sync命令时)src文件会被拷贝到dest。通常会被用于 README 或 Makefile 或其他构建脚本。
dest：是相对于当前目录(执行repo init和repo sync命令的目录)的路径
src：是相对于project path的相对路径

## 5. linkfile元素
与copyfile类似，只不过不是拷贝，而是建立软连接。

更多关于manifest文件的格式的介绍，可以在repo init命令成功执行后，在代码根目录下的.repo/repo/docs下看到相关文档说明(问：不会写manifest文件，就无法创建清单库，从而无法执行repo init怎么办？答：找个开源清单库执行repo init即可，或者去github上下载一份repo的源码)。最简单的方式是直接去网站https://gerrit-googlesource.proxy.ustclug.org/git-repo/+/HEAD/docs/manifest-format.md查看帮助。

# repo sync
初始化好一个 repo 工作目录后下一步就是把代码同步下来了，该命令用来下载新的更改并更新本地环境中的工作文件。如果您在未使用任何参数的情况下运行 repo sync，则该操作会同步所有项目(所有项目是指manifest文件中所有的project元素)的文件。

* project ：为manifest文件中project元素的name属性或者path属性的值。如果只需要同步某一个或者个别几个项目的话，就可以采用这种方法。

运行repo sync 后，将出现以下情况：
* 如果目标项目从未同步过，则 repo sync 相当于 git clone。远程代码库中的所有分支都会复制到本地项目目录中。
* 如果目标项目已同步过，则 repo sync 相当于以下命令：
```shell
git remote update
git rebase origin/<BRANCH>
```
* 如果 git rebase 操作导致合并冲突，那么您需要使用普通 Git 命令（例如 git rebase --continue）来解决冲突