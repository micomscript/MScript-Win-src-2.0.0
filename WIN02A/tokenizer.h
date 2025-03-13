////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Begin license text.

Copyright 2023- MicomScript Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the �gSoftware�h), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED �gAS IS�h, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.

End license text.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 *	tokenizer.h
 */

#ifndef __TOKENIZER_H__

#define __TOKENIZER_H__


class KeyWords {
public:
	char	*_name;
	Token	*_token;
};

Token *searchKeyWord(char *name);
char  *searchKeyName(int KeyCode);
Token *searchResource(char *name);

class Tokenizer {
private:
#ifdef T_WINDOWS
	FILE	*_file;
#endif
#ifdef T_ESP32
	File	_file;
#endif

	char  	*_nextChar; 				// ���̕����ʒu
	bool	_empty;						// ���̓o�b�t�@����
	char	_inBuffer[IN_BUFF_SIZE];	// ���̓o�b�t�@
	Token	*_stack[TOKEN_STACK_SIZE];
	int		_count;

public:
	Tokenizer() {
#ifdef T_WINDOWS
		_file = NULL;
#endif
		_nextChar = NULL;
		_empty    = true;
		_count 	  = 0;
	}

	Token	*getToken();			// �����؂�o��
	void	 putBack(Token *token);	// �����߂�
	void	 readLine();			// ��s�ǂ�
	int		 setScript(char *s);

#ifdef T_ESP32
	void  closeScript();
#endif
};

#endif
