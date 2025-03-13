////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Begin license text.

Copyright 2023- MicomScript Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the gSoftwareh), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED gAS ISh, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.

End license text.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * varmap.h
 */
#ifndef __VARMAP_H__
#define __VARMAP_H__

#define VMAP_STACK_SIZE		103

class VarElement{
private:
	Variable		*_var;
	VarElement		*_next;

public:
	 VarElement(Variable *var, VarElement *next){
		_var = var;
		_next = next;
	}

	~VarElement(){
#ifdef DEBUG
		printf("delete VarElement\n");
#endif
		delete _var;
	}

	char *getName(){
		return _var->getName();
	}

	Variable *getVariable(){
		return _var;
	}

	VarElement *getNext(){
		return _next;
	}

	void setNext(VarElement *next){
		_next = next;
	}

	void print(){
		_var->print();
		printf("\n");
	}
};

class VarMapLayer {
private:
	VarElement	*_top;
	VarElement	*_last;
	double		 _dummy;


public:
			 VarMapLayer();
	virtual	~VarMapLayer();
	VarElement	*lookFor(const char *name);
	void	 put(const char *name, Value *value);
	bool	 exist(const char *name);
	void	 removeVar(const char *name);
	void	 clear();
	void	 printVars();
	Variable*getVariable(const char *name);
};

class VarMapStack{
private:
	VarMapLayer	*_vmap[VMAP_STACK_SIZE];
	int			 _curLayer;


public:
	VarMapStack(){
		_curLayer = 0;
		_vmap[0] = new VarMapLayer();
	}
	~VarMapStack(){
		clearAll();
		delete _vmap[0];
	}

	void	 put(const char *name, Value *value);
	bool	 exist(const char *name);
	void	 removeVar(const char *name);
	void	 clearLayer();
	void	 clearAll();
	void	 printVars();
	Variable*getVariable(const char *name);
	void	 stackUp();
	void	 stackDown();
};

#endif
