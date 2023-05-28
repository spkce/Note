git的配置：
	创建一个目录作为git仓库，在此目录下执行
		git init
	在该目录下就会出现一个.git的文件

创建ssh key
	使用下面的命令在本地创建一个ssh key
		 ssh-keygen -t rsa -C "xx@xx.com"
	该命令的中最后一个字符串就是注册github账号的邮箱地址。在该过程中会要求设置密码，直接回车表示不设置密码。重复密码时也直接回车。
	最后会在id_rsa的文件中生成ssh key。打开github的网页，把ssh key填入。这样就可以把本地代码上传到github上了。

测试是否成功
ssh -T git@github.com

如果出现
Hi XXXX! You've successfully authenticated, but GitHub does not provide shell access.
那么表示已经成功连上github

配置git的配置文件username和email
#配置用户名
git config --global user.name "我的用户名"
#配置email
git config --global user.email "我的github所使用的注册邮箱"


将本地的仓库关联到github上：
	在github上创建一个代码仓库，进入仓库界面可以看见仓库的地址https://github.com/xxxxx
	在本地执行下面的命令
		git remote add origin https://github.com/xxxxx
	这样就把本地仓库关联到github上了。
	

## 上传代码：
	执行
		git add . #注意add 和 . 之间是有空格的。
	将所有文件添加到仓库中,然后执行
		git commit -m "注释语句"
	添加文件注释。最后执行
		git push -u origin master
	将代码submit到github上。实测可以只执行git push
	另外执行git pull则是将代码从github更新到本地。

## 查看当前分支的url
	`git remote show origin`

## github token
自从2021年8月13日后不再支持用户名密码的方式验证，推送代码需要创建个人访问令牌（personal access token）

令牌生成： github网页settings -> Developer settings -> Personal access tokens -> Generate new token
(Expiration为有效时间，最好选择NO Expiration)

生成后在仓库中设置:
`git remote set-url origin https://<token>@github.com/<username>/<repo>.git`

## 查看仓库url地址
git remote show origin

# git clone 仓库太大问题
只克隆一个commit id， 工程太大clone不到本地可用此方法
```
git clone --depth=1
```
这样的话使用`git branch -a`是看不到其他分支的。
若要clone 其他分支
```
$ git remote set-branches --add origin  origin 'remote_branch_name'
$ git fetch --depth 1 origin remote_branch_name
$ git checkout remote_branch_name
```

# 设置git 默认编辑器
git config --global core.editor vim


# 防止每次都需要输入密码
git config --global credential.helper store
