#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdio.h>
#include<math.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<netdb.h>
#include<ctype.h>
#include<netinet/ip_icmp.h>
#include <netdb.h>

#define MAX_INTERFACES 16
#define CHLD_MAX  20
//int pid[CHLD_MAX];
/*
void flood_comm();
void udp_comm();

void tcp_comm();

void icmp_comm();

void on_pause();
void on_stop();*/
//void  ip_dialog();
//void  in_attack_dialog();

unsigned short ip_sum(unsigned short *,int);
unsigned short int cksum(unsigned short int* ,int);
void random_init(void);
inline long getrandom(int ,int);
unsigned long resolve(char *host);
int isip(char *ip);
unsigned long fakeip (int num);


unsigned short int udptcp_cksum(unsigned char* buf,int len);
inline unsigned long getrandomip (int iptype);


struct sa
{
 unsigned short int  fam,dp;
 unsigned int add;
 unsigned char zero[8];
};

struct su
{
 unsigned short int fam;
 char path[108];
};

struct ip_h
{
#if  _BTTE_ORDER==_LITTLE_ENDIAN
   unsigned char ihl:4,ver:4;
#else 
   unsigned char ver:4,ihl:4;
#endif
    unsigned char tos;
    unsigned short int tl,id,off;
    unsigned char ttl,pro;
    unsigned short int sum;
    unsigned int src,dst;  
};

struct icmp_h
{
 //这时在定义icmp的头部,
 unsigned char type,code; // 类型,代码
 unsigned short int sum; //16位校验和
 unsigned short int id,seq; // 这两个不知道是干什么的,比资料里多处来的
};

struct udp_h
{//定义udp的头部
unsigned short int src_port,dst_port,len,sum;
};

struct tcp_h
{
 unsigned short int src_port,dst_port;
 unsigned int seq_no,ack_no;
#if _BYTE_ORDER==_LITILE_ENDIAN
unsigned char x2:4,off:4;
#else
unsigned char off:4,x2:4;
#endif
unsigned char flag;
unsigned short int wnd_size,chk_sum,urgt_p;
#define FIN 0x01
#define SYN 0x02
#define RST 0x04
#define PUSH 0x08
#define ACK 0x10
#define URG 0x20
};

typedef struct{
 unsigned int  sourceip;    //源IP地址
 unsigned int  destip;      //目的IP地址
 unsigned char mbz;           //置空(0)
 unsigned char ptcl;          //协议类型
 unsigned short int plen;        //TCP/UDP数据包的长度(即从TCP/UDP报头算起到数据包结束的长度 单位:字节)
}Psd_Header;


