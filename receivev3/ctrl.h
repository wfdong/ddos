#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define SA struct sockaddr
#define SERV_PORT 9877
#define LISTENNUM 1024
#define THREADS_MAXNUM LISTENNUM
#define CHLD_MAX  20
struct ptime
{
	int sec;//0~59
	int min;//0~59
	int hour;//0~23
	int day;//1~31
	int mon;//1~12
	int year;//>2000
};

struct command
{
	int att_ip;
	int port;//0~65535,-1:random default:-1
	int threadnumber;
	int type;//攻击类型：0:不发送 1：TCP  2:UDP  3:ICMP 4:CMD for Recv end
	int rate;//攻击速率(pps) 
	int pkt_size;//>0,-1:random; default:-1
	struct ptime starttime;//开始时间
	struct ptime endtime;//结束时间
};//检测项目命令信息


#define MAXLINE 4096
/*
typedef struct ClientMsg
{
	socklen_t sid;
	struct sockaddr_in cliaddr;
	int clilen;
};//Message from the attack client and receive client
*/
struct ReqFromCli
{
	char  MsgType[10];//请求类型：“att":发送端请求  "rev":接收端请求
	char  other[10];
};

void Re_Client(int sock);
void getcommand(int sock,struct ReqFromCli *requ,struct command *recmd,struct ptime* pstm,struct ptime* petm);
void settime(struct command* rcmd,struct ptime* stm,struct ptime* etm);
/*function for recv*/
int getRequest(int argc,char *argv[],char* type);
int waitforcmd(int sock);
struct command*  getCMD();
void printCMD(struct command* cmd);
char * get_exe_name();
void root_to_do();
int isip(char* ip_string);
void send_to_ctrl();
