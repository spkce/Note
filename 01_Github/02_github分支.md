## 如果要下载xxx分支
git checkout -b xxx(本地分支) origin/xxx(远程分支)
如果要在本地的不同分支见切换
git checkout xxx

git checkout -b   develop origin/develop  拉取远程分支

如果当前分支有修改，可以使用git reset重置，或者使用git stash保存修改。

## git branch用法：
```
git branch     列出本地已经存在的分支，并且在当前分支的前面加“*”号标记
git branch -r  列出远程分支
git branch -a  列出本地和远程分支
git branch name 创建新的本地分支，但不进行分支切换
git branch -m | -M oldbranch newbranch 重命名分支，如果newbranch名字分支已经存在，则需要使用-M强制重命名，否则，使用-m进行重命名
git branch -d | -D branchname 删除branchname分支
git branch -d -r branchname 删除远程branchname分支

```

## 拉取远程分支
```
git checkout -b develop origin/develop
```
## 删除远程分支
```
git push origin --delete branch_name
```

## 推送新分支到远程
```
git push origin newbranch

```
## 将当前修改推送到一个新分支（临时）
```
git push origin master : temp

```
## merge

```
git merge <branchname>
git rebase <branchname>
git cherry-pick <submitID>

cherry-pick 多个提交
git cherry-pick <HashA> <HashB>

cherry-pick A到B多个提交 不包含A
git cherry-pick A..B 

cherry-pick A到B多个提交 包含A
git cherry-pick A^..B 

git cherry-pick 是本地操作，
从A分支cherry-pick到B分支时，请确保cherry-pick 的版本以及pull到本地A分支
git cherry-pick bad object
```

## 回退到某个版本
 如果想恢复到之前某个提交的版本，且那个版本之后提交的版本我们都不要了，就可以用这种方法

 git reset --hard commitId

git revert是用于“反做”某一个版本，以达到撤销该版本的修改的目的。比如，我们commit了三个版本（版本一、版本二、 版本三），突然发现版本二不行（如：有bug），想要撤销版本二，但又不想影响撤销版本三的提交，就可以用 git revert 命令来反做版本二，生成新的版本四，这个版本四里会保留版本三的东西，但撤销了版本二的东西。
 git revert