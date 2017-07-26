bin=httpd
cc=gcc
obj=http.o main.o
pth=-lpthread
deb=-D _DEBUG_
CGI_PATH=sql wwwroot/cgi-bin

.PHONY:all
all:$(bin) cgi

$(bin):$(obj)
	@$(cc) -o $@ $^ $(pth)	#$(deb)
	@echo "[Linking] [$^] to [$@] ...done"
%.o:%.c
	@$(cc) -c $<
	@echo "[Compling] [$^] to [$@] ...done"
cgi:
	@for i in `echo $(CGI_PATH)`;\
	do\
			cd $$i;\
			make;\
			cd -;\
	done

.PHONY:clean
clean:
	@rm -rf *.o $(bin) output
	@for i in `echo $(CGI_PATH)`;\
	do\
			cd $$i;\
			make clean;\
			cd -;\
	done
	@echo "clean project .....done"

.PHONY:output
output:
	@mkdir -p output/wwwroot/cgi-bin
	@cp -rf log output
	@cp -rf conf output
	@cp wwwroot/index.html output/wwwroot
	@cp wwwroot/cgi-bin/math_cgi output/wwwroot/cgi-bin
	@cp sql/insert_cgi output/wwwroot/cgi-bin
	@cp sql/select_cgi output/wwwroot/cgi-bin
	@cp plugin/ctl_server.sh output/
	@cp httpd output
	@cp -rf sql/lib output
	@echo "output"
