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

## git log 

git log [选项] [分支名/提交哈希]
常用的选项包括：

-p：显示提交的补丁（具体更改内容）。
--oneline：以简洁的一行格式显示提交信息。
--graph：以图形化方式显示分支和合并历史。
--decorate：显示分支和标签指向的提交。
--author=<作者>：只显示特定作者的提交。
--since=<时间>：只显示指定时间之后的提交。
--until=<时间>：只显示指定时间之前的提交。
--grep=<模式>：只显示包含指定模式的提交消息。
--no-merges：不显示合并提交。
--stat：显示简略统计信息，包括修改的文件和行数。
--abbrev-commit：使用短提交哈希值。
--pretty=<格式>：使用自定义的提交信息显示格式。

## 修改最后一次提交
 git commit --amend 以新的改进后的提交来 替换 掉旧有的最后一次提交


 git commit --amend --no-edit