#include"gtk_callback.h"


int listenfd,connfd;

int  main(int agrc, char* argv[])
{
	struct sockaddr_in servaddr;	
	
	bzero(& servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);

	/****************************************************/
	/*Bind and listen*/
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("Sock--listenfd:");
	}
	if(bind(listenfd,(SA *)&servaddr,sizeof(servaddr))<0)
	{
		perror("Bind failed:");
	}
	
	
	/****************************************************/
	get_gtk_init(agrc,argv);
		
	return 0;
}

