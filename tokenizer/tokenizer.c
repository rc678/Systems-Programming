/*
 * tokenizer.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ {
	char* token;
	struct TokenizerT_* nextToken;
	int viewed;
};

typedef struct TokenizerT_ TokenizerT;

/* function prototypes of the functions I declared.*/
char* substring(char*, int, int); 
void print(TokenizerT*);  
char* checkEscape(char*, int, int);

/*
 * TKCreate creates a new TokenizerT object for a given set of separator
 * characters (given as a string) and a token stream (given as a string).
 * 
 * TKCreate should copy the two arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
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

	print(head); 

	return head;
}

char* checkEscape(char* string, int currIndex, int strLen)
{
	char str[strLen];
	char letter;
	char* res;
	char* hex;  
	int i, size; 
	char copy[7]; 
	char result[strLen + 5]; 
	int j, resultIndex;  
	strcpy(str, string);
	letter = str[currIndex+1];

	printf("CURR INDEX IS %d\n", currIndex);
	/*	for(i = 0; i < strLen; i++)
		{
		printf("string in checkEscape is %c\n", str[i]);
		}*/	
	switch(letter)
	{
		case 'n':
			hex = "[0x0a]";
			strcpy(copy, hex);
			break; 
		case 't':
			hex = "[0x09]";
			strcpy(copy, hex);
			break;
		case 'v':
			hex = "[0x0b]";
			strcpy(copy, hex);
			break;
		case 'b':
			hex = "[0x08]";
			strcpy(copy, hex);
			break;
		case 'r':
			hex = "[0x0d]";
			strcpy(copy, hex);
			break;
		case 'f':
			hex = "[0x0c]";
			strcpy(copy, hex);
			break;
		case 'a':
			hex = "[0x07]";
			strcpy(copy, hex);
			break;
		case '\\':
			hex = "[0x5x]";
			strcpy(copy, hex);
			break;
		case '"':
			hex = "[0x22]";
			strcpy(copy, hex);
			break;
		default:
			return string;
	}/*end of switch*/

	/*for(i=0; i < 6; i++)
	  {
	  printf("copy is %c\n", copy[i]);
	  }*/

	resultIndex = 0;
	for(i=0; i < strLen; i++)
	{
		if(i == currIndex)
		{
			printf("ESCAPE CHAR INDEX is %d\n", i);
			for(j=0; j < 6; j++)
			{
				result[resultIndex] = copy[j];
				/*	printf("result at i+j is %c\n", result[i+j]); 	*/
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


/*

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

	/*printf("str is %s\n", str);
	  printf(" start is %d end is %d size is %d\n ", start, end, size);*/

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
	for(i=0; i < strlen(result); i++)
	{
		printf("chars in result array in substring are %c\n", result[i]);
	}
	printf("substring is %s\n", res);
	return res; 	

}/*end of method*/

/*
 * Takes in a reference to the head of the built linked list and prints all the tokens. 
 */
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
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
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
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken(TokenizerT *tk) {

	return NULL;
}


/*
 * main will have two string arguments (in argv[1] and argv[2]).
 * The first string contains the separator characters.
 * The second string contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {

	int strLen = strlen(argv[2]);
	int sepLen = strlen(argv[1]);
	char string[strLen];
	char sep[sepLen]; 
	int i;
	int finalSize; 
	char* finalString;
	int escapePresent = 0; 
	strcpy(string, argv[2]);
	strcpy(sep, argv[1]);

	/*	for(i = 0; i < strLen; i++)
		{
		printf("input string is %c\n", string[i]);
		}*/	
	for(i=0; i < strLen; i++)
	{
		if(string[i] == '\\')
		{
			printf("escape char\n");
			finalString = checkEscape(string, i, strLen);
			escapePresent = 1; 
		}
	}

	if(escapePresent == 1)
	{
		finalSize = strlen(finalString);
		char result[finalSize];
		strcpy(result, finalString);
		TKCreate(sep, result);
	}else{
		printf("in here\n");
		TKCreate(sep, string);
	}

	/*	printf("str is %s\n", string);*/
	/*	printf("sep is %d\n", sepLen);*/


	return 0;
}

