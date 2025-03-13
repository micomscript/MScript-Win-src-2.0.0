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
 * sentence.h
 */

#ifndef __SENTENCE_H__
#define __SENTENCE_H__

class ErrorException {
};

class BreakException {
};
class ContinueException {
};

class Sentence {
private:

public:
	Sentence() {}
	virtual ~Sentence() {}

	virtual void execute() = 0;  // é¿çs
	virtual bool printMode() { return false; }
};

// break
class Break : public Sentence {
public:
	Break() {}
	~Break() {}

	void execute() { throw BreakException(); }
};

// continue
class Continue : public Sentence {
public:
	Continue() {}
	~Continue() {}

	void execute() { throw ContinueException(); }
};

#ifdef T_ESP32
// reset
class Reset : public Sentence {
public:
	Reset() {}
	~Reset() {}

	void execute();
};
#endif

// éÆï∂
class FormulaSentence : public Sentence {
private:
	Expression	*_expr;

public:
	FormulaSentence(Expression *e) {
		_expr = e;
	}

	~FormulaSentence() {
#ifdef DEBUG
		printf("delete FormulaSentence\n");
#endif
		if(_expr) {
			delete _expr;
		}
	}

	void execute();
};

class VarNode {
public:
	char		*_name;
	Expression	*_express;
	VarNode		*_next;

	VarNode(char *name, Expression *e) {
		_name = new char[strlen(name)+1];
		strcpy(_name, name);
		_express = e;
		_next = NULL;
	}
	~VarNode() {
#ifdef DEBUG
		printf("delete VarNode(%s)\n", _name);
#endif
		delete _name;
		if(_express)
			delete _express;
	}

	void setExpress(Expression *e) {
		_express = e;
	}
};

class VarSentence : public Sentence {
private:
	VarNode	*_top;
	VarNode	*_last;

public:
	VarSentence() {
		_top = _last = NULL;
	}
	~VarSentence() {
#ifdef DEBUG
		printf("delete var-sentence\n");
#endif
		clear();
	}

	void append(char *name, Expression *e);
	void clear();
	void execute();
};

class SentenceNode {
public:
	Sentence		*_sentence;
	SentenceNode	*_next;

	SentenceNode(Sentence *s, SentenceNode *next) {
		_sentence = s;
		_next = next;
	}
};

// ï°ï∂
class BlockSentence : public Sentence {
private:
	SentenceNode	*_top;
	SentenceNode	*_last;

public:
	BlockSentence() {
		_top = _last = NULL;
	}

	~BlockSentence() {
#ifdef DEBUG
		printf("delete { }\n");
#endif
		clear();
	}

	void addSentence(Sentence *s);
	void execute();
	void clear();
};

// ifï∂
class IfNode {
public:
	Expression	*_logic;
	Sentence	*_sentence;
	IfNode		*_next;

	IfNode(Expression *logic, Sentence *s) {
		_logic = logic;
		_sentence = s;
		_next = NULL;
	}
	~IfNode() {
#ifdef DEBUG
		printf("delete ifnode\n");
#endif
		if(_logic)
			delete _logic;
		if(_sentence)
			delete _sentence;
	}
};

class IfSentence : public Sentence {
private:
	IfNode *_top;
	IfNode *_last;

public:
	IfSentence() {
		_top = _last = NULL;
	}
	~IfSentence();

	void addIfSentence(IfNode *node);
	void execute();
	bool printMode() { return true; }
};

// whileï∂
class WhileSentence : public Sentence {
private:
	Expression	*_logic;
	Sentence	*_sentence;
	bool		_do_ch;			// do-whileï∂ÇÃèÍçáÅAtrue

public:
	WhileSentence(Expression *logic, Sentence *s, bool ch) {
		_logic = logic;
		_sentence = s;
		_do_ch = ch;
	}
	~WhileSentence() {
#ifdef DEBUG
		printf("delete while\n");
#endif
		delete _logic;
		delete _sentence;
	}

	void execute();
	bool printMode() { return true; }
};

// forï∂
class InitialNode {
public:
	bool		_var_ch;
	char 		*_name;
	Expression	*_e;
	InitialNode	*_next;

	InitialNode(bool ch,char *name,Expression *e) {
		_var_ch = ch;
		_name = new char[strlen(name)+1];
		strcpy(_name, name);
		_e = e;
		_next = NULL;
	}
	~InitialNode() {
#ifdef DEBUG
		printf("delete InitialNode\n");
#endif
		delete _name;
		delete _e;
	}
	void evaluate();
};

class InitialVal {
private:
	InitialNode *_top;
	InitialNode *_last;

public:
	InitialVal() {
		_top = _last = NULL;
	}
	~InitialVal();

	void setInitialVal(InitialNode *node);
	void evaluate();
};

class ForSentence : public Sentence {
private:
	InitialVal 	*_initial_val;
	Expression	*_logic;
	Expression	*_e2;
	Sentence	*_sentence;

public:
	ForSentence(InitialVal *initial,Expression *logic,
				Expression *e2,Sentence *s) {
		_initial_val = initial;
		_logic = logic;
		_e2 = e2;
		_sentence = s;
	}
	~ForSentence() {
#ifdef DEBUG
		printf("delete for\n");
#endif
		delete _initial_val;
		delete _logic;
		if(_e2)
			delete _e2;
		delete _sentence;
	}

	void execute();
	bool printMode() { return true; }
};

class ForeachSentence : public Sentence {
private:
	char		*_vname;
	char		*_listname;
	Sentence	*_sentence;

public:
	ForeachSentence(char *vname,char *listname,Sentence *s) {
		_vname = vname;
		_listname = listname;
		_sentence = s;
	}
	~ForeachSentence() {
#ifdef DEBUG
		printf("delete foreach sentence()\n");
#endif
		delete _vname;
		delete _listname;
		delete _sentence;
	}

	void execute();
	bool printMode() { return true; }
};

// timesï∂
class TimesSentence : public Sentence {
private:
	Expression	*_count;
	Sentence	*_sentence;
public:
	TimesSentence(Expression *cnt, Sentence *s) {
		_count = cnt;
		_sentence = s;
	}

	~TimesSentence() {
#ifdef DEBUG
		printf("delete TimesSentence\n");
#endif
		delete _count;
		delete _sentence;
	}

	void execute();
	bool printMode() { return true; }
};

// repeatï∂
class RepeatSentence : public Sentence {
private:
	char		*_vname;
	Expression	*_e1;
	Expression	*_e2;
	Expression	*_e3;
	Sentence	*_sentence;

public:
	RepeatSentence(char *vname, Expression *e1, Expression *e2,
				   Expression *e3, Sentence *sentence)			{
		_vname = new char[strlen(vname)+1];
		strcpy(_vname, vname);
		_e1 = e1;
		_e2 = e2;
		_e3 = e3;
		_sentence = sentence;
	}
	~RepeatSentence() {
#ifdef DEBUG
		printf("delete RepeatSentence\n");
#endif
		delete _vname;
		delete _e1;
		delete _e2;
		if(_e3)
			delete _e3;
		delete _sentence;
	}

	void execute();
	bool printMode() { return true; }
};

// returnï∂
class ReturnSentence : public Sentence{
private:
	Expression	*_e; // éÆ

public:
	ReturnSentence(){
		_e = NULL;
	}

	~ReturnSentence(){
#ifdef DEBUG
		printf("delete returnï∂\n");
#endif
		delete _e;
	}

	void setExpression(Expression *e){
		delete _e;
		_e = e;
	}

	void	execute();			// ï∂ÇÃé¿çs
};

class TreeSentence : public Sentence {
private:
	Expression *_e;

public:
	TreeSentence(Expression *e) {
		_e = e;
	}
	~TreeSentence() {
#ifdef DEBUG
		printf("delete TreeSentence\n");
#endif
		delete _e;
	}

	void execute();
};

#endif;
