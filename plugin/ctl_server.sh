#! /bin/bash

ROOT_PATH=$(pwd)
LIB=$ROOT_PATH/lib/lib
BIN=$ROOT_PATH/httpd
CONF=$ROOT_PATH/conf/server.conf

id=''
proc=$(basename $0)
function usage(){
	printf "Usage %s [Start(-s) | stop(-t) | restart(-rt) | status(-ss)]\n" "$proc"
}

function server_status()
{
	#查看服务器状态 用pidof httpd可以查看当前进程pid
	server_bin=$(basename $BIN)
	id=$(pidof $server_bin)
	if [ ! -z "$id" ];then
		return 0	
	else
		return 1
	fi
}

function server_start()
{
	if ! server_status;then
		ip=$(awk -F: '/^IP/{print $NF}' $CONF)
		port=$(awk -F: '/^PORT/{print $NF}' $CONF)
		echo "start......"
		$BIN $ip $port
	else
		echo "server start $id"
	fi
}

function server_stop()
{
	if server_status;then
		kill -9 $id
	else
		echo "server not start $id"
	fi
}

function server_restart()
{
	if server_status;then
		server_stop
	fi
		server_start
}

function serverstatus_print()
{
	if server_status;then
		echo "staring:$id"
	else
		echo "not running $id"
	fi
}

if [ $# -eq 0 ];then
	usage
	exit 1
fi

if [ -z $LD_LIBRARY_PATH ];then
	export $LIB
fi

case $1 in
	start | -s)
		server_start
	;;
	stop | -t)
		server_stop
	;;
	restart | -rt)
		server_restart
	;;
	status | -ss)
		serverstatus_print
	;;
	*)
	usage;
	exit 1
	;;

esac
