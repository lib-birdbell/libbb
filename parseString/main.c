#include <stdio.h>
#include <dlfcn.h>

#include "parseString.h"


int main(int argc, char** argv){
	char chGetData[] = {"PORT=12345\nIVY=00:01:02:03:04:05\ngreen=10:20:30:40:50:60\n"};
	struct macList *pMacList;

	printf("parseString test program.\n");

	pMacList = parseMacList(chGetData);

	showMacList(pMacList);

	deleteMacList(pMacList);

	printf("parseString test program end.\n");

	return 0;
}
