#include "head.h"

/* Main & Supporting Functions */
int main(int argc, char **argv)
{
	int i;

	if(argc < 2) {

		printf("No files found to compile please try again \n");
	}

	if(argc >= 2) {

		for(i = 1; i < argc; i++) {

			printf("\n");

			compiler(argv[i]);

			printf("\n");
		}		
	}

	printf("The compiler operation ended \n\n");

	return 0;
}
