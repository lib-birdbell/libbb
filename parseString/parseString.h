#ifndef	PARSE_STRING_LIBBB
#define	PARSE_STRING_LIBBB
#endif


#define	MAX_COMMAND_SIZE	32



struct macList {
	unsigned char *pMacs;
	char *pNames;
	int nMacCounts;
	int nPort;
};


/*
 * Function name	: parseMacList
 * Arg1			: Full text of characters to be parsed.
 * Return		: struct macList pointer
 * Remarks		: *After use, you must call the free() function.
 * 			  char text format
 * 			  PORT=12345
 * 			  IVY=00:00:00:00:00:00
 * 			  GREEN=00:00:00:00:00:01
 */
struct macList *parseMacList(char* chText);
void deleteMacList(struct macList *pMacList);
int parseCommandMac(char* chMac, char* chGetData, char chCommand[][32], int nCommandSize, struct macList *pMacList);
void showMacList(struct macList *pMacList);
