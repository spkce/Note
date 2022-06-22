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

## git branch用法：
git branch     列出本地已经存在的分支，并且在当前分支的前面加“*”号标记
git branch -r  列出远程分支
git branch -a  列出本地和远程分支
git branch name 创建新的本地分支，但不进行分支切换
git branch -m | -M oldbranch newbranch 重命名分支，如果newbranch名字分支已经存在，则需要使用-M强制重命名，否则，使用-m进行重命名
git branch -d | -D branchname 删除branchname分支
git branch -d -r branchname 删除远程branchname分支

## 如果要下载xxx分支
git checkout -b xxx(本地分支) origin/xxx(远程分支)
如果要在本地的不同分支见切换
git checkout xxx

如果当前分支有修改，可以使用git reset重置，或者使用git stash保存修改。

## 还原仓库
git clean -f && git reset --hard
 工作目录和缓存区回到最近一次commit时候一摸一样的状态
reset只影响被track过的文件, 所以需要clean来删除没有track过的文件. 
