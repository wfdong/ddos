 #include<stdio.h>
 #include<unistd.h>
 #include <stdlib.h>     
 #include <string.h>   
 #include <netdb.h>  
 #include <sys/types.h>    
 #include <netinet/in.h>   
 #include <sys/socket.h>    
 #include <unistd.h>   
 #include <sys/ioctl.h>  
 #include <arpa/inet.h>   
 #include <net/if_arp.h>     
 #include <net/if.h>   
 #include <arpa/inet.h> 
 #include<fcntl.h>
 #include"ip.h"

char rseed[65535];
int rcounter=0;
unsigned long myip = 2130706433;	/* 127.0.0.1 network byte ordered */
#define DEFAULT_ETH "eth1"


/* -------------------------------------------------------------------- */
char * get_exe_name()
{
	static char buf[1200];
	int result=readlink("/proc/self/exe",buf,1200);
	if(result<0||result>=1200)
		return NULL;
	buf[result]='\0';
	return buf;
}


void root_to_do()
{
	char *path;
	char su_comm[1200];
	path=get_exe_name();
	sprintf(su_comm,"gksudo %s",path);
	execlp("gksudo","gksudo",path,(char *)0);
 }




/* -------------------------------------------------------------------- */


unsigned long resolve(char *host)//马磊新增加的
{
	struct hostent *he;//这个hostent第一次见到的
	struct sa temp;
	he=gethostbyname(host);
	if(he)
	{//caddr_t是个什么类型的？？？？？？？？
		memcpy((caddr_t)&temp.add,he->h_addr,he->h_length);
	}
	else
		return 0;
	return (temp.add);
}



unsigned short ip_sum(addr ,len)
    unsigned short *addr;
    int len;
 {
	register int nleft=len;
	register unsigned short *w=addr;
	register int sum=0;
	unsigned short answer=0;
	int other;
	while(nleft>1)
	{
		sum+=*w++;
			nleft-=2;
	}
	if(nleft==1)
	{
		// *(unsigned char *)(&answer)=*(unsigned char *)w;//马磊的这里没有这一句
		// sum+=answer;
		sum+= (*(unsigned char *) w)<<8;
	}
	sum=(sum>>16)+(sum&0xffff);
	sum +=(sum>>16);
	answer=~sum;
	return (answer);
}


/* -------------------------------------------------------------------- */
 unsigned short int cksum(unsigned short int *buf,int nwords)
{
   unsigned long sum;
   for (sum=0;nwords>0;nwords--)
  	 sum+=*buf++;
   sum=(sum>>16)+(sum&0xffff);
   sum+=(sum>>16);
   return ~sum;
}

/* -------------------------------------------------------------------- */
unsigned short int udptcp_cksum(unsigned char* buf,int len)
{
	int sum;
	struct ip_h *ih=(struct ip_h*)buf;
	Psd_Header *psd_header;
	if(len%2==0)
		psd_header=(Psd_Header *)(buf+len);
	else
	{
		psd_header=(Psd_Header *)(buf+len+1);
	//	*(buf+len)=*(buf+len-1);//??原来是+1的 chang
	//	*(buf+len-1)=0;               
	//此处以前翻过资料 貌似填充的零放在高位，如今又放到低位了
		*(buf+len)=0;
	}
	len-=sizeof(struct ip);
	psd_header->destip=ih->dst;
	psd_header->sourceip=ih->src;
	psd_header->mbz=0;

	if(ih->pro==6)//定义宏TCP??
		psd_header->ptcl=0x06;
	else
		psd_header->ptcl=0x11;
	psd_header->plen=htons(len);
	
	if(len%2==0)
		return ip_sum((unsigned short int*)(buf+sizeof(struct ip_h)),len+12);
	else
		return ip_sum((unsigned short int*)(buf+sizeof(struct ip_h)),len+13);
	
}


/* -------------------------------------------------------------------- */

//产生随机数的部分
void random_init(void)
{
 	int rfd=open("/dev/urandom",O_RDONLY);
	if(rfd<0)
		rfd=open("/dev/random",O_RDONLY);
	rcounter=read(rfd,rseed,65535);
	close(rfd);
}

/* -------------------------------------------------------------------- */

inline long getrandom(int min ,int max)
{
	if (rcounter<2)
		random_init();
	srand(rseed[rcounter]+(rseed[rcounter-1]<<8));
	rcounter-=2;
	return ((random()%(int)(((max)+1)-(min)))+(min));
}


/* -------------------------------------------------------------------- */
inline unsigned long getrandomip (int iptype)
{
	struct in_addr hax0r;
	char convi[16];
	int a, b, c, d;

	if (iptype < 1)
		return (unsigned long) (getrandom (0, 65535) + (getrandom (0, 65535) << 16));

	hax0r.s_addr = htonl (myip);

	sscanf ((char *)inet_ntoa (hax0r), "%d.%d.%d.%d", &a, &b, &c, &d);//明白了
	if (iptype < 2)
		b = getrandom (1, 254);
	if (iptype < 3)
		c = getrandom (1, 254);
	d = getrandom (1, 254);

	sprintf (convi, "%d.%d.%d.%d", a, b, c, d);

	return inet_addr (convi);
}

 unsigned long fakeip (int num)
{
	
	struct in_addr hax0r;
	char convi[16];
	int a, b, c, d;
	char local_ip[16];
	if (num ==3)//全部是伪造
		return (unsigned long) (getrandom (0, 65535) + (getrandom (0, 65535) << 8));
	getlocalip(local_ip);
	sscanf(local_ip,"%d.%d.%d.%d",&a,&b,&c,&d);
	if (num ==2)
	{
		b = getrandom (1, 254);
		c = getrandom (1, 254);
	}
	if (num==1)
		c = getrandom (1, 254);
	
	d = getrandom (1, 254);

	sprintf (convi, "%d.%d.%d.%d", a, b, c, d);

	return inet_addr (convi);
}

 int getlocalip(char *ip) //获取默认的ip地址
{   
    int sockfd;   
    if(-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0)))  
     {  
         perror( "socket" );  
         return -1;    
    }  
  
    struct ifreq req;  
    struct sockaddr_in *host;  
    bzero(&req, sizeof(struct ifreq));  
    strcpy(req.ifr_name, DEFAULT_ETH);  
    ioctl(sockfd, SIOCGIFADDR, &req);  
    host = (struct sockaddr_in*)&req.ifr_addr;  
    strcpy(ip,(char *)inet_ntoa(host->sin_addr));  
    close(sockfd);  
   return 1;  
  
} 

