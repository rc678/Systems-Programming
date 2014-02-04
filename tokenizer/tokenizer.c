/*
 * tokenizer.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */
struct TokenizerT_ {/*implemented as a Linked List*/
	char* token;
	struct TokenizerT_* nextToken;
	int viewed;
};

typedef struct TokenizerT_ TokenizerT;

/* prototypes of the functions declared.*/
char* substring(char*, int, int); 
char* checkEscape(char*, int, int, char*, int);
int checkSep(char*, int); 
/*
 * TKCreate creates a new TokenizerT object for a given set of separator
 * characters (given as a string) and a token stream (given as a string).
 * TKCreate should copy the two arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate(char *separators, char *ts) {
	TokenizerT* head;
	TokenizerT* end;
	TokenizerT* temp;
	int currIndex, j;
	int strLen, sepLen;
	char* token;

	strLen = strlen(ts);
	sepLen = strlen(separators);

	char string[strLen];
	char sep[sepLen];
	int isNewToken = 1;
	int startIndex = 0;
	int endIndex = 0; 
	int isDelim = 0;
	head = NULL;
	end = NULL;

	strcpy(string, ts);
	strcpy(sep, separators);

	/*1st for loop traverses through the given string character by character. 
	 * 2nd for loop checks to see if the character is a separator character. 
	 */
	for(currIndex = 0; currIndex < strLen; currIndex++)
	{
		for(j = 0; j < sepLen; j++)
		{

			if(string[currIndex] == sep[j])/*checks to see if current char is a separator*/
			{
				isDelim = 1;
				break;
			}	
		}/*end of inner for */

		/* if the current character is not a separator and it is the start of a new token*/
		if((isNewToken == 1) && (isDelim == 0))
		{
			startIndex = currIndex;
			isNewToken = 0;
			continue;
		}

		/*finds the index of the last character of the new token*/
		if(((isDelim == 1) && (isNewToken == 0)) ||(((isDelim == 0) && (isNewToken == 0) && (currIndex==strLen-1))))
		{
			if((currIndex==strLen-1) && (isDelim == 0))
			{
				endIndex = currIndex;
			}else{
				endIndex = currIndex - 2;
			}
			isNewToken = 1;
			isDelim = 0;
			token = substring(string, startIndex, endIndex);
			if(head == NULL)/*Creates the head of the linked list*/
			{
				head = (TokenizerT*)malloc(sizeof(TokenizerT));
				head->token = token;
				head->nextToken = NULL;
				head->viewed = 0;
				temp = head; 
				end = head;
			}else{/*adds to the end of the linked list*/
				temp = (TokenizerT*) malloc(sizeof(TokenizerT));
				temp->token = token;
				temp->nextToken = NULL;
				temp->viewed = 0; 
				end->nextToken = temp;
				end = end->nextToken;
			}
		}

		/* if you have multiple separators in the beginning of the string*/
		if(((isDelim == 1) && (isNewToken == 1)) || ((isDelim == 0) && (isNewToken == 0)))
		{
			isDelim = 0;

		}

	}/*end of outer for*/
	if(startIndex == endIndex)
	{

		head = (TokenizerT*)malloc(sizeof(TokenizerT));
		head->token = ts;
		head->nextToken = NULL;
		head->viewed = 0;
		temp = head; 
		end = head;		
	}

	return head;
}

/*CheckEscape accepts a character string, the index at which a backslash occurs, a string of separator
 * characters, and the lengths of the two strings. It then checks if the backlash is part of an escape character
 *  or if it is simply a single backslash.
 * In the first case, it returns a string of proper length that has the hex form of the escape character in
 * question instead of the escape character itself. In the latter case, it returns a string of proper length
 * in which the backslash is removed.
 */
char* checkEscape(char* string, int currIndex, int strLen, char* sep, int sepLen)
{
	char str[strLen];
	char letter;
	char* res;
	char* hex;
	int i, size;
	char copy[7];
	char result[strLen + 5];
	char removeBackslash[strLen];
	int j, resultIndex;
	int isSep = 0;
	strcpy(str, string);
	letter = str[currIndex+1];

	switch(letter)
	{
		case 'n':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x0a]";
			strcpy(copy, hex);
			break;
		case 't':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x09]";
			strcpy(copy, hex);
			break;
		case 'v':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x0b]";
			strcpy(copy, hex);
			break;
		case 'b':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x08]";
			strcpy(copy, hex);
			break;
		case 'r':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x0d]";
			strcpy(copy, hex);
			break;
		case 'f':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x0c]";
			strcpy(copy, hex);
			break;
		case 'a':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x07]";
			strcpy(copy, hex);
			break;
		case '\\':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x5c]";
			strcpy(copy, hex);
			break;
		case '"':
			isSep = checkSep(sep, sepLen);
			if (isSep == 1){
				return string;
			}
			hex = "[0x22]";
			strcpy(copy, hex);
			break;
		default:
			resultIndex = 0;
			for (i = 0; i < strLen; i++)
			{
				if (i == currIndex){
					continue;
				}

				removeBackslash[resultIndex] = str[i];
				resultIndex++;
			}
			removeBackslash[strLen-1] = '\0';
			size = strLen;
			res = malloc(size);
			strcpy(res, removeBackslash);
			return res;
	}/*end of switch*/

	resultIndex = 0;
	for(i=0; i < strLen; i++)
	{
		if(i == currIndex)
		{
			for(j=0; j < 6; j++)
			{
				result[resultIndex] = copy[j];
				resultIndex++;
			}
			i = i + 1;
			continue;		
		}

		result[resultIndex] = str[i];
		resultIndex++; 
	}
	result[strLen+4] = '\0'; 	
	for(i = 0; i < strlen(result); i++)
	{
		printf("char at %d is %c in checkSpace\n", i, result[i]);	
	}
	size = strLen + 5;
	res = malloc(size);
	strcpy(res, result);
	/*	printf("res is %s\n", res);*/
	return res; 

}/*end of checkEscape method*/

/*If the argument contains a '\' character, checkSep checks if this is a backslash character or
 *  * part of an escape character*/
int checkSep(char* sep, int sepLen)
{
	int i;
	char seps[sepLen];
	strcpy(seps, sep);

	for(i=0; i < sepLen; i++)
	{
		if(sep[i] == '\\')
		{
			switch(sep[i+1])
			{
				case 'n':
					return 1;
				case 't':
					return 1; 
				case 'v':
					return 1;
				case 'b':
					return 1; 
				case 'r':
					return 1;
				case 'f':
					return 1; 
				case 'a':
					return 1;
				case '\\':
					return 1;
				case '"':
					return 1; 
			}
		}
	}	
	return 0; 
}

/*
 * 
 * Takes in the input string, index of the 1st character of the token in the input string, and 
 * index of the last character of the token in the input string. Uses these variables to dynamically allocate 
 * the token string. The function returns a pointer to the dynamically allocated string. 
 */
char* substring(char* string, int start, int end)
{
	int strLen = strlen(string);
	int size = (end - start) + 1;
	char result[size];
	int i, curr;
	char str[strLen];
	char* res = malloc(size);

	strcpy(str, string);

	curr = 0;
	/*for loop goes to the start index of the new token in the given string and copies the characters
	 * into a new array until the end index in the input string. These characters are a token.  
	 */
	for(i = start; i <= end+1; i++)
	{
		result[curr] = str[i];
		curr++; 
	}/*end of for*/
	curr++; 
	result[curr] = '\0';
	strcpy(res, result);
	return res; 	

}/*end of method*/


/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy(TokenizerT *tk) {

	TokenizerT* curr;

	while(tk != NULL){
		curr = tk;
		tk = tk->nextToken;
		free(curr);
	}

}/*end of TKDestory method*/

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken(TokenizerT *tk) {

	TokenizerT *temp = tk;
	char* res; 

	if (temp == NULL)
		return 0;

	if(temp->viewed == 0)
	{
		res = temp->token;
		temp->viewed = 1;
		return res; 
	}

	return 0;
}


/*
 * main will have two string arguments (in argv[1] and argv[2]).
 * The first string contains the separator characters.
 * The second string contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {

	if(argc < 3){
		printf("Error: Too few arguments\n");
		return 0;
	}
	if(argc > 3){
		printf("Error: Too many arguments\n");
		return 0;
	}

	int strLen = strlen(argv[2]);
	int sepLen = strlen(argv[1]);
	char string[strLen];
	char sep[sepLen];
	int i;
	int escapePresent = 0;
	int finalSize;
	char* finalString;
	char* token;
	TokenizerT *tokenizer;
	strcpy(string, argv[2]);
	strcpy(sep, argv[1]);

	for(i=0; i < strLen; i++)
	{
		if(string[i] == '\\')
		{
			finalString = checkEscape(string, i, strLen, sep, sepLen);
			escapePresent = 1;
		}
	}

	if(escapePresent == 1)
	{
		finalSize = strlen(finalString);
		char result[finalSize];
		strcpy(result, finalString);
		tokenizer = TKCreate(sep, result);
	}else{
		tokenizer = TKCreate(sep, string);
	}


	while (tokenizer != NULL){
		token = TKGetNextToken(tokenizer);
		printf("%s\n", token);
		tokenizer = tokenizer->nextToken;
	}

	TKDestroy(tokenizer);

	return 0;
}


