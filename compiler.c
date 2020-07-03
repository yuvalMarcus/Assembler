#include "head.h"

/* Main function compile source code */

int compiler(char *file)
{
	/* Compiler list of commands */

	command cmd[] = {
		{".entry", "", entryAction},
		{".extern", "", externAction},
		{".string", "", stringAction},
		{".data", "", dataAction},
		{".define", "", defineAction},
		{"mov", "0000", movAction},
		{"cmp", "0001", cmpAction},
		{"add", "0010", addAction},
		{"sub", "0011", subAction},
		{"not", "0100", notAction},
		{"clr", "0101", clrAction},
		{"lea", "0110", leaAction},
		{"inc", "0111", incAction},
		{"dec", "1000", decAction},
		{"jmp", "1001", jmpAction},
		{"bne", "1010", bneAction},
		{"red", "1011", redAction},
		{"prn", "1100", prnAction},
		{"jsr", "1101", jsrAction},
		{"rts", "1110", rtsAction},
		{"stop", "1111", stopAction}
	};

	/* Setting operand */

	operand souOperand = {"", -1, "", SOU_OPERAND, "", ""};
	operand desOperand = {"", -1, "", DES_OPERAND, "", ""};

	operand * operands[2];

	ptr tableSymbolsHead = NULL;

        char *comma = " \t\n";
	char *cp;
	char buffer[3], addressICDC[10];
	char * nameFileInput;
	char * nameFileEnt;
	char * nameFileExt;
	char * nameFileOb;

	char commandInput[LENGTH_COMMAND], command[LENGTH_COMMAND];

	int numberRun, h, n, j, err, loopRun, cpLength, cmdLength = sizeof(cmd) / sizeof(cmd[0]), posLine, posLineInput;

	FILE *finput, *fcode, *fdata, *fentry, *fextern, *fob;

	/* Setting name file input */

	nameFileInput = malloc(strlen(file) + strlen(".as") + 1);

	strcpy(nameFileInput, file);
	strcat(nameFileInput, ".as");

	printf("\033[1m\033[36mCompiler to file : %s \033[0m \n\n", nameFileInput);

	operands[0] = &souOperand;
	operands[1] = &desOperand;

	finput = fopen(nameFileInput, "r+");

	if(finput == NULL) {

		printf("file %s not exsist \n", nameFileInput);

		return 0;
	}

	/* Setting name file entry */

	nameFileEnt = malloc(strlen(file) + strlen(".ent") + 1);

	strcpy(nameFileEnt, file);
	strcat(nameFileEnt, ".ent");

	/* Setting name file extern */

	nameFileExt = malloc(strlen(file) + strlen(".ext") + 1);

	strcpy(nameFileExt, file);
	strcat(nameFileExt, ".ext");

	/* Setting name file object */

	nameFileOb = malloc(strlen(file) + strlen(".ob") + 1);

	strcpy(nameFileOb, file);
	strcat(nameFileOb, ".ob");

	fcode = fopen("code.txt", "w+");
	fdata = fopen("data.txt", "w+");
	fentry = fopen(nameFileEnt, "w+");
	fextern = fopen(nameFileExt, "w+");
	fob = fopen(nameFileOb, "w+");

	numberRun = 0;

	err = 0;

	while(numberRun < 2) {

	loopRun = 1;

	posLineInput = 1;

	/* Scan the first and second pass commands */

	while(loopRun && !err) {

	  	if(fgets(command, LENGTH_COMMAND, finput) != NULL)
		{
			copyString(commandInput, command);

			cp = strtok(command, comma);

			if(cp != NULL && command[0] != ';') {

				cpLength = strlen(cp);

				if(commasEqualsNumber(cp, 0, FALSE) && cp[cpLength-1] != ',') {

					for(j = 0; j < cmdLength && strcmp(cp, cmd[j].name) != 0; j++) {}

					if(j == cmdLength) {

						loopRun = codeAction(numberRun, cmd[j].binaryCode, &tableSymbolsHead, operands, commandInput, cp, fcode, fdata, fentry, fextern, posLineInput);

					} else {
						
						loopRun = (*(cmd[j].func))(numberRun, cmd[j].binaryCode, &tableSymbolsHead, operands, commandInput, cp, fcode, fdata, fentry, fextern, posLineInput);
					}

				} else {
			
					printf("Illegal comma \n\n");
				}

			}

		} else {
		
			loopRun = 0;

		}

	posLineInput++;

	}

	if(!numberRun) {

		/* Update icon table */

		updateAddressesSymbol(&tableSymbolsHead);

		addressIC(Reset);
		addressDC(Reset);
	}

	err = error(Get, "");

	fseek(finput, 0, SEEK_SET);

	numberRun++;

	}

	if(!err) {

		/* Build the final output file */

		sprintf(addressICDC, "%d %d", (addressIC(Get) - INITIAL_IC_ADDRESS), (addressDC(Get) - INITIAL_DC_ADDRESS));

		fseek(fdata, 0, SEEK_SET);

		while(fgets(command, LENGTH_COMMAND, fdata) != NULL) {

			posLine = setCodeCommand(1, fcode);

			setCodeBits(fcode, posLine, 0, command);
		}

		fseek(fcode, 0, SEEK_SET);

		addressIC(Reset);


		/* Enter the row numbers that are built into the object file */

		fseek(fob, 0, SEEK_SET);

		fputs("            \n", fob);

		fseek(fob, 0, SEEK_SET);

		fputs(addressICDC, fob);


		while(fgets(command, LENGTH_COMMAND, fcode) != NULL) {

			posLine = setObCommand(fob);

			for(h = 1; h < 8; h++) {

				n = h * 2;

				buffer[0] = command[n - 1 + 4];
				buffer[1] = command[n + 4];
				buffer[2] = '\0';

				if(!strcmp(buffer, "00")) {

					buffer[0] = '*';

				} else if(!strcmp(buffer, "01")) {

					buffer[0] = '#';

				} else if(!strcmp(buffer, "10")) {

					buffer[0] = '%';

				} else if(!strcmp(buffer, "11")) {

					buffer[0] = '!';
				}

				buffer[1] = '\0';

				setObBits(fob, posLine, h-1, buffer);
			}
		}

		printf("\033[1m\033[32mProper program found, output files created\033[0m\n");

		return 1;

	} else {

		remove(nameFileEnt);
		remove(nameFileExt);

		printf("\n\033[1m\033[31mError found in program, output files were not created\033[0m\n");
	}

	remove("code.txt");
	remove("data.txt");

	free(nameFileInput);
	free(nameFileEnt);
	free(nameFileExt);
	free(nameFileOb);

	fclose(finput);
	fclose(fcode);
	fclose(fdata);
	fclose(fentry);
	fclose(fextern);
	fclose(fob);

	addressIC(Reset);
	addressDC(Reset);

	error(Reset, "");

	return 0;
}
