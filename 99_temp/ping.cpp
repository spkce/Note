#include "unp.h"
#include "netinit/in_systm.h"
#include "netinit/ip.h"
#include "netinit/ip_icmp.h"

#define BUFSIZE 1500;
char sendbuf[BUFSIZE];
int datalen;
char *host;
int nsent;
pid_t pid;
int sockfd;
int verbose;

struct porto {
	void (*fproc)(char*, ssize_t, struct msghdr*, struct timeval*)
	void (*fsend)(void);
	void (*finit)(void);
	struct sockaddr *sasend;
	struct sockaddr *saracv;
	socklen_t salen;
	int icmpproto;
}

void proc_v4(char *ptr, ssize_t len, struct msghdr* msg, struct timeval *tvrecv);
void tv_sub(struct timeval * out, struct timeval *in);
void readloop();
void send_v4(void);
uint16_t in_cksum(uint16_t * addr, int len);

struct porto proto_v4 =
{proc_v4,send_v4, NULL,NULL,NULL,0,IPPROTO_ICMP};

int main(int argc, char const *argv[])
{
	int c;
	struct addrinfo* ai;
	char* h;

	opterr = 0;
	while((c = getopt(argc, argv, "v")) != -1)
	{
		switch (c)
		{
		case 'v':
			verbose++;
			break;
		case '?':
			err_quit("unrecognized option:%c",c);
		}
	}

	if (optind != argc -1)
	{
		err_quit("usage:ping [-v] <hostname>");
	}

	host = argv[optind];

	pid = getid() & 0xFFFF;
	Signal(SIGALM, sig_alrm);

	ai = Host_serv(host, NULL, 0, 0);
	h = Sock_ntop_host(ai->ai_addr, ai->ai->ai_addr_len);
	if (ai->ai_family == AF_INET)
	{
		pr = &proto_v4;
	}
	else
	{
		err_quit("unknow address family:%d", ai->ai_family);
	}
	return 0;

	pr->sasend = ai->ai->ai_addr;
	pr->sarecv = Calloc(1, ai->ai_addr_len);
	pr->salen = ai->ai_addr_len;

	readloop();
	exit(0);
}

void readloop()
{
	int size;
	char recvbuf[];
	char controlbuf[];
	struct msghdr msg;
	struct iovec iov;
	ssize_t n;
	struct timeval tval;

	sockfd = Socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
	setuid(getuid());
	if (pr->finit())
	{
		(*pr->finit)();
	}
	size = 60 * 1024;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size))

	sig_alrm(SIGALM);

	iov.iov_base = recvbuf;
	iov.iov_len = sizeof(recvbuf);
	msg.msg_name = pr->sarecv;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = controlbuf;

	for (;;)
	{
		msg.msg_namelen = pr->salen;
		msg.msg_controllen = sizeof(controlbuf);

		n = recvmsg(sockfd, &msg, 0);
		if (n < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				err_sys("*recvmsg error");
			}
		}
		Gettimeofday(&tval, NULL);
		(*pr->fproc)(recvbuf, n, &msg, &tval);
	}
}

void tv_sub(struct timeval * out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0) 
	{
		--out->tv_usec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

void proc_v4(char *ptr, ssize_t len, struct msghdr* msg, struct timeval *tvrecv)
{
	int hlen, icmplen;
	double rtt;
	struct ip *ip;
	struct icmp *icmp;
	struct timeval *tvsend;

	ip = (struct ip*) ptr;
	hlen = ip->ip_hl << 2;

	if (ip->ip_p != IPPROTO_ICMP)
	{
		return ;
	}

	icmp = (struct icmp*) (ptr + hlen);
	if ((icmplen = len - hlen) < 8)
	{
		return;
	}

	if (icmp->icmp_type == ICMP_ECHOREPLY)
	{
		if (icmp->icmp_id != pid)
		{
			return;
		}
		if (icmplen < 16)
		{
			return;
		}

		tvsend = (struct timeval*)icmp->icmp_date;
		tv_sub(tvrecv,tvsend);
		rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;
		printf("	%d byte form %s: seq=%u ttl=%d rtt=%.3f ms\n",
			icmplen, Sock_ntop_host(ptr->sarecv, pr_salen), icmp->icmp_seq, ip->ip_ttl, rtt);
	}
	else if (verbose)
	{
		printf("	%d byte form %s: type=%u code=%d\n",
		icmplen, Sock_ntop_host(ptr->sarecv, pr_salen), icmp->icmp_type, icmp->icmp_code);
	}
}


void send_v4(void)
{
	int len;
	struct icmp * icmp;
	icmp = (struct icmp*)sendbuf;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = pid;
	icmp->icmp_seq = nsent++;
	memset(icmp->icmp_date, 0xa5, datalen);
	Gettimeofday((struct timeval*)icmp->icmp_date,NULL);

	len = 8 + datalen;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = incksum((u_short*)icmp, len);

	sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
}


uint16_t in_cksum(uint16_t * addr, int len);
{
	int nleft = len;
	uint32_t sum = 0;
	uint16_t *w = addr;
	uint16_t answer = 0;

	while(nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1)
	{
		*(unsigned char*)(&answer) = *(unsigned char*)w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = -sum;
	return answer;
}
