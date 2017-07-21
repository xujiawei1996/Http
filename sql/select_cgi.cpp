#include"sql.h"
void select_data()
{
	sql obj("root","127.0.0.1","","student_info",3306);
	obj.connect();
	std::cout<<std::endl;	
	obj.select();
}

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
			return 1;
		}
		strcpy(buff,arg_string);
	}else if(method && strcasecmp(method,"POST") == 0){
		content_len = getenv("CONTENT_LENGTH");
		if(!content_len){
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
		return 1;
	}

	select_data();

	return 0;
}
