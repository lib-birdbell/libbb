#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "parseString.h"


#define	MAC_SIZE	6
#define	MAC_NAME_SIZE	64


//#define	DEBUG_ENABLE	1


static unsigned char hexToInt(char* pHex){
	char chHex[5] = {'0', 'x', 0, 0, 0};
	int value;

	chHex[2] = pHex[0];
	chHex[3] = pHex[1];

	sscanf(chHex, "%x", &value);

	return (unsigned char)(value&0xFF);
}


static int leftCharCount(char* pHex){
	int i = 0;;

	while(*(pHex+i)!='\0' && *(pHex+i)!='\r' && *(pHex+i)!='\n'){
#ifdef	DEBUG_ENABLE
		printf("[%c:%x]", *(pHex+i), *(pHex+i));
#endif
		i++;
	}

	return i;
}


struct macList *parseMacList(char* chText){
	struct macList *pMacList;
	int nLineCount = 0, nTextCount = 0;
	int i;
	int nCharCount = 0, nNameNum = 0;
	char chTemp[MAC_NAME_SIZE] = {0};

	pMacList = (struct macList*)malloc(sizeof(struct macList));
	if(!pMacList){
		return 0;
	}

	while(*(chText+nTextCount)){
		if(*(chText+nTextCount) == '\r' || *(chText+nTextCount) == '\n'){
			nLineCount++;
		}
		nTextCount++;
	}
#ifdef	DEBUG_ENABLE
	printf("nLineCount=%d\n", nLineCount);
#endif

	pMacList->pMacs = (unsigned char*)malloc(sizeof(unsigned char) * nLineCount * MAC_SIZE);
	pMacList->pNames = (char*)malloc(sizeof(char) * nLineCount * MAC_NAME_SIZE);

	memset(pMacList->pMacs, 0, sizeof(unsigned char) * nLineCount * MAC_SIZE);
	memset(pMacList->pNames, 0, sizeof(char) * nLineCount * MAC_NAME_SIZE);

	nCharCount = 0;
	nNameNum = 0;
	memset(chTemp, 0, sizeof(char) * MAC_NAME_SIZE);
	for(i=0;i<nTextCount;i++){
		if(chText[i] == '='){	// Next
			chTemp[nCharCount] = '\0';
#ifdef	DEBUG_ENABLE
			printf("chTemp=%s\n", chTemp);
#endif
			if(strcmp(chTemp, "PORT") == 0 || strcmp(chTemp, "port") == 0){
				pMacList->nPort = atoi(&chText[i+1]);
#ifdef	DEBUG_ENABLE
				printf("PORT=%d\n", pMacList->nPort);////debug
#endif
				nCharCount=0;
				i += leftCharCount(&chText[i]);
				continue;
			}

			strcpy(&pMacList->pNames[MAC_NAME_SIZE*nNameNum], chTemp);
			pMacList->pNames[MAC_NAME_SIZE*nNameNum + MAC_NAME_SIZE -1] = '\0';	// NULL
			if((i+18-1) < nTextCount){
				pMacList->pMacs[nNameNum*MAC_SIZE+0] = hexToInt(&chText[i+1]);
				pMacList->pMacs[nNameNum*MAC_SIZE+1] = hexToInt(&chText[i+4]);
				pMacList->pMacs[nNameNum*MAC_SIZE+2] = hexToInt(&chText[i+7]);
				pMacList->pMacs[nNameNum*MAC_SIZE+3] = hexToInt(&chText[i+10]);
				pMacList->pMacs[nNameNum*MAC_SIZE+4] = hexToInt(&chText[i+13]);
				pMacList->pMacs[nNameNum*MAC_SIZE+5] = hexToInt(&chText[i+16]);
#ifdef	DEBUG_ENABLE
				printf("MAC1=%xh\n", pMacList->pMacs[nNameNum*MAC_SIZE+0]);
				printf("MAC2=%xh\n", pMacList->pMacs[nNameNum*MAC_SIZE+1]);
				printf("MAC3=%xh\n", pMacList->pMacs[nNameNum*MAC_SIZE+2]);
				printf("MAC4=%xh\n", pMacList->pMacs[nNameNum*MAC_SIZE+3]);
				printf("MAC5=%xh\n", pMacList->pMacs[nNameNum*MAC_SIZE+4]);
				printf("MAC6=%xh\n", pMacList->pMacs[nNameNum*MAC_SIZE+5]);
#endif
			}
			i += leftCharCount(&chText[i]);
			nNameNum++;
			nCharCount = 0;
		}else if(chText[i] == '\0'){
			break;
		}else{
			if(nCharCount<MAC_NAME_SIZE){
				chTemp[nCharCount++] = chText[i];
			}
		}
	}

	pMacList->nMacCounts = nNameNum;

	return pMacList;
}


void deleteMacList(struct macList *pMacList){
	free(pMacList->pMacs);
	free(pMacList->pNames);
#ifdef	DEBUG_ENABLE
	printf("pMacList in delete = %p\n", pMacList);////debug
#endif
	free(pMacList);
}


/*
 * Name	: parseCommandMac
 */
int parseCommandMac(char* chMac, char* chGetData, char chCommand[][MAX_COMMAND_SIZE], int nCommandSize, struct macList *pMacList){
	//int nCommand;
	int nLength;
	int ret;
	int i;
	int nMacCounts;
	unsigned char *pMacs;
	char *pNames;

	memset(chMac, 0, sizeof(char)*MAC_SIZE);

	pMacs = pMacList->pMacs;
	pNames = pMacList->pNames;
	nMacCounts = pMacList->nMacCounts;

	// Command parse
	for(i=0;i<nCommandSize;i++){
		nLength = strlen(&chCommand[i][0]);
		ret = strncmp(&chCommand[i][0], chGetData, nLength);
#ifdef	DEBUG_ENABLE
		printf("strncmp(%d) %s : %s\n", ret, &chCommand[i][0], chGetData);
#endif
		if(ret == 0){
			ret = i;
			break;
		}else{
			ret = -EINVAL;
		}
	}

	if(ret < 0){
		return ret;
	}
	
	// Check mac address
	for(i=0;i<nMacCounts;i++){
#ifdef	DEBUG_ENABLE
		printf("strcmp(%d) %s : %s\n", i, &pNames[MAC_NAME_SIZE*i], &chGetData[nLength]);
#endif
		if(strncmp(&pNames[MAC_NAME_SIZE*i], &chGetData[nLength], strlen(&pNames[MAC_NAME_SIZE*i])) == 0){
			memcpy(chMac, &pMacs[MAC_SIZE*i], MAC_SIZE);
			break;
		}
	}

	return ret;
}


void showMacList(struct macList *pMacList){
	int i, n;

	printf("PORT=%d\n", pMacList->nPort);
	n = pMacList->nMacCounts;
	for(i=0;i<n;i++){
		printf("%d] %s = %02x:%02x:%02x:%02x:%02x:%02x\n", i, &pMacList->pNames[MAC_NAME_SIZE*i], pMacList->pMacs[i*MAC_SIZE+0], pMacList->pMacs[i*MAC_SIZE+1], pMacList->pMacs[i*MAC_SIZE+2], pMacList->pMacs[i*MAC_SIZE+3], pMacList->pMacs[i*MAC_SIZE+4], pMacList->pMacs[i*MAC_SIZE+5]);
	}
}
