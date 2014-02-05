#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 *  * Tokenizer type.  You need to fill in the type as part of your implementation.
 *   */

struct TokenizerT_ {
	char* token;
	struct TokenizerT_* nextToken;
	int viewed;
};

typedef struct TokenizerT_ TokenizerT;

/* function prototypes of the functions I declared.*/
char* substring(char*, int, int); 
void print(TokenizerT*);  
int checkEscape(char*, int, int, char*, int);
char* fixEscape(char*, int, char*, int, int);
int checkSep(char*, int, char);


/*
 *  * TKCreate creates a new TokenizerT object for a given set of separator
 *   * characters (given as a string) and a token stream (given as a string).
 *    * 
 *     * TKCreate should copy the two arguments so that it is not dependent on
 *      * them staying immutable after returning.  (In the future, this may change
 *       * to increase efficiency.)
 *        *
 *         * If the function succeeds, it returns a non-NULL TokenizerT.
 *          * Else it returns NULL.
 *           *
 *            * You need to fill in this function as part of your implementation.
 *             */

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
	 * 	 * 2nd for loop checks to see if the character is a separator character. 
	 * 	 	 */
	for(j = 0; j < strLen; j++)
	{
		printf("STRING IN CREATE TOKEN %c\n", string[j]);
	}

	for(currIndex = 0; currIndex < strLen; currIndex++)
	{
		for(j = 0; j < sepLen; j++)
		{

			/*	printf("sep is %c\n", sep[j]);	*/
			if(string[currIndex] == sep[j])/*checks to see if current char is a separator*/
			{
				if (j>0 && sep[j-1] == '\\' && currIndex>0 && string[currIndex-1] != '\\'){
                                        continue;
                                }
                                if (sep[j] == '\\' && j < sepLen-1){
                                        if ((sep[j+1] == 'n' || sep[j+1] == 't' || sep[j+1] == 'v' || sep[j+1] == 'b' ||
                                        sep[j+1] == 'r' || sep[j+1] == 'f' || sep[j+1] == 'a' || sep[j+1] == '\\' || 
                                        sep[j+1] == '"') && string[currIndex+1] == sep[j+1]){
                                                j++;
                                        }
                                }
                                
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
			/*	printf("token is %s\n", token);*/
			if(head == NULL)
			{
				head = (TokenizerT*)malloc(sizeof(TokenizerT));
				head->token = token;
				head->nextToken = NULL;
				head->viewed = 0;
				temp = head; 
				end = head;
			}else{
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

	printf("TESTING %d", isDelim);
	for(j=0; j < sepLen; j++)
	{
		if(sep[j] == string[currIndex-2] && isDelim == 0)
		{
			token = substring(string, strLen-1, strLen-1);
			temp = (TokenizerT*) malloc(sizeof(TokenizerT));
			temp->token = token;
			temp->nextToken = NULL;
			temp->viewed = 0; 
			end->nextToken = temp;
			end = end->nextToken;
		}
	}

	return head;
}

int checkEscape(char* string, int currIndex, int strLen, char* sep, int sepLen)
{
	char letter;
	int isSep = 0;
	int sizeChange = 0;
	char str[strLen];
	strcpy(str, string);
	letter = str[currIndex+1];

	switch(letter)
	{   
		case 'n':
			isSep = checkSep(sep, sepLen, 'n');
			if (isSep == 1){ 
				return sizeChange;
			}   
			sizeChange = 4;
			return sizeChange;
		case 't':
			isSep = checkSep(sep, sepLen, 't');
			if (isSep == 1){ 
				return sizeChange;
			}
			sizeChange = 4;
			return sizeChange;
		case 'v':
			isSep = checkSep(sep, sepLen, '4');
			if (isSep == 1){
				return sizeChange;
			}
			sizeChange = 4;
			return sizeChange;
		case 'b':
			isSep = checkSep(sep, sepLen, 'b');
			if (isSep == 1){
				return sizeChange;
			}
			sizeChange = 4;
			return sizeChange;
		case 'r':
			isSep = checkSep(sep, sepLen, 'r');
			if (isSep == 1){
				return sizeChange;
			}
			sizeChange = 4;
			return sizeChange;
		case 'f':
			isSep = checkSep(sep, sepLen, 'f');
			if (isSep == 1){
				return sizeChange;
			}
			sizeChange = 4;
			return sizeChange;
		case 'a':
			isSep = checkSep(sep, sepLen, 'a');
			if (isSep == 1){
				return sizeChange;
			}
			sizeChange = 4;
			return sizeChange;
		case '\\':
			isSep = checkSep(sep, sepLen, '\\');
			if (isSep == 1){
				return sizeChange;
			}
			sizeChange = 4;
			return sizeChange;
		case '"':
			isSep = checkSep(sep, sepLen, '"');
			if (isSep == 1){
				return sizeChange;
			}
			sizeChange = 4;
			return sizeChange;
		default:
			sizeChange = -1;
			return sizeChange;
	}
}

/*CheckEscape accepts a character string, the index at which a backslash occurs, a string of separator
 * characters, and the lengths of the two strings. It then checks if the backlash is part of an escape character
 * or if it is simply a single backslash.
 *
 * In the first case, it returns a string of proper length that has the hex form of the escape character in
 * question instead of the escape character itself. In the latter case, it returns a string of proper length
 * in which the backslash is removed.
 * */
char* fixEscape(char* string, int strLen, char* sep, int sepLen, int size)
{
	char str[strLen];
	char letter;
	char* res;
	char* hex;  
	int i;
	char copy[7]; 
	char result[size];
	int j;
	int resultIndex = 0;
	int isSep = 0;  
	strcpy(str, string);

	for (i=0; i<strLen; i++)
	{
		if (str[i] == '\\' && i == 0){
                        continue;
                }

		if (str[i] == '\\' && i > 0 && str[i-1] != '\\')
		{

			letter = str[i+1];
			printf("CURR INDEX IS %d\n", i);
			/*	for(i = 0; i < strLen; i++)
			 *		{
			 *				printf("string in checkEscape is %c\n", str[i]);
			 *					}*/	
			switch(letter)
			{
				case 'n':
					isSep = checkSep(sep, sepLen, 'n');
					if (isSep == 1){
						result[resultIndex] = str[i];
                				resultIndex++;
						isSep = 0;
						continue;
					}
					hex = "[0x0a]";
					strcpy(copy, hex);
					break; 
				case 't':
					isSep = checkSep(sep, sepLen, 't');
					if (isSep == 1){ 
						result[resultIndex] = str[i];
                                                resultIndex++;
                                                isSep = 0;
                                                continue;
					}   
					hex = "[0x09]";
					strcpy(copy, hex);
					break;
				case 'v':
					isSep = checkSep(sep, sepLen, 'v');
					if (isSep == 1){ 
						result[resultIndex] = str[i];
                                                resultIndex++;
                                                isSep = 0;
                                                continue;
					}   
					hex = "[0x0b]";
					strcpy(copy, hex);
					break;
				case 'b':
					isSep = checkSep(sep, sepLen, 'b');
					if (isSep == 1){ 
						result[resultIndex] = str[i];
                                                resultIndex++;
                                                isSep = 0;
                                                continue;
					}   
					hex = "[0x08]";
					strcpy(copy, hex);
					break;
				case 'r':
					isSep = checkSep(sep, sepLen, 'r');
					if (isSep == 1){ 
						result[resultIndex] = str[i];
                                                resultIndex++;
                                                isSep = 0;
                                                continue;
					}   
					hex = "[0x0d]";
					strcpy(copy, hex);
					break;
				case 'f':
					isSep = checkSep(sep, sepLen, 'f');
					if (isSep == 1){ 
						result[resultIndex] = str[i];
                                                resultIndex++;
                                                isSep = 0;
                                                continue;
					}   
					hex = "[0x0c]";
					strcpy(copy, hex);
					break;
				case 'a':
					isSep = checkSep(sep, sepLen, 'a');
					if (isSep == 1){ 
						result[resultIndex] = str[i];
                                                resultIndex++;
                                                isSep = 0;
                                                continue;
					}   
					hex = "[0x07]";
					strcpy(copy, hex);
					break;
				case '\\':
					isSep = checkSep(sep, sepLen, '\\');
					if (isSep == 1){ 
						result[resultIndex] = str[i];
                                                resultIndex++;
                                                isSep = 0;
                                                continue;
					}
					hex = "[0x5c]";
					strcpy(copy, hex);
					break;
				case '"':
					isSep = checkSep(sep, sepLen, '"');
					if (isSep == 1){ 
						result[resultIndex] = str[i];
                                                resultIndex++;
                                                isSep = 0;
                                                continue;
					}   
					hex = "[0x22]";
					strcpy(copy, hex);
					break;
				default:
					continue;			
			}/*end of switch*/

			for (j = 0; j < 6; j++)
			{
				result[resultIndex] = copy[j];
				resultIndex++;
			}
			
			i++;
			continue;
		}/*end of if*/

		result[resultIndex] = str[i];
		resultIndex++;
	}/*end of for*/
	/*for(i=0; i < 6; i++)
	 * 	{
	 * 			printf("copy is %c\n", copy[i]);
	 * 				}*/

	result[size-1] = '\0';

	for(i = 0; i < size; i++){
		printf("%c\n", result[i]);
	}
	
	res = malloc(size);
	strcpy(res,result);
	return res;

}/*end of fixEscape method*/

/*If the argument contains a '\' character, checkSep checks if this is a backslash character or
 * part of an escape character*/
int checkSep(char* sep, int sepLen, char c)
{
	int i;
	char seps[sepLen];
	strcpy(seps, sep);

	for(i=0; i < sepLen; i++)
	{
		if(sep[i] == '\\' && i < sepLen-1 &&  sep[i+1] == c)
		{
			return 1;
		}
	}	
	return 0; 
}

/*
 *
 * * Takes in the input string, index of the 1st character of the token in the input string, and 
 * * index of the last character of the token in the input string. Uses these variables to dynamically allocate 
 * * the token string. The function returns a pointer to the dynamically allocated string. 
 * */
char* substring(char* string, int start, int end)
{
	int strLen = strlen(string);
	int size = (end - start) + 1;
	char result[size];
	int i, curr;
	char str[strLen];
	char* res = malloc(size);

	strcpy(str, string);

	/*printf("str is %s\n", str);
	 * 	printf(" start is %d end is %d size is %d\n ", start, end, size);*/

	curr = 0;
	/*for loop goes to the start index of the new token in the given string and copies the characters
	 * 	* into a new array until the end index in the input string. These characters are a token.  
	 * 		*/
	for(i = start; i <= end+1; i++)
	{
		result[curr] = str[i];
		curr++; 
	}/*end of for*/
	curr++; 
	result[curr] = '\0';
	strcpy(res, result);
	for(i=0; i < strlen(result); i++)
	{
		printf("chars in result array in substring are %c\n", result[i]);
	}
	printf("substring is %s\n", res);
	return res; 	

}/*end of method*/

/*
 * * Takes in a reference to the head of the built linked list and prints all the tokens. 
 * */
void print(TokenizerT* head)
{
	printf("in print\n");
	TokenizerT* ptr = head; 

	while(ptr != NULL)
	{
		printf("%s\n", ptr->token);
		ptr = ptr->nextToken;
	}
}/*end of method*/


/*
 *  * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 *   * allocated memory that is part of the object being destroyed.
 *    *
 *     * You need to fill in this function as part of your implementation.
 *      */

void TKDestroy(TokenizerT *tk) {

	TokenizerT* curr;

	while(tk != NULL){
		curr = tk;
		tk = tk->nextToken;
		free(curr);
	}

}

/*
 *  * TKGetNextToken returns the next token from the token stream as a
 *   * character string.  Space for the returned token should be dynamically
 *    * allocated.  The caller is responsible for freeing the space once it is
 *     * no longer needed.
 *      *
 *       * If the function succeeds, it returns a C string (delimited by '\0')
 *        * containing the token.  Else it returns 0.
 *         *
 *          * You need to fill in this function as part of your implementation.
 *           */

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
 *  * main will have two string arguments (in argv[1] and argv[2]).
 *   * The first string contains the separator characters.
 *    * The second string contains the tokens.
 *     * Print out the tokens in the second string in left-to-right order.
 *      * Each token should be printed on a separate line.
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
	/*int escapePresent = 0;*/
	int finalSize = strLen;
	char* finalString;
	char* token;
	TokenizerT *tokenizer;
	strcpy(string, argv[2]);
	strcpy(sep, argv[1]);

	/*      for(i = 0; i < strLen; i++)
	 *      	{
	 *      		printf("input string is %c\n", string[i]);
	 *   		}*/
	for(i=0; i < strLen; i++)
	{
		if((string[i] == '\\' && i == 0) || (string[i] == '\\' && i>0 && string[i-1] != '\\'))
		{
			printf("escape char\n");
			/*finalString = checkEscape(string, i, strLen, sep, sepLen);
			  escapePresent = 1;*/
			finalSize = finalSize + checkEscape(string, i, strLen, sep, sepLen);
		}
	}

	if(finalSize != strLen)
	{
		finalSize++;
		finalString = fixEscape(string, strLen, sep, sepLen, finalSize);
		char result[finalSize];
		strcpy(result, finalString);
		tokenizer = TKCreate(sep, result);
	}else{
		printf("in here\n");
		tokenizer = TKCreate(sep, string);
	}

	printf("\n");
	
	while (tokenizer != NULL){
		token = TKGetNextToken(tokenizer);
		printf("%s\n", token);
		tokenizer = tokenizer->nextToken;
	}

	TKDestroy(tokenizer);

	/*      printf("str is %s\n", string);*/
	/*      printf("sep is %d\n", sepLen);*/

	return 0;
}
