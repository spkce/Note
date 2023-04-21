## 添加文件
	git add xx
	git commit -m "注释"
	git push 

## 删除文件
	git rm xxx
	git commit -m "注释"
	git push 

## git重命名文件和文件夹
	git mv -f oldfolder newfolder
	git add -u newfolder (-u选项会更新已经追踪的文件和文件夹)
	git commit -m "changed the foldername whaddup"
	git push 



## 还原仓库
git clean -f && git reset --hard
 工作目录和缓存区回到最近一次commit时候一摸一样的状态
reset只影响被track过的文件, 所以需要clean来删除没有track过的文件. 

## commit回退
执行commit后，还没执行push时，想要撤销这次的commit
```
git reset --soft HEAD^
```
HEAD^ 表示上一个版本，即上一次的commit，也可以写成HEAD~1
如果进行两次的commit，想要都撤回，可以使用HEAD~2

--soft
不删除工作空间的改动代码 ，撤销commit，不撤销git add file

--hard
删除工作空间的改动代码，撤销commit且撤销add

回退到指定id
git reset --hard commit_id  //退到/进到 指定的commit

## git add 退回

```
git reset HEAD <文件名>
```