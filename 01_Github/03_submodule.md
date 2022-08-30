
## 添加子模块
git submodule add <url> <path>
```
$ git submodule add https://github.com/spencermountain/spacetime.git

```
## 删除子模块
直接删掉子模块
$ rm -rf sub_folder
到.git文件夹下大扫除（删掉所有相关的东西咯）
$ cd .git
$ vim config 删掉submodule信息
$ cd modules
$ rm -rf * 注意一定要慎重！千万别敲错了，或者可以到文件夹手动删除，防止敲错误删
$ cd ../..
继续清理
如果直接add子模块，会提示'sub_folder already exists in the index'，所以需要
$ git rm --cached sub_folder
然后可以确认一下
$git ls-files --stage www/topsdk
如果提示Please stage your changes to .gitmodules or stash them to proceed直接删掉.gitmodules文件即可
$ rm .gitmodules
这时候就可以进行add操作了
$ git add remote_repo sub_folder_path
