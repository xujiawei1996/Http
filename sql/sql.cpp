#include"sql.h"
using namespace std;

//构造函数
sql::sql(const std::string &_user,\
		const std::string &_ip,\
		const std::string &_passwd,\
		const std::string &_db,\
		const int &_port)
:user(_user),ip(_ip),passwd(_passwd),db(_db),port(_port)
{
	conn = mysql_init(NULL);
}

int sql::connect()
{
	if(mysql_real_connect(conn,ip.c_str(),user.c_str(),passwd.c_str(),\
			db.c_str(),port,NULL,0)){
		cout<<"connect success!"<<endl;
		return 0;
	}else{
		return -1;
	}
}

int sql::insert(const std::string &name,const std::string &sex,\
				const std::string &school,const std::string &hobby)
{
	std::string sql = "INSERT INTO students_info (name,sex,school,hobby) values ('";
	sql+=name;
	sql+="','";
	sql+=sex;
	sql+="','";
	sql+=school;
	sql+="','";
	sql+=hobby;
	sql+="')";
	int ret = mysql_query(conn,sql.c_str());
	cout<<sql<<endl;
}

int sql::select()
{
	std::string sql="select * from students_info";
	int ret = mysql_query(conn,sql.c_str());
	if(ret == 0){
		MYSQL_RES *res = mysql_store_result(conn);
		if(res){
			int lines = mysql_num_rows(res);
			int cols = mysql_num_fields(res);
			std::cout<<"line"<<lines<<" cols"<<cols<<endl;
			MYSQL_FIELD *fd = NULL;
			for(;fd = mysql_fetch_field(res);)
			{
				cout<<fd->name<<" ";
			}
			cout<<endl;
			int i = 0;
			for(;i<lines;i++)
			{
				int j = 0;
				MYSQL_ROW row = mysql_fetch_row(res);
				for(;j<cols;j++){
						cout<<row[j]<<"  ";
				}
				cout<<endl;
			}
			
		}
		return 0;
	}
	return -1;
}

sql::~sql()
{
	mysql_close(conn);
}
