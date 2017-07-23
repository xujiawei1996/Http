#include "http.h"

static void show_404(int fd)
{
	const char* echo_header="HTTP/1.0 404 NOT FOUND\r\n";
	send(fd,echo_header,strlen(echo_header),0);
	const char* type = "Content-Type:text/html;charset=ISO-8859-1\r\n";
	send(fd,type,strlen(type),0);
	const char* blank_line = "\r\n";
	send(fd,blank_line,strlen(blank_line),0);

	const char* msg = "<html><h1>Page Not Found!</h1></html>\r\n";
	send(fd,msg,strlen(msg),0);
}

void echo_error(int fd,int error_num)
{
	switch(error_num){
		case 404:
			show_404(fd);
			break;
		case 400:
			break;
		case 401:
			break;
		case 501:
			break;
		case 403:
			break;
		case 500:
			break;
		case 200:
			break;
		deault:
			break;
	}
}

int startup(char* ip,int port)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("sock");
		return -2;
	}

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr(ip);
	local.sin_port = htons(port);
	
	int flg = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&flg,sizeof(flg));

	if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0){
		perror("bind");
		return -3;
	}

	if(listen(sock,10) < 0){
		perror("listen");
		return -4;
	}
	return sock;
}

//获取一行 从http中按行读取
static int get_line(int fd,char* buf,int len)
{
	//初始化c 不能让c为开始的随机值
	char c = '\0';
	int i = 0;
	while(c != '\n' && i < len - 1){
		//窥探 只把数据取出来看一下 不动数据
		//因为平台不同 有些时候的换行为\r\n 有些为\r 
		//要把这些都换成\n
		//这里recv最后一个参数MSG——PEEK表示窥探！！！
		ssize_t s = recv(fd,&c,1,0);
		if(s > 0){
			if(c == '\r'){
				recv(fd,&c,1,MSG_PEEK);
				if(c == '\n'){
					recv(fd,&c,1,0);
				}else{
					c='\n';
				}
			}//当走到这一步的时候 c要么为字符要么为\n
			buf[i++] = c;
		}
	}
	buf[i] = 0;
	return i;
}

//打印日志消息，这里简单实现只是打印错误信息及等级
void print_log(const char* msg,int level)
{
	const char* level_msg[]={
		"NOTICE",
		"WARNING",
		"FATAL",
	};
	printf("[%s][%s]\n",msg,level_msg[level]);
}

//向网页上打印东西
int echo_www(int fd,const char* path,int size)
{
	int new_fd = open(path,O_RDONLY);
	if(new_fd < 0){
		print_log("open file error!",FATAL);
		return 404;
	}
	const char* echo_line = "HTTP/1.0 200 OK\r\n";
	send(fd,echo_line,strlen(echo_line),0);
	const char* blank_line = "\r\n";
	send(fd,blank_line,strlen(blank_line),0);

	if(sendfile(fd,new_fd,NULL,size) < 0){
		print_log("send_file error",FATAL);
		return 200;
	}
	close(new_fd);
}

//读http空行之前的信息 读完头
void drop_header(int fd)
{
	char buff[SIZE];
	int ret = -1;
	do
	{
		ret = get_line(fd,buff,sizeof(buff));
	}while(ret > 0 && strcmp(buff,"\n"));
}

int exe_cgi(int fd,const char*method,\
			const char* path,const char* query_string)
{

		//CONTENT_LENGTH：POST方法输入的数据的字节数。
		int content_len = -1;
		char METHOD[SIZE/10];
		char QUERY_STRING[SIZE];
		char CONTENT_LENGTH[SIZE];
		//先判断是get方法还是post方法
		//这里strcasecmp指的是不强调大小写的比较
		if(strcasecmp(method,"GET") == 0){
				//如果时get方法，则读取url
				drop_header(fd);
		}else{
				//post方法就要读正文了
				char buff[SIZE];
				int ret = -1;
				do{
					ret = get_line(fd,buff,sizeof(buff));
					//这里Content-Length （注意空格）有16个字节
					if(strncasecmp(buff,"Content-Length: ",16) == 0)
							content_len = atoi(&buff[16]);
				}while(ret > 0&&strcmp(buff,"\n"));
				//这里说明没有正文 那就不应该调用cgi 返回错误信息
				if(content_len == -1)
				{
					echo_error(fd,401);
					return -1;
				}
		}
		//当走到这一步，说明一定为cgi调用
		printf("cgi:path:%s\n",path);
		//这里前端把数据给后端cgi，由cgi fork子进程去处理数据，有父进程与客户端实现数据交互
		//但是给cgi的数据怎么给子进程，所以这里用到了进程间通信，由于是父子进程，所以用管道比较方便。
		int input[2];
		int output[2];
		if(pipe(input) < 0){
			echo_error(fd,401);
			return -2;
		}
		if(pipe(output) < 0){
			echo_error(fd,401);
			return -3;
		}

		const char* echo_line="HTTP/1.0 200 OK\r\n";
		send(fd,echo_line,strlen(echo_line),0);
		const char* type = "Content-Type:text/html;charset=ISO-8859-1\r\n";
		send(fd,type,strlen(type),0);
		const char* blank_line="\r\n";
		send(fd,blank_line,strlen(blank_line),0);

		pid_t id = fork();
		if(id<0){
			echo_error(fd,501);
			return -2;
		}else if(id == 0){//子进程
			//子进程获取数据
			close(input[1]);
			close(output[0]);
			sprintf(METHOD,"METHOD=%s",method);

			putenv(METHOD);
			//如果是GET方法，数据被存到了QUERY_STRING的环境变量中
			if(strcasecmp(method,"GET") == 0){
				sprintf(QUERY_STRING,"QUERY_STRING=%s",query_string);
				putenv(QUERY_STRING);
			}else{
				//如果是POST方法，传正文的时候并不知道到底什么时候传完，所以这里有一个环境变量CONTENET_LENGTH。记录了传输过来有多少个字节长度的字符。
				sprintf(CONTENT_LENGTH,"CONTENT_LENGTH=%d",content_len);
				putenv(CONTENT_LENGTH);
			}
			//这里使用重定向，将输出输入都变为以客户端的方式读写
			dup2(input[0],0);
			dup2(output[1],1);
			//程序替换，替换为path路径下的程序
			execl(path,path,NULL);
			exit(1);
		}else{//父进程
			//从管道里取数据，再向客户端发送数据
			close(input[0]);
			close(output[1]);
			int i = 0;
			char c = '\0';
			for(;i<content_len;i++){
				recv(fd,&c,1,0);
				write(input[1],&c,1);
			}
			while(1){
				ssize_t s = read(output[0],&c,1);
				if(s > 0){
					send(fd,&c,1,0);
				}else{
					break;
				}
			}
			waitpid(id,NULL,0);
			close(input[1]);
			close(output[0]);
		}
}


void *handler(void * arg)
{
	int fd = (int)arg;
	int error_num = 200;
	int cgi = 0;
	char *query_string = NULL;
#ifdef _DEBUG_
	//这里是测试是否正确的代码 在编译时加入定义宏debug
	//就可测试看以前的代码是否正确
	printf("#######################################################\n");
	char buff[SIZE];
	int ret = -1;
	
	while(ret < 0 && strcmp(buff,"\n") != 0)
	{
		ret = get_line(fd,buff,sizeof(buff));
		printf("%s",buff);
	}

	printf("#######################################################\n");
#else
	//否则没有定义
	//这里分别提取方法method，url，字符串buf
	char method[SIZE/10];
	char url[SIZE];
	char path[SIZE];
	char buf[SIZE];
	//len表示总长度，i表示某一个单位的长度
	int len,i;
	if(get_line(fd,buf,sizeof(buf)) <= 0){
		//printf_log("get request line error",FATAL);
		goto end;
	}

	//len为总厂 i为一个的长度
	i=0;len=0;
	while( i < sizeof(method)-1 && len<sizeof(buf) &&\
			!isspace(buf[len])){
		method[i] = buf[len];
		len++;
		i++;
	}
	method[len] = 0;

	//这里是跳过所有空格 万一在GET 或者 POST后有多个空格 则要跳过所有空格
	while( isspace(buf[len]) && len < sizeof(buf))
	{
			len++;
	}

	i=0;
	while( i <sizeof(url) && len<sizeof(buf) &&\
			!isspace(buf[len])){
		url[i] = buf[len];
		len++;
		i++;
	}
	url[len] = 0;


	printf("method:%s, url:%s\n",method,url);
	if(strcasecmp(method,"GET") && strcasecmp(method,"POST")){
		print_log("method is not ok!",FATAL);
		error_num = 501;
		goto end;
	}
	
	if(strcasecmp(method,"POST") == 0){
		cgi = 1;
	}

	query_string = url;
	//找url并且找到参数 ?后为参数
	while(*query_string != 0){
		if(*query_string == '?'){
			//设为以cgi模式运行
			cgi = 1;
			*query_string = '\0';
			query_string++;
			break;
		}
		query_string++;
	}

	//资源写到path里
	sprintf(path,"wwwroot%s",url);
	if(path[strlen(path)-1] == '/'){
		strcat(path,"index.html");
	}

	printf("path:%s\n",path);

	//验证目录是否存在 stat 第一个参数为路径
	struct stat st;
	if(stat(path,&st) < 0){
		//资源不存在 
		print_log("path not found!",FATAL);
		error_num = 404;
		goto end;
	}else{
		//文件存在 
		//如果是一个目录
		if(S_ISDIR(st.st_mode)){
			strcat(path,"/index.html");
		}else{
			//看他的权限，如果时可执行程序
			if((st.st_mode & S_IXUSR) ||\
				(st.st_mode & S_IXGRP) ||\
				(st.st_mode & S_IXOTH)){
				cgi = 1;
			}
		}
	
		if(cgi){
			exe_cgi(fd,method,path,query_string);
		}else{
			drop_header(fd);
			error_num =  echo_www(fd,path,st.st_size);
		}
	}

#endif
	end:
		echo_error(fd,error_num);
		close(fd);
}



















