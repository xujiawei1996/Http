cc=g++
ROOT_PATH=$(shell pwd)
INCLUDE=$(ROOT_PATH)/lib/include
LDFLAGS=-lmysqlclient
LIB=$(ROOT_PATH)/lib/lib

.PHONY:all
all:insert_cgi select_cgi

#不仅要在默认路径下找库，还要在指定目录下找库
insert_cgi:insert_cgi.o  sql.o
	@$(cc) -o $@ $^ -L$(LIB) $(LDFLAGS)
select_cgi:select_cgi.o sql.o
	@$(cc) -o $@ $^ -L$(LIB) $(LDFLAGS)

%.o:%.cpp
#-I 找头文件时除了在系统目录 当前目录 还在制定目录下查找
	@$(cc) -c $< -I$(INCLUDE)
	@echo "[Compling] [$^] to [$@] ...done"

.PHONY:clean
clean:
	@rm -f *.o insert_cgi select_cgi
	@echo "clean insert_cgi&&select_cgi ...done"
