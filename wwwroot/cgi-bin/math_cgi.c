#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main()
{
	char *method = NULL;
	char *arg_string = NULL;
	char *content_len = NULL;
	char buff[1024];
	method=getenv("METHOD");
	if(method && strcasecmp(method,"GET") == 0){
		arg_string = getenv("QUERY_STRING");
		if(!arg_string){
			printf("get method get arg error!\n");
			return 1;
		}
		strcpy(buff,arg_string);
	}else if(method && strcasecmp(method,"POST") == 0){
		content_len = getenv("CONTENT_LENGTH");
		if(!content_len){
			printf("get method POST content_length error!\n");
			return 2;
		}
		int i = 0;
		char c = 0;
		int nums = atoi(content_len);
		for(;i<nums;i++){
			read(0,&c,1);
			buff[i] = c;
		}
		buff[i] = '\0';
	}else
	{
		printf("get method error!\n");
		return 1;
	}

	//这里可以调用自己写的函数
	
	return 0;
}
