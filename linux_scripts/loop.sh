#!/system/bin/sh
while :
do
	read X
	if [ "$X" = "" ]
	then
		echo "TEST"
		./app_cash -n 9 -b 7 -d 00
		./app_cash -n 9 -b 7 -d ff
		./app_cash -n 9 -b 7 -d 00
	else
		echo "BREAK"
		break
	fi
done
