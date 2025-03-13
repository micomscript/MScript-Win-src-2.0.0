#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "common.h"

#include "expression.h"
#include "logic.h"
#include "varmap.h"
#include "sentence.h"
#include "function.h"
#include "fobj.h"

//----------------------------------------------------------

void AbstructObject::printTree(int s){
	printSpace(s);
	print();
	printf("\n");
}

//----------------------------------------------------------

FileObject::FileObject(char *name, char *mode){
	_state = 0;
	_fname = new char[strlen(name) + 1];
	strcpy(_fname, name);
	_mode = *mode;
	_fp = fopen(name, mode);
	if (_fp != NULL)
		_state = 1;
}

FileObject::~FileObject(){
#ifdef DEBUG
	printf("delete File(%s, %c)\n", _fname, _mode);
#endif
}

char *FileObject::getText(){
	return _fname;
}

char *FileObject::getType(){
	return (char *)"File";
}

Value *FileObject::evaluate(){
	return new Integer(_state);
}

void FileObject::print(){
	printf("File(%s, %c)\n", _fname, _mode);
}

Value *FileObject::methodCall(char *name, List *params){
	char buff[512];

#ifdef DEBUG
	printf("FileObject::methodCall(%s)\n", name);
#endif
	if (strcmp(name, "eof") == 0){
		if (feof(_fp)){
			return new Bool(true);
		}
		return new Bool(false);
	} else if (strcmp(name, "read") == 0){
		if (fgets(buff, 512, _fp) == NULL){
			return NULL;
		}
		return new Text(buff);
	} else if (strcmp(name, "write") == 0){
		int n = params->size();
		if (n == 0){
			printf("Parameter error.");
			return NULL;
		}
		char *s = params->getElement(0)->getText();
		int state = fputs(s, _fp);
		return new Integer(state);
	} else if (strcmp(name, "close") == 0){
		fclose(_fp);
		_state = 0;
		return NULL;
	} else if (strcmp(name, "rewind") == 0){
		if (_state == 1){
			rewind(_fp);
		}
		return NULL;
	} else {
		printf("Undefined function: %s\n", name);
	}
	return NULL;
}

//----------------------------------------------------------

Value *FileOpen::evaluate(){
	int n = _list->size();
	if (n != 2){
		printf("Invalid number of parameters\n");
		throw ErrorException();
	}
	char *name = _list->getElement(0)->getText();
	char *mode = _list->getElement(1)->getText();
	return new FileObject(name, mode);
}

//----------------------------------------------------------
