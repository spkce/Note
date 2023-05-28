set autolist 自动补齐


像RedHat那样在ls时显示多种颜色
setenv LSCOLORS ExGxFxdxCxegedabagExEx
setenv CLICOLOR yes


普通用户有类似root用户命令行编辑能力
bindkey "^W" backward-delete-word
bindkey -k up history-search-backward
bindkey -k down history-search-forward

显示类似[delphij@spirit]
set prompt = "%B{\e[35m%}%n@%m:%{\e[34m%}%C>%{\e[0m%} "

set nobeep #关闭警报声