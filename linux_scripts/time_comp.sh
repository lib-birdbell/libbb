#!/bin/sh
# time compare script
# check after a certain period of time
CHECK_INTERVAL=600
CUR_TIME=`date +%s`
TEMP=`expr $CUR_TIME % $CHECK_INTERVAL`
LAST_TIME=`expr $CUR_TIME - $TEMP`
LOOP=1

while [ $LOOP != 0 ]
do
	CUR_TIME=`date +%s`
	if [ ${CUR_TIME} -gt `expr $LAST_TIME + $CHECK_INTERVAL` ];then
		echo "BASE TIME: "
		echo $LAST_TIME
		echo "CURRENT TIME: "
		echo $CUR_TIME
		echo "COMPARE TIME: "
		echo `expr $LAST_TIME + $CHECK_INTERVAL`
		LOOP=0
	fi
	sleep 1
done

