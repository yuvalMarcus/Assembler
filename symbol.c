#include "head.h"

/* Add symbol to the symbol table */

void addSymbol(ptr *head, char * name, int value, char * property, int external)
{
	ptr curr;

	symbol * item = (ptr) malloc(sizeof(symbol));

	item->name[0] = '\0';
	item->property[0] = '\0';

	strcat(item->name, name);
	strcat(item->property, property);

	item->value = value;
	item->external = external;

	if(*head == NULL) {

		*head = item;
	} else {

		curr = *head;

		while(curr->next != NULL) {

			curr = curr->next;
		}

		curr->next = item;
	}
}

/* Get symbol from symbol table */

symbol * getSymbol(ptr *head, char * name, char * property)
{
	ptr curr = NULL;
	symbol * item;

	curr = *head;
	
	while(curr != NULL) {

		if(!strcmp(curr->name,name) && !strcmp(curr->property,property)) {

			item = curr;
			
			return item;

		}

		curr = curr->next;
	}

	return NULL;
}

/* Check symbol from symbol table */

int checkSymbol(ptr *head, char * name, char * property)
{
	ptr curr = NULL;

	curr = *head;
	
	while(curr != NULL) {

		if(!strcmp(curr->name,name) && !strcmp(curr->property,property))
			 return 1;

		curr = curr->next;
	}

	return 0;
}

