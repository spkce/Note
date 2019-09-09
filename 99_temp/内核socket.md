## socket
socket的定义为:

```C++
#include <sys/types.h>
#include <sys/socket.h>

int socket(int family, int type, int protocol);
```
在内核中`socket`的定义为：
```C++
//文件 net/socket.c
SYSCALL_DEFINE3(socket, int, family, int, type, int, protocol)
{
	int retval;
	struct socket *sock;
	int flags;

	/* Check the SOCK_* constants for consistency.  */
	BUILD_BUG_ON(SOCK_CLOEXEC != O_CLOEXEC);
	BUILD_BUG_ON((SOCK_MAX | SOCK_TYPE_MASK) != SOCK_TYPE_MASK);
	BUILD_BUG_ON(SOCK_CLOEXEC & SOCK_TYPE_MASK);
	BUILD_BUG_ON(SOCK_NONBLOCK & SOCK_TYPE_MASK);

	flags = type & ~SOCK_TYPE_MASK;
	if (flags & ~(SOCK_CLOEXEC | SOCK_NONBLOCK))
		return -EINVAL;
	type &= SOCK_TYPE_MASK;

	if (SOCK_NONBLOCK != O_NONBLOCK && (flags & SOCK_NONBLOCK))
		flags = (flags & ~SOCK_NONBLOCK) | O_NONBLOCK;

	retval = sock_create(family, type, protocol, &sock);
	if (retval < 0)
		goto out;

	retval = sock_map_fd(sock, flags & (O_CLOEXEC | O_NONBLOCK));
	if (retval < 0)
		goto out_release;

out:
	/* It may be already another descriptor 8) Not kernel problem. */
	return retval;

out_release:
	sock_release(sock);
	return retval;
}
```

可以看出，`socket`函数首先进行入参校验，然后使用`sock_create`初始化struct socket。然后使用`sock_map_fd`将struct socket和文件描述符进行绑定。最后传出文件描述符。
```C++
//文件 net/socket.c
int sock_create(int family, int type, int protocol, struct socket **res)
{
	return __sock_create(current->nsproxy->net_ns, family, type, protocol, res, 0);
}
```
`sock_create`仅仅是对`__sock_create`的封装，current是一个宏获取当前线程的信息。
`__sock_create`函数使用`sock_alloc`申请一个struct socket，然后使用传入的参数family申请一个struct net_proto_family结构体，该结构有两个成员：一个family成员表示所属协议，一个create函数指针，用来对struct socket进行创建。获取struct net_proto_family后创建struct socket。

## sendto
sendto)系统调用-->
    sock_sendmsg()-->
        __sock_sendmsg()-->
            __sock_sendmsg_nosec()-->
                sock->ops->sendmsg()-->
                    udp_sendmsg()


sendto的声明为：
```C++
#include <sys/types.h>
#include <sys/socket.h>

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
```
在内核中sendto的定义为
```C++
//文件 net/socket.c
SYSCALL_DEFINE6(sendto, int, fd, void __user *, buff, size_t, len,
		unsigned, flags, struct sockaddr __user *, addr,
		int, addr_len)
{
	struct socket *sock;
	struct sockaddr_storage address;
	int err;
	struct msghdr msg;
	struct iovec iov;
	int fput_needed;

	if (len > INT_MAX)
		len = INT_MAX;
	sock = sockfd_lookup_light(fd, &err, &fput_needed);
	if (!sock)
		goto out;

	iov.iov_base = buff;
	iov.iov_len = len;
	msg.msg_name = NULL;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_namelen = 0;
	if (addr) {
		err = move_addr_to_kernel(addr, addr_len, (struct sockaddr *)&address);
		if (err < 0)
			goto out_put;
		msg.msg_name = (struct sockaddr *)&address;
		msg.msg_namelen = addr_len;
	}
	if (sock->file->f_flags & O_NONBLOCK)
		flags |= MSG_DONTWAIT;
	msg.msg_flags = flags;
	err = sock_sendmsg(sock, &msg, len);

out_put:
	fput_light(sock->file, fput_needed);
out:
	return err;
}
```
`sendto`函数使用`sockfd_lookup_light`根据文件描述符获取struct socket，通过`move_addr_to_kernel`将用户空间的数据拷贝到内核空间中，并使用msg和iov打包数据。然后用`sock_sendmsg`发送数据
```C++
//文件 net/socket.c
int sock_sendmsg(struct socket *sock, struct msghdr *msg, size_t size)
{
	struct kiocb iocb;
	struct sock_iocb siocb;
	int ret;

	init_sync_kiocb(&iocb, NULL);
	iocb.private = &siocb;
	ret = __sock_sendmsg(&iocb, sock, msg, size);
	if (-EIOCBQUEUED == ret)
		ret = wait_on_sync_kiocb(&iocb);
	return ret;
}
```
`sock_sendmsg`首先定义并初始化一个struct kiocb，linux内核中所有I/O操作都要依赖于此结构。然后调用`__sock_sendmsg`
udp_sendmsg函数的定义（伪代码）
```C++
//文件 net/ipv4/udp.c
int udp_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len)

```
1. 使用inet_sk()和udp_sk()获得struct inet_sock和struct udp_sock
2. 判断该UDP套接字上是否有等待发送的IP数据包，若有则将当前数据通过ip_append_datat添加到队列尾端。
3. 若没有等待发送的IP数据包。则将upd头长度添加
```C++
ulen += sizeof(struct udphdr);
```
4. 从msg->msg_name中获取目标地址
```C++
if (msg->msg_name) {
	struct sockaddr_in * usin = (struct sockaddr_in *)msg->msg_name;
	if (msg->msg_namelen < sizeof(*usin))
		return -EINVAL;
	if (usin->sin_family != AF_INET) {
		if (usin->sin_family != AF_UNSPEC)
			return -EAFNOSUPPORT;
	}

	daddr = usin->sin_addr.s_addr;
	dport = usin->sin_port;
```
5.获取路由表
```C++
ip_route_output_flow(net, fl4, sk);
```

6. 将所有数据粘为一个struct sk_buff
```C++
skb = ip_make_skb(sk, fl4, getfrag, msg->msg_iov, ulen,sizeof(struct udphdr), &ipc, &rt, msg->msg_flags);
```
7.发送数据
```C++
udp_send_skb()
```
