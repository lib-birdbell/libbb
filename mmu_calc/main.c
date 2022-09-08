#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>



#define	LEVEL_PAGE	1
#define	LEVEL_SECTION	2
#define	LEVEL_RESERVED	3
int main(int argc, char** argv){
	unsigned int hex;
	unsigned int nSectionBaseAddress;
	unsigned int AP;
	int levelone;

	printf("APP calc mmu(memory management unit)\n");

	if(argc < 3){
		printf("How to use : calc_mmu [option] [4byte hex value]\n");
		printf("Option : -a = address calc\n");
		printf("Option : -c = data convert\n");
		return -EINVAL;
	}

	hex = strtol(argv[2], NULL, 16);

	if(strcmp(argv[1], "-c") == 0){

		printf("hex=%xh\n", hex);
	
		levelone = (hex&0x03);
		switch(levelone){
			case 1:
				printf("level one Page\n");
				break;
			case LEVEL_SECTION:
				printf("level one Section\n");
				break;
			case 3:
				printf("level one Reserved\n");
				break;
			default:
				printf("level one Fault!\n");
				break;
		}
	
		if(levelone == LEVEL_SECTION){
			nSectionBaseAddress = (hex & 0xFFF00000);
			printf("Section Base Address = %xh\n", nSectionBaseAddress);
	
			AP = (hex & 0x00000C00) >> 10;
			printf("AP=%xh\n", AP);
			switch(AP){
				case 0:
					break;
				case 1:
					printf("Permissions Supervisor : Read/Write\n");
					printf("User : No Access\n");
					break;
				case 2:
					printf("Permissions Supervisor : Read/Write\n");
					printf("User : Read Only\n");
					break;
				case 3:
					printf("Permissions Supervisor : Read/Write\n");
					printf("User : Read/Write\n");
					break;
				default:
					break;
			}
		}
	}else if(strcmp(argv[1], "-a") == 0){
		//printf("Virt Address = %xh\n", hex*0x100000);
		printf("Virt Address = %xh\n", hex*0x40000);
	}

	return 0;
}
