#!/bin/sh
echo "Multi"
BASE=$1
if [ -z $BASE ];then
	echo "Please enter the number for multiple"
else
	for i in 1 2 3 4 5 6 7 8 9
	do
		RESULT=`expr $1 \* $i`
		echo "$1 x $i = $RESULT"
	done
fi
echo "Multi end"
