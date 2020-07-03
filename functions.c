#include "head.h"

/* Function manage the entry command */

int entryAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	char *comma = " \t\n";
	char buffer[80];
	char bufferNumber[80];
	symbol * item;

	if(numberRun) {

		cp = strtok(NULL, comma);

		if(checkSymbol(head, cp, "data") || checkSymbol(head, cp, "code")) {

			item = getSymbol(head, cp, "data");

			if(!item)
				item = getSymbol(head, cp, "code");

			sprintf(bufferNumber, "%d", item->value);

			strcat(buffer, cp);
			strcat(buffer, " ");

			if(item->value < 1000) {

				strcat(buffer, "0");
			}

			strcat(buffer, bufferNumber);
			strcat(buffer, "\n");

			fseek(fentry, 0, SEEK_END);

			fputs(buffer, fentry);

		} else {

			error(Set, "");
			printf("Line %d : Error : Label %s not exists \n", posLineInput, cp);

		}

	}

	return 1;
}

/* Function manage the extern command */

int externAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	char *comma = " \t\n";
	char bufferName[80];
	
	if(!numberRun) {

		cp = strtok(NULL, comma);

		bufferName[0] = '\0';

		strcat(bufferName, cp);

		if(!checkSymbol(head, bufferName, "code") && !checkSymbol(head, bufferName, "data") && !checkSymbol(head, bufferName, "macro")) {

			addSymbol(head, bufferName, 0, "extern", 1);
		} else {

			error(Set, "");
			printf("Line %d : Error : Label %s exists \n", posLineInput, cp);
		}
	}

	return 1;
}

/* Function manage the define command */

int defineAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	char *comma = " \t\n";
	char bufferName[80];
	int isNumber, number, labelString;
	
	if(!numberRun) {

		cp = strtok(NULL, comma);

		labelString = isLabel(cp, "partial", posLineInput);

		if(!labelString)
			return 1;

		if(checkSymbol(head, bufferName, "macro")) {

			error(Set, "");
			printf("Line %d : Error : Define %s exists \n", posLineInput, cp);
			return 1;
		}


		if(checkSymbol(head, bufferName, "code") || checkSymbol(head, bufferName, "data")) {

			error(Set, "");
			printf("Line %d : Error : Label %s exists \n", posLineInput, cp);
			return 1;
		}

		bufferName[0] = '\0';

		strcat(bufferName, cp);

		cp = strtok(NULL, comma);

		if(strcmp(cp, "=")) {
	
			error(Set, "");
			printf("Line %d : Error : Define not valid \n", posLineInput);

			return 1;			
		}
	
		cp = strtok(NULL, comma);

		isNumber = toNumber(cp, &number);

		if(!isNumber) {

			error(Set, "");
			printf("Line %d : Error : Value %s of define not valid \n", posLineInput, cp);

			return 1;
		}

		addSymbol(head, bufferName, number, "macro", 0);

	}

	return 1;
}

/* Function manage the label command */

int codeAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	commandAction(numberRun, binaryCode, head, operands, commandInput, cp, fcode, fdata, fentry, fextern, posLineInput);

	return 1;
}

/* Function manage the label command */

int commandAction(int active, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	command cmd[] = {
		{".entry", "", warningAction},
		{".extern", "", warningAction},
		{".string", "", stringAction},
		{".data", "", dataAction},
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

	int j, len, cmdLength = sizeof(cmd) / sizeof(cmd[0]), labelString;

	char *comma = " \t\n";

	char bufferName[32];

	labelString = isLabel(cp, "full", posLineInput);

	if(!labelString) {

			return 1;
	}

	if(!active) {
		
		len = strlen(cp);

		bufferName[0] = '\0';

		strcat(bufferName, cp);

		bufferName[len-1] = '\0';
	}

	cp = strtok(NULL, comma);

	if(cp == NULL || !strcmp(cp, ";")) {

		error(Set, "");
		printf("Line %d : Error : Command empty or note after lebal is not valid \n", posLineInput);
		return 1;
	}

	if(!active) {

		if(isData(cp)){

			if(!checkSymbol(head, bufferName, "data")) {

				addSymbol(head, bufferName, addressDC(Get), "data", 0);
			}

		} else {

			if(!checkSymbol(head, bufferName, "code"))
				addSymbol(head, bufferName, addressIC(Get), "code", 0);
		}	
	}

	for(j = 0; j < cmdLength && strcmp(cp, cmd[j].name) != 0; j++) {}

	if(j == cmdLength) {
		
		error(Set, "");
		printf("Line %d : Error : Action %s not valid \n", posLineInput, cp);

	} else {
				
		commandInput = commandInput + len;
		
		(*(cmd[j].func))(active, cmd[j].binaryCode, head, operands, commandInput, cp, fcode, fdata, fentry, fextern, posLineInput);
	}

	return 1;
}

/* Function manage the mov command */

int movAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 2;

	err = 0;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);

		if(err) {

			error(Set, "");
			printf("Line %d : Error : operand %s not valid in command mov \n", posLineInput, operands[i]->string);
			err = 0;
		}
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);

	return 1;
}

/* Function manage the cmp command */

int cmpAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 2;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the add command */

int addAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 2;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the sub command */

int subAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 2;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the not command */

int notAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the clr command */

int clrAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the lea command */

int leaAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 2;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the inc command */

int incAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the dec command */

int decAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the jmp command */

int jmpAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the bne command */

int bneAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the red command */

int redAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the prn command */

int prnAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the jsr command */

int jsrAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int i, posLine, err, operandsCount;

	operandsCount = 1;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	for(i = 0; i < operandsCount; i++) {

		err = operandValid(cp, i, operands[i]->type);
	}

	if(err) {

		error(Set, "");
		error(Print, "invalid operands");
	}

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the rts command */

int rtsAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int posLine, operandsCount;

	operandsCount = 0;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the stop command */

int stopAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	int posLine, operandsCount;

	operandsCount = 0;

	posLine = setCodeCommand(numberRun, fcode);

	if(numberRun)
		setCodeBits(fcode, posLine, 4, binaryCode);

	addressingOperands(head, operands, commandInput, cp, posLine, operandsCount, posLineInput);

	setOperands(numberRun, fcode, fextern, head, posLine, operands, operandsCount, posLineInput);	

	return 1;
}

/* Function manage the string command */

int stringAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{	
	char *comma = " \t\n";
	int i, len, err, posLine, number;
	char buffer[14];

	cp = strtok(NULL, comma);

	len = strlen(cp);

	err = 0;

	if(cp[0] != '"' || cp[len-1] != '"') {

		error(Set, "");
		printf("Line %d : Error : string %s not valid \n", posLineInput, cp);
	}

	for(i = 1; i < (len-1); i++) {

		posLine = setDataCommand(!numberRun, fdata);

		if(cp[i] == '"') {

			err = 1;
		}

		if(!numberRun) {
		
			number = (int) cp[i];

			convertNumberToBinatyCode(number, buffer, 14);

			setDataBits(fdata, posLine, 0, buffer);

		}
	}

	if(err) {

		error(Set, "");
		printf("Line %d : Error : string %s not valid \n", posLineInput, cp);
	}

	posLine = setDataCommand(!numberRun, fdata);

	return 1;
}

/* Function manage the data command */

int dataAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	char *comma = " ,\t\n";
	char buffer[14];
	int i, number, isNumber, posLine;
	symbol * item;

	i = 0;

	while((cp = strtok(NULL, comma)) != NULL) {

		isNumber = toNumber(cp, &number);

		if(!isNumber && checkSymbol(head, cp, "macro")) {

			item = getSymbol(head, cp, "macro");
			number = item->value;
		}

		if(!isNumber && !checkSymbol(head, cp, "macro")) {

			error(Set, "");
			printf("Line %d : Error : In data, \"%s\" is not valid \n", posLineInput, cp);
		}

		posLine = setDataCommand(!numberRun, fdata);

		if(!numberRun) {

			convertNumberToBinatyCode(number, buffer, 14);

			setDataBits(fdata, posLine, 0, buffer);

		}

		i++;
	}

	if(!i) {

		error(Set, "");
		printf("Line %d : Error : Command empty after .data is not valid \n", posLineInput);
		return 1;
	}

	commasEqualsNumber(commandInput, i-1, posLineInput);

	return 1;
}

int warningAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput)
{
	printf("Line %d : Warning : the %s guideline cannot come after a label \n", posLineInput, cp);

	return 1;
}

