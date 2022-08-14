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

