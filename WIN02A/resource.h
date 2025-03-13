// ÉäÉ\Å[ÉX -------------------------------------------------------

class Resource{
private:

public:

	virtual Value	*evaluate() = 0;
	virtual bool	 eval() = 0;
	virtual void	 print() = 0;
	virtual void	 printTree() = 0;
	virtual Value	*methodCall(char *name, List *params) = 0;
};

class WiFiRes : public Resource{
private:

public:
	WiFiRes(){
	}

	Value	*evaluate();
	bool	 eval();
	void	 print();
	void	 printTree();
	Value	*methodCall(char *name, List *params);

	void	begin(List *params);
	int		status();

};

