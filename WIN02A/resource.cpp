////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Begin license text.

Copyright 2023- MicomScript Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the ÅgSoftwareÅh), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED ÅgAS ISÅh, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.

End license text.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "common.h"

#include "expression.h"
#include "resource.h"

WiFiRes		_wifi;

Value *WiFiRes::evaluate(){
	return NULL;
}

bool WiFiRes::eval(){
	return false;
}

void WiFiRes::print(){
	printf("WiFi\n");
}

void WiFiRes::printTree(){
	printf("WiFi\n");
}

Value *WiFiRes::methodCall(char *name, List *params){
	printf("Wifi.%s()\n", name);
	if (strcmp(name, "begin") == 0){
		begin(params);
		return NULL;
	} else if (strcmp(name, "status") == 0){
		int state = status();
	} else if (strcmp(name, "localIP") == 0){
		printf(".........localIP()\n");
	} else {
		printf("Unknown method name : %s\n", name);
	}
	return NULL;
}

void WiFiRes::begin(List *params){
	printf("WiFiRes::begin()\n");
}

int WiFiRes::status(){
	printf("WiFiRes::status()\n");
	return 0;
}
