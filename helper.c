#include "head.h"

/* Check if it's a valid label */

int isLabel(char * s, char * type, int posLineInput)
{
	int i, len, bufferNameLen, err, registerString, actionString;
	char * actions[16] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
	char bufferName[32];

	err = 0;

	len = strlen(s);

	bufferName[0] = '\0';

	strcat(bufferName, s);

	if(bufferName[len-1] == ':') {
		bufferName[len-1] = '\0';
	}

	bufferNameLen = strlen(bufferName);

	if(strlen(bufferName) > 31) {

		err = 1;
	}

	if(s[0] < 64 || s[0] > 123) {

		err = 1;
	}

	for(i = 1; i < bufferNameLen; i++) {

		if(!isalpha(bufferName[i]) && !isdigit(bufferName[i])) {

			err = 1;
		}
	}

	if(!strcmp(type, "full") && s[i] != ':') {

		err = 1;
	}

	if(err) {

		error(Set, "");
		printf("Line %d : Error : Label \"%s\", not valid label \n", posLineInput, s);
	}

	registerString = isRegister(bufferName);

	if(registerString) {

		err = 1;

		error(Set, "");
		printf("Line %d : Error : Label \"%s\", is register \n", posLineInput, s);
	}

	actionString = 0;

	for(i = 0; !actionString && i < 15; i++) {

		if(!strcmp(bufferName, actions[i]))
			actionString = 1;
	}

	if(actionString) {

		err = 1;

		error(Set, "");
		printf("Line %d : Error : Label \"%s\", is a reserved word \n", posLineInput, s);
	}

	return !err;
}

/* Check if it's a valid register */

int isRegister(char * s)
{
	int i, len;

	len = strlen(s) - 1;

	if(s[0] != 'r')
		return 0;

	for(i = 1; i < len; i++) {

		if(!isdigit(s[i])) {

			return 0;
		}
	}

	return 1;
}

/* Check if it's a data or string */

int isData(char * s)
{
	if(!strcmp(s, ".string") || !strcmp(s, ".data"))
		return 1;
	else
		return 0;
}

/* The function copies of string */

void copyString(char target[], char source[])
{
	int i = 0;

	for(i = 0; source[i] != '\0'; i++) {

		target[i] = source[i];
	}

	target[i] = '\0';
}

/* The function performs a valid comma check in the full command */

int commasEqualsNumber(char s[], int amount, int posLineInput)
{

	int i, length, count, isComma, FirstWord, moveToEndFirstWord, isNotFirstComma;

	if(s[0] == ',') {

		error(Set, "");
		printf("Line %d : Error : Commas not valid \n", posLineInput);

		return 0;
	}

	length = strlen(s);

	count = 0;
	FirstWord = FALSE;
	moveToEndFirstWord = FALSE;
	isComma = FALSE;
	isNotFirstComma = TRUE;

	for(i = 0; !moveToEndFirstWord && i < length; i++) {

		if(!FirstWord && s[i] != ' ' && s[i] != '\t' && s[i] != '\n') {

			FirstWord = TRUE;
		}

		if(FirstWord && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n')) {

			moveToEndFirstWord = TRUE;
		}
		
	}

	for(; i < length; i++) {

		if((isNotFirstComma && isComma) || (s[i] == ',' && isComma)) {

			error(Set, "");
			printf("Line %d : Error : Commas not valid \n", posLineInput);

			return 0;
		}

		if(s[i] == ',') {

			isComma = TRUE;

		} else if (s[i] != ',' && s[i] != ' ' && s[i] != '\t' && s[i] != '\n') {

			isNotFirstComma = FALSE;
			isComma = FALSE;
		}

		if(s[i] == 44) {

			count++;
		}
	}

	if(isComma) {

		error(Set, "");
		printf("Line %d : Error : Commas not valid \n", posLineInput);

		return 0;
	}

	if(amount > count) {
		
		error(Set, "");
		printf("Line %d : Error : Missing commas \n", posLineInput);
	
		return 0;
	}


	if(amount < count) {

		error(Set, "");
		printf("Line %d : Error : Multiple consecutive commas \n", posLineInput);
	
		return 0;
	}

	return 1;
}

/* This function checks for text at the end of the full command */

int commandTextAfter(char *cp)
{

	char *comma = " \t";
	int length;

	cp = strtok(NULL, comma);

	if(cp == NULL) {

		return 0;
	}

	length = strlen(cp);

	if(length <= 1) {

		return 0;
	}

	printf("Extraneous text after end of command \n\n");

	return 1;
}

void convertNumberStringToBinatyCode(char * s, char * buffer, int bits)
{
	int number;

	toNumber(s, &number);

	convertNumberToBinatyCode(number, buffer, bits);
}

int convertNumberToBinatyCode(unsigned short number, char * buffer, int bits)
{

	int len = sizeof(number) * 8, pos;
	int mask = 1 << (len - (1 + (len - bits)));

	pos = 0;

	while(mask && pos < bits) {

		if(mask & number) {

			buffer[pos] = '1';

		} else {

			buffer[pos] = '0';

		}

		mask = mask >> 1;
		pos++;

	}

	buffer[pos] = '\0';

	return 0;

}

/* The function translates a number in string to float number and returns whether or not it succeeds */

int toNumber(char s[], int *number)
{	
	int c, sign, offset;
	float n, posDecimal;
	char type;

	posDecimal = 10.0;

	type = 'b';

	if(s[0] == '-')
		sign = -1;
	
	if(sign == -1 || s[0] == '+')
		offset = 1;
	else
		offset = 0;

	n = 0;

	for(c = offset; s[c] != '\0'; c++) {

		if(!isdigit(s[c])) {		

			return 0;
		}
			
		if(type == 'b')
			n = n * 10 + ( s[c] - '0' );

		if(type == 'a') {
			n = n + ( (s[c] - '0') / posDecimal );
			posDecimal = posDecimal * 10;
		}	

	}

	if(sign == -1)
		n = -n;

	*number = n;

	return 1;
}

/* Array encoding for separated data */

int arrayEncoding(char * string, char * nameArrayBuffer, char * cellArrayBuffer)
{
	int i, j = 0, h = 0, type = 0;

	if(isdigit(string[0]))
		return 0;

	for(i = 0; string[i] != '\0'; i++) {
		
		if(string[i] == '[' && type == 0) {
			type = 1;
			i++;
		}

		if(string[i] == ']' && type == 1) {
			type = 2;
		}

		switch(type) {
			case 0:
				nameArrayBuffer[j] = string[i];
				j++;
			break;
			case 1:
				cellArrayBuffer[h] = string[i];
				h++;
			break;
		}
	}

	nameArrayBuffer[j] = '\0';
	cellArrayBuffer[h] = '\0';

	if(type == 2 && string[i-1] == ']')
		return 1;
	else
		return 0;
}

/* Managing code wrinkle errors */

int error(int action, char * s)
{
	static int error = FALSE;
	
	switch(action) {
		case Set:
			error = TRUE;
		break;
		case Reset:
			error = FALSE;
		break;
		case Get:
			return error;
		break;
		case Print:
			printf("%s \n", s);
		break;
		default:
			return error;
		break;	
	}

	return -1;
}

