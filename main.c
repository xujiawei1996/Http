#include"http.h"

int main(int argc,char* argv[])
{
	if(argc<3)
	{
		printf("%s : [ip] [port]\n",argv[0]);
	}

	int listen_sock = startup(argv[1],atoi(argv[2]));
	if(listen_sock < 0){
		return 2;
	}

	//deamon(0,0);
	while(1)
	{
		struct sockaddr_in client;
		socklen_t len = sizeof(client);

		int new_sock = accept(listen_sock,(struct sockaddr*)&client,&len);
		if(new_sock<0){
			perror("new sock");
			continue;
		}
		
		//创建线程
		printf("connect success get ip port:%s %d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
		pthread_t tid;
		pthread_create(&tid,NULL,handler,(void*)new_sock);
		pthread_detach(tid);
	}
	return 0;
}
