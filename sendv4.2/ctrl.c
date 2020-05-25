#include "ctrl.h"
#define MAXTIME 1024
int sockfd;
int pkt_ps[MAXTIME];
int pkt_tm;
//*getRequest()****************************/
char recvline[MAXLINE+1];
int getRequest(int argc,char *argv[],char *type)
{
	
	int re;
	struct sockaddr_in servaddr;
	struct hostent *hp;

	if (argc !=2)
	{
  		printf("Usage: %s The other host's name or ip\n",argv[0]);
  		exit(1);
	}
	hp=gethostbyname(argv[1]);
	if(hp==NULL)
  	{
    		printf("Gethostbyname error for %s",argv[1] );
    		exit(1);
  	}
	

	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Sock created failed!\n");
	}
	bzero(&servaddr,sizeof(servaddr));

	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
//	inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
	memcpy((char *)&servaddr.sin_addr,(char *)hp->h_addr,hp->h_length);
	printf("Trying to connect to: %s\n",(char *)inet_ntoa(servaddr.sin_addr));
	if(connect(sockfd,(SA *)&servaddr,sizeof(servaddr))<0)
	{
		perror("Connecttion failed!");
	}	
	char* sendline;	
	sendline=type;
	if((re=send(sockfd,sendline,strlen(sendline),0))>=0)
		printf("Request Mesage Sent: %s  socket_id: %d  Succeed!\n",sendline,re);

	waitforcmd(sockfd);

	return 1;

}

int waitforcmd(int sockfd)
{
	int n;	
	struct command *cmd;
	printf("Waiting for cmd..\n");
	while((n = recv(sockfd,recvline, MAXLINE,0)) < 0){  		
 	}	
	recvline[n]=0;	
	printf(" Recv:%d\n",n);
	
	
}
/***************************************/
struct command*  getCMD()
{
	struct command* rcm;
	struct command* cmd;
	cmd=(struct command*)malloc(sizeof(struct command));
	memset(&rcm,0,sizeof(rcm));
	memcpy(&rcm,&recvline,sizeof(rcm));
	rcm=(struct command *)recvline;
	cmd->att_ip=rcm->att_ip;
	cmd->port=rcm->port;
	cmd->threadnumber=rcm->threadnumber;
	cmd->type=rcm->type;
	cmd->rate=rcm->rate;
	cmd->pkt_size=rcm->pkt_size;
	cmd->starttime.sec=rcm->starttime.sec;
	cmd->starttime.min=rcm->starttime.min;
	cmd->starttime.hour=rcm->starttime.hour;
	cmd->starttime.day=rcm->starttime.day;
	cmd->starttime.mon=rcm->starttime.mon;
	cmd->starttime.year=rcm->starttime.year;

	cmd->endtime.sec=rcm->endtime.sec;
	cmd->endtime.min=rcm->endtime.min;
	cmd->endtime.hour=rcm->endtime.hour;
	cmd->endtime.day=rcm->endtime.day;
	cmd->endtime.mon=rcm->endtime.mon;
	cmd->endtime.year=rcm->endtime.year;
	return cmd;
}

void printCMD(struct command* cmd)
{
	printf("Recv CMD-> \n Att_IP:%d\n Port:%d\n ThreadNum:%d\n Type: %d\n Rate: %d\n Pkt_Size: %d\n StartTime: %d-%d-%d %d:%d:%d\n EndTime: %d-%d-%d %d:%d:%d\n",cmd->att_ip,cmd->port,cmd->threadnumber,cmd->type,cmd->rate,cmd->pkt_size,cmd->starttime.year,cmd->starttime.mon,cmd->starttime.day,cmd->starttime.hour,cmd->starttime.min,cmd->starttime.sec,
cmd->endtime.year,cmd->endtime.mon,cmd->endtime.day,cmd->endtime.hour,cmd->endtime.min,cmd->endtime.sec);
}

time_t switchtime(struct ptime tm)
{
	struct tm  tm1;
	tm1.tm_sec=tm.sec;
	tm1.tm_min=tm.min;
	tm1.tm_hour=tm.hour;
	tm1.tm_mday=tm.day;
	tm1.tm_mon=tm.mon-1;
	tm1.tm_year=tm.year-1900;
	tm1.tm_isdst=0;
	time_t ret=mktime(&tm1);
//	ret-=8*3600;
	return ret;
}
void send_to_ctrl()
{
	int re;pkt_ps[pkt_tm]=0;
	if((re=send(sockfd,pkt_ps,pkt_tm*4,0))>=0)
	printf("Result Mesage Sent to Ctrl Succesfully!\n");
//	exit(0);
}
