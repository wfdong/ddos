//#include "ctrl.h"
#include "gtk_callback.h"

void Re_Client(int sock)
{
	char buf[MAXLINE];
	char info[MAXLINE];
	struct ReqFromCli req;
	struct command cmd;
	struct ptime *stm=NULL,*etm=NULL;
	ssize_t bufn=recv(sock,buf,MAXLINE,0);
	if(bufn<0)
	{
		perror("Recv error:");
		return;
	}
	memset(&req.MsgType,0,sizeof(req.MsgType));
	memcpy(req.MsgType,buf,bufn);
	
	printf("----ReqType: %s\n",req.MsgType);
	sprintf(info,"----ReqType: %s\n",req.MsgType);
	log_store("Recv_log.txt",info);
	int sign=0;
	if(memcmp(req.MsgType,"att",3)==0)
	{		
	//	g_share->send_connect_sock=sock;
		sign=0;
		if(g_share->send_first_re==1)
		{
			g_share->send_first_re=0;
			
		}		
	}
	else if(memcmp(req.MsgType,"rev",3)==0)
	{
		sign=1;
	//	g_share->rev_connect_sock=sock;
		if(g_share->rev_first_re==1){
	//		g_share->g_cmd.type=0;
			g_share->rev_first_re=0;		
		}		
	}
	else
	{
		printf("Error in Re_Client!\n");
		return ;
	}
	if(g_share->rev_first_re) printf("Recv end Not Connected! Please wait for the recv request.\n");
	if(g_share->send_first_re) printf("Att end Not Connected! Please wait for the att request.\n");
	if(sign==0)
	{
		if((send(sock,&g_share->g_cmd,sizeof(g_share->g_cmd),0))<0)
		{
			perror("Send to att error!");
		}
		sprintCMD(&g_share->g_cmd,info);
		log_store("Recv_log.txt",info);
		Recv_attresult(sock);
	}
	else 
	{
		if((send(sock,&g_share->g_cmd,sizeof(g_share->g_cmd),0))<0)
		{
			perror("Send to recv error!");
		}
		sprintCMD(&g_share->g_cmd,info);
		log_store("Recv_log.txt",info);			
			Recv_result(sock);
	}
	
//	close(g_share->send_connect_sock);
//	close(g_share->rev_connect_sock);
}

void Recv_result(int sock)
{
	char rev_buf[MAXLINE];
	int *intbuf;char *loginfo;
	ssize_t rev_bufn;
	memset(rev_buf,'\0',sizeof(rev_buf));
	while((rev_bufn=recv(sock,rev_buf,MAXLINE,0))>0)
	{
	//	printf("lenth=%d\n",rev_bufn);
	//	memcpy(&intbuf,&rev_buf,1024);太错了
		intbuf=(int *)rev_buf;
		sprintInfo(rev_bufn,intbuf,loginfo);	
	}
	close(sock);
			
}
void Recv_attresult(int sock)
{
/*	char rev_buf[MAXLINE];
	int *intbuf;
	char *loginfo;//
	ssize_t rev_bufn;
	memset(&rev_buf,'\0',sizeof(rev_buf));
//	printf("sizeof(intbuf)1: %d",sizeof(intbuf));
	memset(&intbuf,0,1024);				太错了
	*/
	char rev_buf[MAXLINE];
	int *intbuf;char *loginfo;
	ssize_t rev_bufn;
	memset(rev_buf,'\0',sizeof(rev_buf));
	
	while((rev_bufn=recv(sock,rev_buf,MAXLINE,0))>0)
	{
	//	memcpy(intbuf,rev_buf,1024);//?
		intbuf=(int *)rev_buf;
		printf("first%d\n",intbuf[0]);//第一个字节不见了
		attsprintInfo(rev_bufn,intbuf,loginfo);
	}
	close(sock);
			
}
void attsprintInfo(int lenth,int *re,char * info)//info没用到?
{	
	int i=0;
	char temp[100];
	memset(temp,'\0',sizeof(temp));
	int ll=lenth/sizeof(int);
	printf("ret %d bytes\n",lenth);
	for(;i<ll;i++)
	{
		printf("%dSENDNUM: %d\n",i,re[i]);
		sprintf(temp,"SENDNUM: Packets Amount: %d\n",re[i]);
		log_store("Recv_log.txt",temp);
	}
	Calcu_Recv(re);	
	
}
void sprintInfo(int lenth,int *re,char * info)
{	
	int i=0;
	char temp[100];
	memset(temp,'\0',sizeof(temp));
	printf("ret %d bytes\n",lenth);
	int ll=lenth/sizeof(int);
	for(;i<ll;i++)
	{
		printf("%dREVNUM: %d\n",i,re[i]);
		sprintf(temp,"REVNUM: Packets Amount: %d\n",re[i]);
		log_store("Recv_log.txt",temp);
	}
	Calcu_Recv(re);	
	
}

void Calcu_Recv(int *re)
{
	/*Add code here to calu*/
}

void sprintCMD(struct command *cmd,char *sg)
{
	char *s;
	s=sg;
	sprintf(s,"Recv CMD-> \nAtt_IP:%d\nPort:%d\nThreadNum:%d\nType: %d\nRate: %d\nPkt_Szie:%d\nStartTime: %d-%d-%d %d:%d:%d\nEndTime: %d-%d-%d %d:%d:%d\n",cmd->att_ip,cmd->port,cmd->threadnumber,cmd->type,cmd->rate,cmd->pkt_size,cmd->starttime.year,cmd->starttime.mon,cmd->starttime.day,cmd->starttime.hour,
cmd->starttime.min,cmd->starttime.sec,
cmd->endtime.year,cmd->endtime.mon,cmd->endtime.day,cmd->endtime.hour,cmd->endtime.min,cmd->endtime.sec);	
}

void printCMD(struct command* cmd)
{
	printf("Recv CMD-> \n Att_IP:%d\n Port:%d\n ThreadNum:%d\n Type: %d\n Rate: %d\n Pkt_Size: %d\n StartTime: %d-%d-%d %d:%d:%d\n EndTime: %d-%d-%d %d:%d:%d\n",cmd->att_ip,cmd->port,cmd->threadnumber,cmd->type,cmd->rate,cmd->pkt_size,cmd->starttime.year,cmd->starttime.mon,cmd->starttime.day,cmd->starttime.hour,cmd->starttime.min,cmd->starttime.sec,
cmd->endtime.year,cmd->endtime.mon,cmd->endtime.day,cmd->endtime.hour,cmd->endtime.min,cmd->endtime.sec);
}

void set_default_share(struct g_pipe *scmd)
{
	struct g_pipe *g_share;
	g_share=scmd;

	g_share->send_first_re=1;
	g_share->rev_first_re=1;
	g_share->send_connect_sock=0;
        g_share->rev_connect_sock=0;
	g_share->g_cmd.att_ip=(int)inet_addr("127.0.0.1"); //16777343;
	g_share->g_cmd.type=0;	
	g_share->g_cmd.port=-1;
	g_share->g_cmd.threadnumber=4;
	g_share->g_cmd.rate=10;

	g_share->g_cmd.starttime.sec=0;
	g_share->g_cmd.starttime.min=0;
	g_share->g_cmd.starttime.hour=0;
	g_share->g_cmd.starttime.day=0;
	g_share->g_cmd.starttime.mon=0;
	g_share->g_cmd.starttime.year=2010;

	g_share->g_cmd.endtime.sec=0;
	g_share->g_cmd.endtime.min=0;
	g_share->g_cmd.endtime.hour=0;
	g_share->g_cmd.endtime.day=0;
	g_share->g_cmd.endtime.mon=0;
	g_share->g_cmd.endtime.year=2010;
}


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
int isip(char* ip_string)
{
	int a,b,c,d;
	sscanf(ip_string,"%d.%d.%d.%d",&a,&b,&c,&d);

	if(a<0||b<0||c<0||d<0)
	{
		//dialog_information("ip <0");
		return 0;
	}
	if(a>255||b>255||c>255||d>255)
	{
		//dialog_information("ip>255");
		return 0;
	}


	return 1;
	
}


