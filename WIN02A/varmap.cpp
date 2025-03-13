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

/*
 * varmap.cpp
 */
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "expression.h"
#include "sentence.h"
#include "varmap.h"

VarMapStack	_varMap;
// ÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑü

VarMapLayer::VarMapLayer(){
	_top = _last = NULL;
}

VarMapLayer::~VarMapLayer(){
#ifdef DEBUG
	printf("delete VarMapLayer\n");
#endif
	clear();
}

void VarMapLayer::clear(){
	VarElement *node, *next;
	for(node = _top; node ; node = next){
		next = node->getNext();
		delete node;
	}
	_top = _last = NULL;
}

VarElement *VarMapLayer::lookFor(const char *name){
	VarElement *node;
	for(node = _top; node ; node = node->getNext()){
		if (strcmp(node->getName(), name) == 0){
			return node;
		}
	}
	return NULL;
}

void VarMapLayer::put(const char *name, Value *value){
	VarElement *node = lookFor(name);
	if (node){
		Variable *var = node->getVariable();
		var->setValue(value);
		return;
	}
	Variable *var = new Variable(name);
	var->setValue(value);
	node = new VarElement(var, NULL);
	if (_top){
		_last->setNext(node);
		_last = node;
	} else {
		_top = _last = node;
	}
}

bool VarMapLayer::exist(const char *name){
	return lookFor(name) != NULL;
}

void VarMapLayer::removeVar(const char *name){
	VarElement *node, *pre = NULL;
	for(node = _top; node ; node = node->getNext()){
		if (strcmp(node->getName(), name) == 0){
			if (pre){
				pre->setNext(node->getNext());
			} else {
				_top = node->getNext();
			}
			delete node;
			return;
		}
		pre = node;
	}
	printf("Error :Nnot found Variable : %s (line = %d)\n",name,_line_number);
	throw ErrorException();
}

// ïœêîÇÃàÍóóï\ÇèoóÕÇ∑ÇÈÅB
void VarMapLayer::printVars(){
	int i;
	VarElement *node;
	for(node = _top; node ; node = node->getNext()){
		node->print();
	}
}
Variable *VarMapLayer::getVariable(const char *name){
	VarElement *node = lookFor(name);
	if (node)
		return node->getVariable();
	return NULL;
}

// ÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑü

void VarMapStack::stackUp(){
	if (_curLayer >= VMAP_STACK_SIZE - 1){
		printf("Error : Stack overflow. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	_vmap[++_curLayer] = new VarMapLayer();
}

void VarMapStack::stackDown(){
	if (_curLayer == 0){
		printf("Error : Stack underflow. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	delete _vmap[_curLayer];
	_curLayer--;
}

void VarMapStack::clearLayer(){
	_vmap[_curLayer]->clear();
}

void VarMapStack::clearAll(){
	for(int i = 1; i < _curLayer ; i++){
		delete _vmap[i];
	}
	_vmap[0]->clear();
	_curLayer = 0;
}

void VarMapStack::put(const char *name, Value *value){
	_vmap[_curLayer]->put(name, value);
}

bool VarMapStack::exist(const char *name){
	return _vmap[_curLayer]->lookFor(name) != NULL;
}

void VarMapStack::removeVar(const char *name){
	_vmap[_curLayer]->removeVar(name);
}

// ïœêîÇÃàÍóóï\ÇèoóÕÇ∑ÇÈÅB
void VarMapStack::printVars(){
	printf("[Global Vars]\n");
	_vmap[0]->printVars();

	if (_curLayer > 0){
		printf("[Local Vars]\n");
		_vmap[_curLayer]->printVars();
	}
}

Variable *VarMapStack::getVariable(const char *name){
//	return _vmap[_curLayer]->getVariable(name);
	Variable *v = _vmap[_curLayer]->getVariable(name);
	if ((v == NULL) && (_curLayer > 0)){
		v = _vmap[0]->getVariable(name);
	}
	return v;
}

