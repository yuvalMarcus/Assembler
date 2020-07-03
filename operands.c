#include "head.h"

/* Encoding all operands in the command */

void addressingOperands(ptr * head, operand * operands[], char commandInput[], char *cp, int posLine, int operandsNumber, int posLineInput)
{
        char *comma = " ,\t\n";
	int i = 0, commas;
	char buffer[3];

	commas = operandsNumber-1;

	if(commas < 0)
		commas = 0;

	commasEqualsNumber(commandInput, commas, posLineInput);

	while((cp = strtok(NULL, comma)) != NULL && i < operandsNumber) {

		operandEncoding(head, operands[i], buffer, i, cp, posLineInput);

		i++;	
	}

	if(i < operandsNumber) {

		error(Set, "");
		printf("Line %d : Error : invalid operands number : Missing operands \n", posLineInput);
	}

	if(cp != NULL) {

		error(Set, "");
		printf("Line %d : Error : invalid operands number : Too many operands \n", posLineInput);
	}
}

/* Encoding operand in the command */

void operandEncoding(ptr * head, operand * operand, char * buffer, int type, char * s, int posLineInput)
{
	int number, isNumber, err;
	char nameArrayBuffer[120];
	char cellArrayBuffer[120];
	symbol * item;

	operand->string = s;

	if(s[0] == '#') {

		operand->type = 0;
		operand->bufferType[0] = '\0';
		strcat(operand->bufferType, "00");
		
		operand->bufferValue[0] = '\0';

		if(!toNumber(s+1, &number)) {

			if(checkSymbol(head, s+1, "macro")) {

				item = getSymbol(head, s+1, "macro");

				number = item->value;

			} else {

				error(Set, "");
				printf("Line %d : Error : Define %s not exsist, or value not valid \n", posLineInput, s+1);
			}

			convertNumberToBinatyCode(number, operand->bufferValue, 12);

		} else {

			convertNumberStringToBinatyCode((s+1), operand->bufferValue, 12);
		}

	} else if(s[0] == 'r') {

		operand->type = 3;
		operand->bufferType[0] = '\0';
		strcat(operand->bufferType, "11");
		operand->bufferValue[0] = '\0';

		isNumber = toNumber((s+1), &number);

		if(!isNumber || number < 0 || number > 7) {

			error(Set, "");
			printf("Line %d : Error : invalid register set \n", posLineInput);
		}

		convertNumberStringToBinatyCode((s+1), operand->bufferValue, 3);

	} else if(arrayEncoding(s, nameArrayBuffer, cellArrayBuffer)) {

		operand->type = 2;
		operand->bufferType[0] = '\0';
		strcat(operand->bufferType, "10");
		operand->bufferValue[0] = '\0';
		strcat(operand->bufferValue, nameArrayBuffer);
		operand->bufferValueExtra[0] = '\0';

		if(!toNumber(cellArrayBuffer, &number)) {

			if(checkSymbol(head, cellArrayBuffer, "macro")) {

				item = getSymbol(head, cellArrayBuffer, "macro");

				number = item->value;

				err = 0;

			} else {

				err = 1;
				error(Set, "");
				printf("Line %d : Error : Define %s not exsist \n", posLineInput, cellArrayBuffer);
			}

			if(!err && number < 0) {

				error(Set, "");
				printf("Line %d : Error : Number %d, not valid to use in index \n", posLineInput, number);
			}

			convertNumberToBinatyCode(number, operand->bufferValueExtra, 12);

		} else {

			if(number < 0) {

				error(Set, "");
				printf("Line %d : Error : Number %s, not valid to use in index \n", posLineInput, cellArrayBuffer);
			}

			convertNumberStringToBinatyCode(cellArrayBuffer, operand->bufferValueExtra, 12);
		}


	} else {

		operand->type = 1;
		operand->bufferType[0] = '\0';
		strcat(operand->bufferType, "01");
		operand->bufferValue[0] = '\0';
		strcat(operand->bufferValue, s);
	}
			
}

/* Placing the operands encoding in the command */

void setOperands(int active, FILE *fdata, FILE * fextern, ptr *head, int posLine, operand * operands[], int count, int posLineInput)
{
	int posLineOperandsData, location, isLabel, isExtern;
	char buffer[120];
	char bufferNumber[80];
	symbol * item;

	if(count == 0)
		return;

	if(count == 1)
		location = 10;
	else
		location = 8;

	setCodeBits(fdata, posLine, location, operands[0]->bufferType);

	switch(operands[0]->type) {
		case 0:
			posLineOperandsData = setCodeCommand(active, fdata);
			
			if(active)
				setCodeBits(fdata, posLineOperandsData, 0, operands[0]->bufferValue);
			break;
		case 1:
			posLineOperandsData = setCodeCommand(active, fdata);

			if(active) {

			if(checkSymbol(head, operands[0]->bufferValue, "code") || checkSymbol(head, operands[0]->bufferValue, "data")) {

				item = getSymbol(head, operands[0]->bufferValue, "data");

				if(!item)
					item = getSymbol(head, operands[0]->bufferValue, "code");

				buffer[0] = '\0';

				convertNumberToBinatyCode(item->value, buffer, 12);
				
				setCodeBits(fdata, posLineOperandsData, 0, buffer);

				setCodeBits(fdata, posLineOperandsData, 12, "10");

				isLabel = TRUE;
			} else {

				isLabel = FALSE;
			}

			if(checkSymbol(head, operands[0]->bufferValue, "extern")) {


				item = getSymbol(head, operands[0]->bufferValue, "extern");

				sprintf(bufferNumber, "%d", posLineOperandsData + INITIAL_IC_ADDRESS);

				buffer[0] = '\0';

				strcat(buffer, item->name);
				strcat(buffer, " ");

				if(item->value < 1000) {

					strcat(buffer, "0");
				}

				strcat(buffer, bufferNumber);
				strcat(buffer, "\n");

				fseek(fextern, 0, SEEK_END);

				fputs(buffer, fextern);

				setCodeBits(fdata, posLineOperandsData, 12, "01");

				isExtern = TRUE;
			} else {

				isExtern = FALSE;

			}


			if(!isLabel && !isExtern) {

				error(Set, "");
				printf("Line %d : Error : Label %s not exsist \n", posLineInput, operands[0]->bufferValue);
				printf("Line %d : Error : Extern %s not exsist \n", posLineInput, operands[0]->bufferValue);
			}

			}
			
			break;
		case 2:
			posLineOperandsData = setCodeCommand(active, fdata);

			if(active) {
		
				if(checkSymbol(head, operands[0]->bufferValue, "code") || checkSymbol(head, operands[0]->bufferValue, "data")) {

				item = getSymbol(head, operands[0]->bufferValue, "data");

				if(!item)
					item = getSymbol(head, operands[0]->bufferValue, "code");

					buffer[0] = '\0';

					convertNumberToBinatyCode(item->value, buffer, 12);

					setCodeBits(fdata, posLineOperandsData, 0, buffer);

					setCodeBits(fdata, posLineOperandsData, 12, "10");	
				} else {

					error(Set, "");
					printf("Line %d : Error : Name of array %s not exsist \n", posLineInput, operands[0]->bufferValue);

				}
			}

			posLineOperandsData = setCodeCommand(active, fdata);

			if(active) {

				setCodeBits(fdata, posLineOperandsData, 0, operands[0]->bufferValueExtra);		
			}

			break;
		case 3:
			posLineOperandsData = setCodeCommand(active, fdata);
	
			if(active)			
				setCodeBits(fdata, posLineOperandsData, 6, operands[0]->bufferValue);
			break;
	}

	if(count == 1)
		return;

	setCodeBits(fdata, posLine, 10, operands[1]->bufferType);

	switch(operands[1]->type) {
		case 0:
			posLineOperandsData = setCodeCommand(active, fdata);
			
			if(active)
				setCodeBits(fdata, posLineOperandsData, 0, operands[1]->bufferValue);
			break;
		case 1:
			posLineOperandsData = setCodeCommand(active, fdata);

			if(active) {

			if(checkSymbol(head, operands[1]->bufferValue, "code") || checkSymbol(head, operands[1]->bufferValue, "data")) {

				item = getSymbol(head, operands[1]->bufferValue, "data");

				if(!item)
					item = getSymbol(head, operands[1]->bufferValue, "code");

				buffer[0] = '\0';

				convertNumberToBinatyCode(item->value, buffer, 12);
				
				setCodeBits(fdata, posLineOperandsData, 0, buffer);

				setCodeBits(fdata, posLineOperandsData, 12, "10");

				isLabel = TRUE;
			} else {

				isLabel = FALSE;
			}

			if(checkSymbol(head, operands[1]->bufferValue, "extern")) {


				item = getSymbol(head, operands[1]->bufferValue, "extern");

				sprintf(bufferNumber, "%d", posLineOperandsData + INITIAL_IC_ADDRESS);

				buffer[0] = '\0';

				strcat(buffer, item->name);
				strcat(buffer, " ");

				if(item->value < 1000) {

					strcat(buffer, "0");
				}

				strcat(buffer, bufferNumber);
				strcat(buffer, "\n");

				fseek(fextern, 0, SEEK_END);

				fputs(buffer, fextern);

				setCodeBits(fdata, posLineOperandsData, 12, "01");

				isExtern = TRUE;
			} else {

				isExtern = FALSE;

			}

			if(!isLabel && !isExtern) {

				error(Set, "");
				printf("Line %d : Error : Label %s not exsist \n", posLineInput, operands[1]->bufferValue);
				printf("Line %d : Error : Extern %s not exsist \n", posLineInput, operands[1]->bufferValue);
			}

			}
			
			break;
		case 2:
			posLineOperandsData = setCodeCommand(active, fdata);

			if(active) {
		
				if(checkSymbol(head, operands[1]->bufferValue, "code") || checkSymbol(head, operands[1]->bufferValue, "data")) {

				item = getSymbol(head, operands[1]->bufferValue, "data");

				if(!item)
					item = getSymbol(head, operands[1]->bufferValue, "code");

					buffer[0] = '\0';

					convertNumberToBinatyCode(item->value, buffer, 12);

					setCodeBits(fdata, posLineOperandsData, 0, buffer);

					setCodeBits(fdata, posLineOperandsData, 12, "10");
				} else {

					error(Set, "");
					printf("Line %d : Error : Name of array %s not exsist \n", posLineInput, operands[1]->bufferValue);

				}
			}

			posLineOperandsData = setCodeCommand(active, fdata);

			if(active) {

				setCodeBits(fdata, posLineOperandsData, 0, operands[1]->bufferValueExtra);		
			}

			break;
		case 3:
			if(operands[0]->type != 3)
				posLineOperandsData = setCodeCommand(active, fdata);
	
			if(active)			
				setCodeBits(fdata, posLineOperandsData, 9, operands[1]->bufferValue);
			break;
	}
}

/* Check whether the operand is valid in the command */

int operandValid(char * s, int typeOperand, int method)
{
	int err = FALSE;

	if(!strcmp(s, "mov")) {

		if(typeOperand == DES_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "add")) {

		if(typeOperand == DES_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "sub")) {

		if(typeOperand == DES_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "not")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "clr")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "lea")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0 || method == 3) {

				err = TRUE;
			}
		}

		if(typeOperand == DES_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "inc")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "dec")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "jmp")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0 || method == 2) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "ben")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0 || method == 2) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "red")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0) {

				err = TRUE;
			}
		}

	} else if(!strcmp(s, "jsr")) {

		if(typeOperand == SOU_OPERAND) {

			if(method == 0 || method == 2) {

				err = TRUE;
			}
		}

	}

	return err;
}


