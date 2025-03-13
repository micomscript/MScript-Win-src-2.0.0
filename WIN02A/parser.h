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
 *	parser.h
 *	�\�����
 */

#ifndef __PARSER_H__
#define __PARSER_H__

class ExitException {
};

class Parser {
private:
	Tokenizer _tokenizer;
	Token     *_token;
	int		  _code;
	int		  _keyCode;

public:
	Parser() {
		_token 	= NULL;
		_code 	= T_EOL;
	}

	int			setScript(char *s);

	int  		nextToken();   					//�������ǂݎ��A�����ʃR�[�h��Ԃ�
	void 		putBack();						//�����߂�

	void		parse();						//�\�����

#ifdef T_ESP32
	Sentence	*file_list();
	Sentence	*save_file();
	Sentence	*show_file();
	Sentence	*load_file();
	Sentence	*remove_file();
	void		 execute_file(char *path);
#endif

	Sentence	*sentence();					// ��
	Sentence	*tree_sentence();				// tree
	Sentence	*return_sentence();				// return
	Sentence	*block_sentence();				// ����
	Sentence	*var_sentence();				// var��
	Sentence	*func_sentence();				// ���[�U��`�֐��̍쐬
	Sentence	*undef_sentence();				// undef
	Sentence	*help_sentence();				// help
	Sentence	*if_sentence();					// if��
	Sentence	*while_sentence(bool ch);		// while��
	Sentence	*for_sentence();				// for��
	Sentence	*foreach_sentence();			// foreach��
	Sentence	*times_sentence();				// times��
	Sentence	*repeat_sentence();				// repeat��
	Sentence	*print_sentence();				// print�錾
	Sentence	*express_sentence();			// ����

	Expression	*expression();					// ��
	Expression	*logic_or(Expression *e);		// �_���a
	Expression	*logic_and(Expression *e);		// �_����
	Expression	*bit_or(Expression *e);			// �r�b�g�a
	Expression	*bit_exor(Expression *e);		// �r���I�r�b�g�a
	Expression	*bit_and(Expression *e);		// �r�b�g��
	Expression	*equal_express(Expression *e);	// ������
	Expression	*relation_express(Expression *e);// �֌W��
	Expression	*shift_express(Expression *e);	// �V�t�g��
	Expression	*add_express(Expression *e);	// ������
	Expression	*term(Expression *e);			// ��
	Expression	*factor(Expression *e);			// ���q
	Expression	*particle();					// ���q
	Expression	*atom();						// ���q
};

#endif
