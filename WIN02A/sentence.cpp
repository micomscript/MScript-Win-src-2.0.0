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
 * sentence.cpp
 */
#include<stdio.h>
#include<string.h>
#include<math.h>
#include "common.h"

#ifdef T_ESP32
#include<ESP.h>
#endif

#include "expression.h"
#include "varmap.h"
#include "logic.h"
#include "sentence.h"
#include "function.h"

extern VarMapStack _varMap;
extern int _system_mode;
extern bool print_Mode;
extern int _line_number;

// execute ---------------------------------
#ifdef T_ESP32
void Reset::execute() {
	ESP.restart();
}
#endif

void FormulaSentence::execute() {
	if(_expr) {
#ifdef DEBUG
	//éÆÇÃï\é¶
		printf("Expression: ");
		_expr->print();
		printf("\n");

	//ñÿÇÃï\é¶
		_expr->printTree(0);
#endif
		bool print_ch = _expr->isPrint();
		Value *value = _expr->evaluate();
		if(print_ch == false)
			print_ch = print_Mode;
		if(value != NULL) {
			if(print_ch == false && _system_mode == 0) {
				value->print();
				printf("\n");
			}
			delete value;
		}
	}
}

void VarSentence::execute() {
	VarNode *node;
	Value *value;
	for(node = _top; node; node = node->_next) {
		if(node->_express)
			value = node->_express->evaluate();
		else
			value = NULL;
		_varMap.put(node->_name, value);
	}
}

void BlockSentence::execute() {
	SentenceNode *node;
	for(node = _top; node ; node = node->_next) {
		node->_sentence->execute();
	}
}

void IfSentence::execute() {
	IfNode *node;
	for(node = _top; node; node = node->_next) {
		if(node->_logic == NULL) {
			node->_sentence->execute();
			return;
		}
		if(node->_logic->eval()){
			node->_sentence->execute();
			return;
		}
	}
}

void WhileSentence::execute() {
	Value *result;
	try {
		if(_do_ch) {
			try {
				_sentence->execute();
			} catch(ContinueException e) {
				;
			} catch(BreakException e) {
				return;
			}
		}
		while(_logic->eval()) {
			try {
				_sentence->execute();
			} catch(ContinueException e) {
				;
			} catch(BreakException e) {
				return;
			}
		}
	} catch(ErrorException e) {}
}

void ForSentence::execute() {
	Value *result;
	try {
		_initial_val->evaluate();
		while(_logic->eval()) {
			try {
				_sentence->execute();
			} catch(ContinueException e) {
				;
			} catch(BreakException e) {
				return;
			}

			if(_e2) {
				Value *v = _e2->evaluate();
				if(v)
					delete v;
			}
		}
	} catch(ErrorException e) {}
}

void ForeachSentence::execute() {
	try {
		Variable *var = _varMap.getVariable(_vname);
		if(var == NULL) {
			return;
		}
		Variable *v = _varMap.getVariable(_listname);
		if(v == NULL) {
			return;
		}
		Value *value = v->evaluate();
		if(!value->isList()) {
			printf("Error : Specify the array name. (line = %d)\n",_line_number);
			return;
		}
		List *list = (List *)value;
		for(int i = 0; i < list->size(); i++) {
			var->setValue(list->getElement(i)->evaluate());
			try {
				_sentence->execute();
			} catch(ContinueException e) {
				;
			} catch(BreakException e) {
				delete list;
				return;
			}
		}
		delete list;
	} catch(ErrorException e) {}
}
void TimesSentence::execute() {
	try {
		Value *value = _count->evaluate();
		if(value == NULL)
			return;
		long count = value->getInt();
		delete value;
		for(int i = 0; i < count; i++) {
			try {
				_sentence->execute();
			} catch(ContinueException e) {
				;
			} catch(BreakException e) {
				return;
			}
		}
	} catch(ErrorException e) {}
}

void RepeatSentence::execute() {
	int step = 1;
	try {
		Value *v1 = _e1->evaluate();
		if(v1 == NULL)
			return;
		Value *v2 = _e2->evaluate();
		if(v2 == NULL) {
			delete v1;
			return;
		}
		int min = v1->getInt();
		int max = v2->getInt();
		delete v1;
		delete v2;
		if(_e3 != NULL) {
			Value *v3 = _e3->evaluate();
			if(v3 == NULL)
				return;
			step = v3->getInt();
			delete v3;
		} else {
			if(min > max)
				step = -1;
		}
		Variable *var = _varMap.getVariable(_vname);
		if(var == NULL) {
			printf("Error : Not Found Variable(name = %s, line = %d)\n", _vname,_line_number);
			return;
		}
		if(step > 0) {
			for(int i = min; i <= max; i+= step) {
				var->setData(i);
				try {
					_sentence->execute();
				} catch(ContinueException e) {
					;
				} catch(BreakException e) {
					return;
				}
			}
		} else {
			for(int i = min; i >= max; i+= step) {
				var->setData(i);
				try {
					_sentence->execute();
				} catch(ContinueException e) {
					;
				} catch(BreakException e) {
					return;
				}
			}
		}
	} catch(ErrorException e) {}
}

void ReturnSentence::execute(){
	Value *value = NULL;
	if (_e){
		value = _e->evaluate(); // ï]âø
	}
//	if (value == NULL)
//		value = new Integer(0);
	throw ReturnException(value);
}

void TreeSentence::execute() {
	Value *value = NULL;
	_e->printTree(0);

	bool print_ch = _e->isPrint();
	value = _e->evaluate();
	if(value != NULL) {
		if(print_ch == false && _system_mode == 0) {
			printf("result : ");
			value->print();
			printf("\n");
		}
		delete value;
	}
}
// others -----------------------------------

void VarSentence::append(char *name, Expression *e) {
	if(_top == NULL) {
		_top = _last = new VarNode(name, e);
	} else {
		_last->_next = new VarNode(name, e);
		_last = _last->_next;
	}
}

void VarSentence::clear() {
	VarNode *node, *next;
	for(node = _top; node; node = next) {
		next = node->_next;
		delete node;
	}
	_top = _last = NULL;
}

void BlockSentence::addSentence(Sentence *s) {
	if(!_top) {
		_top = _last = new SentenceNode(s, NULL);
		return;
	}
	_last->_next = new SentenceNode(s,NULL);
	_last = _last->_next;
}

void BlockSentence::clear() {
	SentenceNode *node, *next;
	for(node = _top; node ; node = next) {
		next = node->_next;
		delete node->_sentence;
		delete node;
	}
}

IfSentence::~IfSentence() {
#ifdef DEBUG
	printf("delete IfSentence\n");
#endif
	IfNode *node, *next;
	for(node = _top; node; node = next) {
		next = node->_next;
		delete node;
	}
}

void IfSentence::addIfSentence(IfNode *node) {
	if(_top ==NULL)
		_top = _last = node;
	else {
		_last->_next = node;
		_last = _last->_next;
	}
}

void InitialNode::evaluate() {
	if(_var_ch)
		_varMap.put(_name,NULL);
	if(_name) {
		Variable *var = _varMap.getVariable(_name);
		if(var == NULL) {
			return;
		}
		Value *v1 = _e->evaluate();
		var->setValue(v1);
	}
}

InitialVal::~InitialVal() {
#ifdef DEBUG
	printf("delete initial value\n");
#endif
	InitialNode *node, *next;
	for(node = _top; node; node = next) {
		next = node->_next;
		delete node;
	}
}

void InitialVal::setInitialVal(InitialNode *node) {
	if(_top == NULL) {
		_top = _last = node;
		return;
	}
	_last->_next = node;
	_last = _last->_next;
}

void InitialVal::evaluate() {
	InitialNode *node;
	for(node = _top; node; node = node->_next) {
		node->evaluate();
	}
}
