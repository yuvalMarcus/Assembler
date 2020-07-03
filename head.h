#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <regex.h>

#define TRUE 1
#define FALSE 0
#define LENGTH_COMMAND 82
#define INITIAL_IC_ADDRESS 100
#define INITIAL_DC_ADDRESS 0
#define BUFFER_COMMAND_LINE 21
#define BUFFER_COMMAND_ADDRESS 5
#define SOU_OPERAND 0
#define DES_OPERAND 1

enum address {Reset, Get, Increase, Decrease, Set, Print};

/* struct of symbol in the symbol table */

typedef struct node * ptr;

typedef struct node {
	char name[LENGTH_COMMAND];
	int value;
	char property[LENGTH_COMMAND];
	int external;
	ptr next;
} symbol;

/* struct of operand */

typedef struct {
	char *string;
	int type;
	char bufferType[3];
	int position;
	char bufferValue[13];
	char bufferValueExtra[13];
} operand;

/* struct of command */

typedef struct {
	char *name;
	char * binaryCode;
	int (*func)(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
} command;

int addressIC(int action);
int addressDC(int action);

void addSymbol(ptr *head, char * name, int value, char * property, int external);
symbol * getSymbol(ptr *head, char * name, char * property);
int checkSymbol(ptr *head, char * name, char * property);
void clearSymbol(ptr *head);

int updateAddressesSymbol(ptr *head);

int getObPosLine(int pos);
int setObCommand(FILE * fob);
void setObBits(FILE *fob, int line, int bits, char * s);

int getDataPosLine(int pos);
int setDataCommand(int active, FILE * fdata);
void setDataBits(FILE *fdata, int line, int bits, char * s);

int getCodePosLine(int pos);
int setCodeCommand(int active, FILE *fdata);
void setCodeBits(FILE *fdata, int line, int bits, char * s);

void addressingOperands(ptr * head, operand * operands[], char commandInput[], char *cp, int posLine, int operandsNumber, int posLineInput);
void operandEncoding(ptr * head, operand * operand, char * buffer, int type, char * s, int posLineInput);
void setOperands(int active, FILE *fdata, FILE * fextern, ptr *head, int posLine, operand * operands[], int count, int posLineInput);
int operandValid(char * s, int typeOperand, int method);

void convertNumberStringToBinatyCode(char * s, char * buffer, int bits);
int convertNumberToBinatyCode(unsigned short number, char * buffer, int bits);
int toNumber(char s[], int *number);
int arrayEncoding(char * string, char * nameArrayBuffer, char * cellArrayBuffer);

int entryAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int externAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int defineAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int codeAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);

int commandAction(int active, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);

int movAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int cmpAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int addAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int subAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int notAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int clrAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int leaAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int incAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int decAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int jmpAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int bneAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int redAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int prnAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int jsrAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int rtsAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int stopAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);

int stringAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);
int dataAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);

int warningAction(int numberRun, char * binaryCode, ptr *head, operand * operands[], char commandInput[], char *cp, FILE * fcode, FILE * fdata, FILE * fentry, FILE * fextern, int posLineInput);

int isLabel(char * s, char * type, int posLineInput);
int isRegister(char * s);
int isData(char * s);

void copyString(char target[], char source[]);
int commasEqualsNumber(char s[], int amount, int showError);
int commandTextAfter(char *cp);

int error(int action, char * s);
