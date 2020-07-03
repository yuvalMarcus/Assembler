#include "head.h"

int addressIC(int action)
{
	static int address = INITIAL_IC_ADDRESS;

	switch(action) {
		case Reset:
			address = INITIAL_IC_ADDRESS;

		break;
		case Get:
			return address;
		break;
		case Increase:
			address++;
		break;
		case Decrease:
			address--;
		break;
		default:
			return address;
		break;	
	}

	return -1;
}

int addressDC(int action)
{
	static int address = INITIAL_DC_ADDRESS;

	switch(action) {
		case Reset:
			address = INITIAL_DC_ADDRESS;
		break;
		case Get:
			return address;
		break;
		case Increase:
			address++;
		break;
		case Decrease:
			address--;
		break;
		default:
			return address;
		break;	
	}

	return -1;
}

int updateAddressesSymbol(ptr *head)
{
	ptr curr = NULL;

	curr = *head;
	
	while(curr != NULL) {

		if(!strcmp(curr->property, "data"))
			curr->value = curr->value + addressIC(Get);

		curr = curr->next;
	}

	return 0;
}

