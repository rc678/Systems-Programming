/*
 * Tokenizer.c
 *   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "uthash.h" /*Copyright (c) 2005-2014, Troy D. Hanson  http://troydhanson.github.com/uthash/ All rights reserved.*/

/*
 *  *  *  *  *  * Tokenizer type.  You need to fill in the type as part of your implementation.
 *   *   *   *   *   */

struct TokenizerT_ {
	char* copied_string;		
	char* current_position;
};

typedef struct TokenizerT_ TokenizerT;

/*
 *  *  *  *  *  * TKCreate creates a new TokenizerT object for a given set of separator
 *   *   *   *   *   * characters (given as a string) and a token stream (given as a string).
 *    *    *    *    *    * 
 *     *     *     *     *     * TKCreate should copy the two arguments so that it is not dependent on
 *      *      *      *      *      * them staying immutable after returning.  (In the future, this may change
 *       *       *       *       *       * to increase efficiency.)
 *        *        *        *        *        *
 *         *         *         *         *         * If the function succeeds, it returns a non-NULL TokenizerT.
 *          *          *          *          *          * Else it returns NULL.
 *           *           *           *           *           *
 *            *            *            *            *            * You need to fill in this function as part of your implementation.
 *             *             *             *             *             */

TokenizerT *TKCreate(char *ts) {

	/*
 * 	 * Description: creates a new tokenizer struct from the token stream and delimiters
 * 	 	 * arameters: set of delimiters, token stream
 * 	 	 	 * Modifies: nothing
 * 	 	 	 	 * Returns: a pointer to a tokenizer struct on success, a null pointer on failure
 * 	 	 	 	 	 * */

	if(ts == NULL){
		return NULL;
	}

	TokenizerT* tokenizer = (TokenizerT*)malloc(sizeof(TokenizerT));

	if(tokenizer == NULL){
		return NULL;
	}

	tokenizer->copied_string = ts;
	tokenizer->current_position = tokenizer->copied_string;

	return tokenizer;
}

/*
 *  *  *  *  *  * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 *   *   *   *   *   * allocated memory that is part of the object being destroyed.
 *    *    *    *    *    *
 *     *     *     *     *     * You need to fill in this function as part of your implementation.
 *      *      *      *      *      */

void TKDestroy(TokenizerT *tk) {	

	/*
 * 	 * Description: destroys tokenizer struct and deallocates all memory
 * 	 	 * Parameters: tokenizer to be destroyed
 * 	 	 	 * Modifies: tokenizer struct: deallocates memory
 * 	 	 	 	 * Returns: nothing 
 * 	 	 	 	 	 * */

	free(tk->copied_string);
	free(tk);

	return;
}

/*This function reads a string token and converts any capital letter to lower case*/
char* toLowerCase(char *token){

	char *ch;

	for (ch = token; *ch != '\0'; ch++){


		if (*ch >= 65 && *ch <= 90){
			*ch = *ch + 32;
		}
	}

	return token;
}

/*This function returns 0 if the input character is a letter or a number, and returns 1 otherwise*/
char is_delimiter(char character) {

	/*
 * 	 * Description: determines if a particular character is a member of the set of delimiters
 * 	 	 * Parameters: character to be compared, string of delimiters
 * 	 	 	 * Modifies: Nothing
 * 	 	 	 	 * Returns: 1 if character is a delimiter, 0 if it is not
 * 	 	 	 	 	 *  */

	int ch = character;

	if (isalnum(ch)){
		return 0;
	} else {
		return 1;
	}
}

/*
 *  *  *  *  *  * TKGetNextToken returns the next token from the token stream as a
 *   *   *   *   *   * character string.  Space for the returned token should be dynamically
 *    *    *    *    *    * allocated.  The caller is responsible for freeing the space once it is
 *     *     *     *     *     * no longer needed.
 *      *      *      *      *      *
 *       *       *       *       *       * If the function succeeds, it returns a C string (delimited by '\0')
 *        *        *        *        *        * containing the token.  Else it returns 0.
 *         *         *         *         *         *
 *          *          *          *          *          * You need to fill in this function as part of your implementation.
 *           *           *           *           *           */

char *TKGetNextToken(TokenizerT *tk) {

	/*
 * 	 * Description: returns the next token from the token stream specified within the tokenizer
 * 	 	 * Parameters: tokenizer from which to extract token
 * 	 	 	 * Modifies: tokenizer->current_position: identifies starting point of next token; creates a new string with 
 * 	 	 	 	 * Returns: token extracted as a char* on success, null on failure/end of string;
 * 	 	 	 	 	 * */

	char* token = NULL;
	char* token_start = NULL;

	while(tk->current_position - tk->copied_string < strlen(tk->copied_string)) {
		if(!is_delimiter(*tk->current_position)) {

			token_start = tk->current_position;
			break;
		}
		tk->current_position++;
	}

	if(token_start == NULL) {
		return NULL;
	}

	while(tk->current_position - tk->copied_string < strlen(tk->copied_string)) {
		if(is_delimiter(*tk->current_position)) {
			break;
		}
		tk->current_position++;
	}	

	token = (char*)malloc(sizeof(char) * (tk->current_position - token_start + 1));
	strncpy(token, token_start, tk->current_position - token_start);
	token[(tk->current_position - token_start)] = '\0';
	token =	toLowerCase(token);

	return token;
}

