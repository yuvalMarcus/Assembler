#include "head.h"

/* Get the row position in an object file */

int getObPosLine(int pos)
{
	return (13 * (pos+1));
}

/* Insert a new command into an object file  */

int setObCommand(FILE * fob)
{
	char bufferNumber[BUFFER_COMMAND_ADDRESS];
	char buffer[BUFFER_COMMAND_LINE];
	int NextCommandLineAdrees = addressIC(Get);
	int posLineInFile = getObPosLine(NextCommandLineAdrees - INITIAL_IC_ADDRESS);

	sprintf(bufferNumber, "%d", NextCommandLineAdrees);

	if(NextCommandLineAdrees < 1000) {

		buffer[0] = '0';
		buffer[1] = '\0';
	} else {
		buffer[0] = '\0';
	}
	
	strcat(buffer, bufferNumber);
	strcat(buffer, " ");
	strcat(buffer, "*******\n");

	fseek(fob, posLineInFile, SEEK_SET);

	fputs(buffer, fob);

	addressIC(Increase);

	return (NextCommandLineAdrees - 100);
}

/* Enter code into command line an object file */

void setObBits(FILE *fob, int line, int bits, char * s)
{
	int pos;

	pos = getObPosLine(line) + 5 + bits;

	fseek(fob, pos, SEEK_SET);

	fputs(s, fob);
}

/* Get the row position in an data file */

int getDataPosLine(int pos)
{
	return (15 * pos);
}

/* Insert a new command into an data file  */

int setDataCommand(int active, FILE * fdata)
{
	int NextCommandLineAdrees = addressDC(Get);
	int posLineInFile = getDataPosLine(NextCommandLineAdrees);

	if(active) {

		fseek(fdata, posLineInFile, SEEK_SET);

		fputs("00000000000000\n", fdata);	
	}

	addressDC(Increase);

	return NextCommandLineAdrees;
}

/* Enter code into command line an data file */

void setDataBits(FILE *fdata, int line, int bits, char * s)
{
	int pos;

	pos = getDataPosLine(line) + bits;

	fseek(fdata, pos, SEEK_SET);

	fputs(s, fdata);
}

/* Get the row position in an code file */

int getCodePosLine(int pos)
{
	return ((BUFFER_COMMAND_LINE - 1) * pos);
}

/* Insert a new command into an code file  */

int setCodeCommand(int active, FILE *fdata)
{
	char bufferNumber[BUFFER_COMMAND_ADDRESS];
	char buffer[BUFFER_COMMAND_LINE];
	int NextCommandLineAdrees = addressIC(Get);
	int posLineInFile = getCodePosLine(NextCommandLineAdrees - 100);

	if(active) {

		sprintf(bufferNumber, "%d", NextCommandLineAdrees);

		if(NextCommandLineAdrees < 1000) {

			buffer[0] = '0';
			buffer[1] = '\0';
		} else {
			buffer[0] = '\0';
		}
	
		strcat(buffer, bufferNumber);
		strcat(buffer, " ");
		strcat(buffer, "00000000000000\n");

		fseek(fdata, posLineInFile, SEEK_SET);

		fputs(buffer, fdata);
	}

	addressIC(Increase);

	return (NextCommandLineAdrees - 100);
}

/* Enter code into command line an code file */

void setCodeBits(FILE *fdata, int line, int bits, char * s)
{
	int pos;

	pos = getCodePosLine(line) + 5 + bits;

	fseek(fdata, pos, SEEK_SET);

	fputs(s, fdata);
}

