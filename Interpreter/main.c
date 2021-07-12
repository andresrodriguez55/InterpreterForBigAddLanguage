#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------------- Functions & Structures Declarations ----------------------

typedef struct StringStructure StringStructure;
typedef struct Memory Memory;
typedef struct OutList OutList;

void addCharToString(struct StringStructure **character_list, char character);


void addElement(struct Memory **list_of_variables, char *variable_name);
int variableExists(struct Memory *list_of_variables, char *variable_name);


void moveValue(char *variable_name, char* value);
char *obtainVariableValue(char *variable_name);
char* doTheSum(char* value1, char* value2, int line_number);
void sumStringIntegerToVariable(char *variable_name, char* value, 
                                                               int line_number);
int whichNumberIsBigger(char *value0, char *value1);
char* skipTheZeros(char* value);
char* doTheSubtraction(char* value1, char* value2);
void subtractStringIntegerToVariable(char *variable_name, char* value, 
                                                               int line_number);
void addStringElementToOutList(struct StringStructure **character_list, 
                                                     struct OutList **out_list);
void printString(struct StringStructure *character_list);
void printOutList(struct OutList **out_list);
void addCharArrayToOutList(char *char_array, struct OutList **out_list);
void addVariableToOutList(char *char_array, struct OutList **out_list);
void addNewlineToOutList(struct OutList **out_list);


void alertError(int error_code, int line_number);
void alertErrorPerState(int line_number, int *state);
void typeOfSpacePositionAndTransitionMaker(int *previous_state, 
          int saved_string_code, char *character_accumulation, int line_number);
void commaPosition(int *previous_state, int saved_string_code, 
                                 char *character_accumulation, int line_number);
void dotPosition(int *previous_state, int saved_string_code, 
                                  char *character_accumulation, int line_number, 
                                                 int can_perform_the_operation);
void endOfLine(int *previous_state, int string_flag, int comment_flag, 
                                                    int is_square_bracket_open);
void stringCharacterPosition(int *string_constant_flag, int *previous_state, 
                                                               int line_number);


int isAlphaChar(char character);
int isConstantChar(char character);
int isPauseChar(char character);
int encodeSavedString(char* word); 


void runLoopBlock(FILE *document_to_be_read, int *previous_state, 
        int *line_number, char *loop_value_variable, char *loop_value, 
        int is_previous_loop_working);
int isLoopValuePositive();
void runMainCodeBlock(char* document_name_to_be_read);


void get_command_of_read_file(FILE* fp);


/*----------------------------- String Store -----------------------------------

In order to have a very good memory and speed distribution, it has been decided 
to save a maximum of 102 characters for the tokens (due to the maximum capacity 
of decimals that can be saved, also taking into account the negative sign and 
for that matter 1 place more).

But there is a big problem and that is how the strings should be stored, the 
characters that are being read cannot be written in the document of the heap 
because the sign of the string may not close, so you must always save the 
content due to the linear reading that the written algorithms will do.

For this reason it has been decided to use a linked list that stores the 
characters of the strings. The temporary variable that saves the characters 
of the other tokens will only save the double quotation marks of the strings 
to know if a string was saved in a correct state (referring to the FSMs). 
Based on that, the characters stored in the linked list will be 
written to the document. */

struct StringStructure
{
    char character;
    struct StringStructure *next; 
};
struct StringStructure *string; //Global variable.

void addCharToString(struct StringStructure **character_list, char character)
{
    struct StringStructure *new_character; //Declaration of a new character.
    
    //Allocating the requested memory.
    new_character = malloc(sizeof(struct StringStructure)); 
    
    new_character->character=character; 
    new_character->next=NULL;
    
    if(*character_list==NULL) //If list is empty.
        *character_list=new_character;
    
    else
    {
        struct StringStructure *reader;
        reader=*character_list;
        
        while ((reader)->next != NULL)
            reader = (reader)->next;
        
        (reader)->next=new_character;
    }
}


/* ----------------------- Memory for Variables---------------------------------
To control the declared variables, it has been decided to create a linked 
list. In order to make controls much faster, it has been decided that the head
of the list is its tail, because normally after the declaration of variables 
they are used at the moment. */

struct Memory
{
    char *variable_name;
    char *value_as_string; 
    struct Memory *behind; //Faster.
};
struct Memory *memory; //Global variable.

void addElement(struct Memory **list_of_variables, char *variable_name)
{
    if(variableExists(*list_of_variables, variable_name)) 
    {
        //If the declared variable exists, it will be given the value 0.
        char *value0=malloc(103);
        *value0='\0'; *(value0+1)='0'; *((value0++)+2)='\0';
        moveValue(variable_name, value0);
        return;
    }
    
    struct Memory *new_element; //Declaration of  the new element.
    new_element = malloc(sizeof(struct Memory));//Allocating the requested  
                                                //memory.
    new_element->value_as_string=malloc(103);
    *((new_element->value_as_string))='\0';
    *(new_element->value_as_string+1)='0'; 
    *((new_element->value_as_string)+2)='\0';
    
    new_element->behind=NULL;

    new_element->variable_name=malloc(strlen(variable_name)+1);
    strcpy(new_element->variable_name, variable_name);
        
    if(*list_of_variables==NULL) //If list is empty.
        *list_of_variables=new_element;

    else
    {
        //Now the new element is the head of the list.
        new_element->behind=*list_of_variables;
        *list_of_variables=new_element;
    }
}

int variableExists(struct Memory *list_of_variables, char *variable_name)
{
    /* The entire linked list will be traversed looking for a stored variable 
    with the same name as the required name. */
    
    if(list_of_variables == NULL)
        return 0;

    struct Memory *reader=list_of_variables;
    while((reader->behind)!=NULL)
    {
        if(strcmp(reader->variable_name, variable_name)==0)
            return 1;
            
        reader=reader->behind;
    }
    
    //Last possible element of the linked list.
    if(strcmp(reader->variable_name, variable_name)==0)
        return 1;    
    
    return 0;
}


/* -----------------------------Interpreter-------------------------------------
The interpreter will basically move values to the declared variables, it will 
obtain values of the declared variables, it will be able to add or subtract 
values to the already declared variables and most importantly, it will have a 
structure that can save the elements (array of characters or elements type 
string structure that we create) and functions that can save and print elements.

Why have we done something like this? Because this program will fulfill the
function of an interpreter, as long as a line of code is correct, it will be
interpreted, this means that if, for example, the line does not have an end of 
line, it should not be interpreted, so it will always be able to print what is 
necessary required to create these structures and functions. */


char *natural_number; //Global variable.
char *variable_name; //Global variable.

void moveValue(char *variable_name, char* value)
{
    struct Memory *reader=memory;
    while((reader->behind)!=NULL)
    {
        if(strcmp(reader->variable_name, variable_name)==0)
            break;

        else        
            reader=reader->behind;
    }
       
    strcpy(reader->value_as_string, value);
}

char *obtainVariableValue(char *variable_name)
{
    struct Memory *reader=memory;
    while((reader->behind)!=NULL)
    {
        if(strcmp(reader->variable_name, variable_name)==0) 
            return reader->value_as_string;
            
        reader=reader->behind;
    }
    return reader->value_as_string;
}

/* The functions that fulfill the functions of adding and subtracting 
will work evenly. Why? Look down.

ADD X TO Y - SUB X FROM Y 

*Sum
    x=0 -> (y)
    y=0 -> (x)
    x>0 & y>0 -> +(add x to y)
    x<0 & y>0 -> +(sub x from y)
    x>0 & y<0 -> -(sub x from y)
    x<0 & y<0 -> -(add x to y)
 
*Sub 
    x=0 -> (y)
    y=0 -> -(x)
    *x>0 & y>0  
        y>=x -> +(sub x from y)
        x>y -> -(sub x from y)
    x<0 & y>0 -> +(add x to y)
    x>0 & y<0 -> -(add x to y)
    *x<0 & y<0 
        y>=x -> +(sub -y from -x)
        x>y -> -(sub -y from -x) 
 */

char* doTheSum(char* value1, char* value2, int line_number)//Only for positives.
{
    char *result;
    result=malloc(102); // \0 - 100 digit - \0
    *(result)='\0';
    
    int x1, x2;
    x1=0; x2=0;
    for(;*(value1+x1)!='\0'; x1++);
    for(;*(value2+x2)!='\0'; x2++);
    x1-=1; x2-=1; //Going to the last digits indeces.
    
    int cin, sum, x3;
    cin=0; x3=-1;
    for(;*(value1+x1)!='\0' && *(value2+x2)!='\0'; x1--, x2--, x3--)
    {
        sum=cin+((int)*(value1+x1)-48)+((int)*(value2+x2)-48);
        if(sum<10)
        {
            sum+=48; //ascii
            *(result+x3)=sum;
            cin=0;
        }
        
        else
        {
            cin=1;
            sum=(sum%10)+48;
            *(result+x3)=sum;
        }
    }
     
    for(;*(value1+x1)!='\0';x1--, x3--)
    {
        sum=cin+((int)*(value1+x1)-48);
        if(sum<10)
        {
            sum+=48; //ascii
            *(result+x3)=sum;
            cin=0;
        }
        
        else
        {
            cin=1;
            sum=(sum%10)+48;
            *(result+x3)=sum;
        }
    }
    
    for(;*(value2+x2)!='\0';x2--, x3--)
    {
        sum=cin+((int)*(value2+x2)-48);
        if(sum<10)
        {
            sum+=48; //ascii
            *(result+x3)=sum;
            cin=0;
        }
        
        else
        {
            cin=1;
            sum=(sum%10)+48;
            *(result+x3)=sum;
        }
    }
    
     
    if(cin)
    {
        if(x3==-101) //Exceding the digits
            alertError(15, line_number);
        
        *(result+x3)=49; //'1';
        x3--;
    }

    *(result+x3)='\0';
    result=result+x3+1;
    
    return result;  
}

char* doTheSubtraction(char* value1, char* value2);
int whichNumberIsBigger(char *value0, char *value1);

void sumStringIntegerToVariable(char *variable_name, char* value, 
                                                                int line_number)
{   
    char *variable_value=malloc(103);
    strcpy(variable_value, obtainVariableValue(variable_name));
    
    if(*variable_value=='0')
    {
        moveValue(variable_name, value);
        return;
    }
    
    else if(*value=='0')
        return;
    
    char* result;
    *(variable_value-1)='\0';
    *(value-1)='\0';
    
    if(*variable_value!='-' && *value!='-') //+ +
        moveValue(variable_name, doTheSum(variable_value, value, line_number));
   
    else if(*value=='-' && *variable_value=='-') // - -
    {
        *(variable_value++)='\0';
        *(value++)='\0';

        result=doTheSum(variable_value, value, line_number);
        *(result-1)='-';
        *(result-2)='\0';
        moveValue(variable_name, --result);       
    }
    
    else if(*variable_value=='-') //+ - 
    {
        *(variable_value++)='\0';
       
        if(whichNumberIsBigger(variable_value, value))
        {
            result=doTheSubtraction(value, variable_value);
            moveValue(variable_name, result);
        }

        else
        {
            result=doTheSubtraction(variable_value, value);
            *(result-1)='-';
            *(result-2)='\0';
            moveValue(variable_name, --result);
        } 
    }
    
    else  //- + 
    {
        *(value++)='\0';
       
        if(whichNumberIsBigger(variable_value, value))
        {
            result=doTheSubtraction(value, variable_value);
            *(result-1)='-';
            *(result-2)='\0';
            moveValue(variable_name, --result);
        }

        else
        {
            result=doTheSubtraction(variable_value, value);
            moveValue(variable_name, result);
        } 
    }
}

/* As we saw in the comment above, we will need this function for the 
   subtraction function. */

int whichNumberIsBigger(char *value0, char *value1) //Positive 2 numbers.
{
    int x0, x1;
    x0=strlen(value0); x1=strlen(value1);
    
    if(x0>x1)
        return 0;
    
    else if(x0<x1)
        return 1;
    
    for(x0=0, x1=0; *(value0+x0)!='\0'; x0++, x1++)
    {
        if((int)*(value0+x0)>(int)*(value1+x1)) //ascii 
            return 0;
        else if((int)*(value0+x0)<(int)*(value1+x1)) //ascii
            return 1;
    }    
    
    return 0;
}

/* The subtractions can leave unnecessary 0s, this function has been written 
   to eliminate them, it will also serve to eliminate unnecessary 0s found in 
   the code (example: 0005, -0033, etc ...). */

char* skipTheZeros(char* value)
{
    int its_negative=0;
    if(*value=='-')
    {
        value++;
        its_negative=1;
    }
        
    char* advancer;
    advancer=value;
    
    int x=0;
    for(;*(advancer+x+1)=='0' && *(advancer+x)=='0' ;x++);
    
    if(*(advancer+x+1)!='\0' && *(advancer+x)=='0')
        x++;
    
    value=(advancer+x);
    
    if(its_negative && *value!='0')
    {
        *(value-1)='-';
        *(value-2)='\0';
        value--;
    }
    
    else
        *(value-1)='\0';
                
    return value;
}

char* doTheSubtraction(char* value1, char* value2) //value1>=value2
{
    char *result;
    result=malloc(103);
    *(result)='\0';
    
    int x1, x2;
    x1=0; x2=0;
    for(;*(value1+x1)!='\0'; x1++);
    for(;*(value2+x2)!='\0'; x2++);
    x1-=1; x2-=1;
    
    int cin, rest, x3;
    cin=0; x3=-1;
    for(;*(value1+x1)!='\0' && *(value2+x2)!='\0'; x1--, x2--, x3--)
    {
        rest=((int)*(value1+x1)-48)-((int)*(value2+x2)-48)-cin;
        if(rest>=0)
        {
            rest+=48; //ascii
            *(result+x3)=rest;
            cin=0;
        }
        
        else
        {
            cin=1;
            rest=(rest+10)+48;
            *(result+x3)=rest;
        }
    }
     
    for(;*(value1+x1)!='\0';x1--, x3--)
    {
        rest=((int)*(value1+x1)-48)-cin;
        if(rest>=0)
        {
            rest+=48; //ascii
            *(result+x3)=rest;
            cin=0;
        }
        
        else
        {
            cin=1;
            rest=(rest+10)+48;
            *(result+x3)=rest;
        }
    }
    
    for(;*(value2+x2)!='\0';x2--, x3--)
    {
        rest=((int)*(value2+x2)-48)-cin;
        if(rest>=0)
        {
            rest+=48; //ascii
            *(result+x3)=rest;
            cin=0;
        }
        
        else
        {
            cin=1;
            rest=(rest+10)+48;
            *(result+x3)=rest;
        }
    }
    
    *(result+x3)='\0';
    result=result+x3+1;
    
    return skipTheZeros(result);  
}

void subtractStringIntegerToVariable(char *variable_name, char* value, 
                                                                int line_number)
{   
    char *variable_value;
    variable_value=obtainVariableValue(variable_name);
    
    if(*variable_value=='0')
    {
        if(*value!='-')
        {
            *(value-1)='-';
            *(value-2)='\0';
            moveValue(variable_name, --value);
            return;
        }
        
        else
        {
            *value='\0';
            moveValue(variable_name, ++value);
            return;      
        }        
    }
    
    else if(*value==0)
        return;
    
    char *result;
    *(variable_value-1)='\0';
    *(value-1)='\0';
    
    if(*variable_value!='-' && *value!='-') //+ +
    {
        //Bigger-lowest values control to flip the result sign
        if(whichNumberIsBigger(variable_value, value))
        {
            result=doTheSubtraction(value, variable_value);
            *(result-1)='-';
            *(result-2)='\0';
            moveValue(variable_name, --result);
        }
        
        else
        {
            result=doTheSubtraction(variable_value, value);
            moveValue(variable_name, result);
        } 
    }
    
    else if(*value=='-')
    {
        if(*variable_value=='-') // - -
        {
            *(variable_value++)='\0';
            *(value++)='\0';
            
            if(whichNumberIsBigger(variable_value, value))
            {
                result=doTheSubtraction(value, variable_value);
                moveValue(variable_name, result); 
            }
            
            else
            {
                result=doTheSubtraction(variable_value, value);
                *(result-1)='-';
                *(result-2)='\0';
                moveValue(variable_name, --result);
            }
        }
        
        else //- +
        {
            *(value++)='\0';
            result=doTheSum(value, variable_value, line_number);
            moveValue(variable_name, result);
        }
    }
    
    else //+ -
    {
        *(variable_value++)='\0';
        result=doTheSum(value, variable_value, line_number);
        *(result-1)='-';
        *(result-2)='\0';
        moveValue(variable_name, --result);
    }
}

struct OutList
{
    void *element_of_outlist;
    int element_code; //0-string, 1-char array
    struct OutList *next_element_of_outlist;
};
struct OutList *out_list; //Global variable.

void addStringElementToOutList(struct StringStructure **character_list, 
                                                      struct OutList **out_list)
{
    struct StringStructure *string; 
    string=malloc(sizeof(struct StringStructure));
    string=*character_list;
    *character_list=NULL;
    
    struct OutList *element;
    element=malloc(sizeof(struct OutList));
    element->element_of_outlist=string;
    (*element).element_code=0;
    element->next_element_of_outlist=NULL;

    if(*out_list==NULL) //If list is empty.
        *out_list=element;
  
    else
    {
        struct OutList *reader;
        reader=*out_list;
        
        while (reader->next_element_of_outlist != NULL)
            reader = (reader)->next_element_of_outlist;
        
        ((reader)->next_element_of_outlist)=element;
    }
}

void printString(struct StringStructure *character_list)
{
    while(character_list!=NULL) //Will reset at the same time!
    {
        printf("%c", (*character_list).character);
        character_list=character_list->next;
    }
}

void printOutList(struct OutList **out_list)
{
    while (*out_list!=NULL)
    {
        if((*out_list)->element_code==0)
        {
            struct StringStructure *string;
            string=(struct StringStructure *)((*out_list)->element_of_outlist);
            
            printString( string );
        }
        
        if((*out_list)->element_code==1)
        {
            printf("%s", (char*)((*out_list)->element_of_outlist));
        }
        
        *out_list = (*out_list)->next_element_of_outlist;
    }
}

void addCharArrayToOutList(char *char_array, struct OutList **out_list)
{
    struct OutList *new_element;
    new_element=malloc(sizeof(struct OutList));
    new_element->element_of_outlist=malloc(103);
    strcpy((new_element->element_of_outlist), char_array);
    (*new_element).element_code=1;
    new_element->next_element_of_outlist=NULL;

    if(*out_list==NULL) //If list is empty.
        *out_list=new_element;
  
    else
    {
        struct OutList *reader;
        reader=*out_list;
        
        while (reader->next_element_of_outlist != NULL)
            reader = (reader)->next_element_of_outlist;
        
        ((reader)->next_element_of_outlist)=new_element;
    }
}

void addVariableToOutList(char *char_array, struct OutList **out_list)
{
    char *number_as_char_array;
    number_as_char_array=obtainVariableValue(char_array);
    addCharArrayToOutList(number_as_char_array, out_list);
}

void addNewlineToOutList(struct OutList **out_list)
{
    char *newline=malloc(2);
    strcpy(newline, "\n");
    addCharArrayToOutList(newline, out_list);
}


/* ------------------------------- PARSER --------------------------------------
 
In order to analyze all the tokens correctly taking into account the grammar of 
the code, it has been decided to partition this work.

First individual FSMs have been drawn simulating each possible action 
(loops, variable declarations, etc ...). Having done so, it has been decided 
to design a state table and thus be able to guide us in each moment of what 
 state we are in and to which we should and next.

For the reject states, it has been decided to design a table error, it will 
contain the possible errors that we can find and will be used in the future to 
send messages to the user via the console, determining the specific error  
found.

In order to implement all this, it has been decided to encode the stored tokens, 
so at all times it will be known if a keyword, a valid variable name, an 
invalid, etc ...

Finally, to know when we have to analyze a situation, pause characters have 
been determined (types of space, periods, commas, etc.). We know that when we 
get to the reading of this type of character, a token should normally have 
already been saved (most of the time). The double quotation mark and curly 
bracket (opener and closer) characters have been excluded from our pause 
characters. The reason for this is due to the intersection between the two.
 
|------------------------------------------------------------------|
|                           State Table                            |
|------------------------------------------------------------------|
| State initial                                             |   0  |
| State int                                                 |   1  |
| State variable name being declared                        |   2  |
| State move                                                |   3  |
| State value that is being moved                           |   4  |            
| State to (move)                                           |   5  | 
| State variable to which a value is being moved            |   6  |
| State loop                                                |   7  |
| State loop n times value                                  |   8  |
| State times                                               |   9  |  
| State out                                                 |  10  |  
| State list element                                        |  11  | 
| State add                                                 |  12  | 
| State increase or decrease value (sum)                    |  13  |
| State to (add)                                            |  14  | 
| State variable being increased or decreased (sum)         |  15  | 
| State sub                                                 |  16  | 
| State increase or decrease value (subtraction)            |  17  |
| State from                                                |  18  |
| State variable being increased or decreased (subtraction) |  19  |
|------------------------------------------------------------------|  

|----------------------------------------------------------|
|                      Encode Values                       |
|----------------------------------------------------------|
| Keyword Int                                         |  0 |
| Valid Variable Name                                 |  1 |
| Invalid Variable Name (bigger then 20 characters)   |  2 |
| Invalid Variable Name (invalid name)                |  3 |
| Keyword Move                                        |  4 |
| Valid IntConstant                                   |  5 |
| Invalid IntConstant (contains more than 100 digits) |  6 |
| Invalid IntConstant (example : -, 5-5, 5--53)       |  7 |
| Keyword To                                          |  8 |
| Keyword Loop                                        |  9 |
| Keyword Times                                       | 10 |  
| Keyword Out                                         | 11 |  
| Keyword Newline                                     | 12 | 
| Keyword Add                                         | 13 | 
| Keyword Sub                                         | 14 | 
| Keyword From                                        | 15 | 
| Valid String                                        | 16 |
| Empty                                               | 17 |
|----------------------------------------------------------| 
 
|-----------------------------------------------------------------------------|
|                                  Error Codes                                |
|-----------------------------------------------------------------------------|
|           Line number>0               |            Line number<0            |
|---------------------------------------|-------------------------------------|
| Invalid statement                | 0  | Square/curly bracket or double  | 0 |
| Invalid variable (length)        | 1  |  quotation mark incomplete      |   |
| Invalid variable (name)          | 2  |                                 |   |
| Invalid variable declaration     | 3  |                                 |   |
| Invalid value assigment          | 4  | End of line not found           | 1 |   
| Use of undeclared variable       | 5  |                                 |   |   
| Invalid integer (decimal length) | 6  |                      |----------|   |  
| Invalid integer                  | 7  | No .ba file          | x>1 | x<0    |  
| Invalid loop                     | 8  |                      |--------------|
| Invalid output                   | 9  |                                     |
| Invalid sum                      | 10 |                                     |
| Invalid substraction             | 11 |                                     |
| Unrecognized character           | 12 |                                     |
| Invalid token found              | 13 |                                     |
| Operation exceeds 100 digits     | 15 |                                     |
|---------------------------------------|-------------------------------------*/

void alertError(int error_code, int line_number)
{    
    printf("\033[0;31m"); //Set the text to the color red
    
    if(line_number>0)
    {
        if(error_code==0)
            printf("Invalid statement");

        else if(error_code==1)
            printf("Variables can't be bigger then 20 characters, "
                    "invalid variable declaration statement");
        
        else if(error_code==2)
            printf("Use of invalid variable name, invalid variable declaration "
                    "statement");
        
        else if(error_code==3)
            printf("Invalid variable declaration statement");
        
        else if(error_code==4)
            printf("Invalid value assigment statement");
        
        else if(error_code==5)
            printf("Invalid statement, an undeclared variable is being used");
        
        else if(error_code==6)
            printf("Invalid use of integer, the declared integer passes "
                    "100 decimal places");
        
        else if(error_code==7)
            printf("Invalid integer");

        else if(error_code==8)
                printf("Invalid loop statement");

        else if(error_code==9)
            printf("Invalid output statement");

        else if(error_code==10)
            printf("Invalid sum statement");

        else if (error_code==11)
            printf("Invalid subtraction statement");
        
        else if (error_code==12)
            printf("Unrecognized character found");
        
        else if (error_code==13)
            printf("Invalid token found");
        
        else if(error_code==14)
            printf("Unrecognized character detected in code");
        
        else if(error_code==15)
            printf("The result of the arithmetic operation exceeds 100 digits");
        
        printf(" at line %d...\n", line_number);
    }
    
    else 
    {
        if(error_code==0)
            printf("A big lexeme is just left open...\n");
        else if(error_code==1)
            printf("Statement not finalized, end of line character "
                    "forgotten...\n");
        else
            printf("There is no .ba file with the name entered in the root "
                "folder...\n");
    }

    printf("\033[0m"); //Resets the text to default color
    
    printf("\nPress Any Key to end the program\n");  
    getchar();  
    exit(0);
}

void alertErrorPerState(int line_number, int *state)
{
    if(*state==0)
        alertError(0, line_number);
    
    else if(*state==2 || *state==1) //Invalid variable declaration.
        alertError(3, line_number);
    
    else if(3<=*state && 6>=*state) //Invalid value assigment.
        alertError(4, line_number);
    
    else if(7<=*state && 9>=*state) //Invalid loop statement.
        alertError(8, line_number);
    
    else if(*state==11 || *state==10) //Invalid out.
        alertError(9, line_number);
    
    else if(12<=*state && 15>=*state) //Invalid sum statement.
        alertError(10, line_number);
    
    else //Invalid subtraction statement.
        alertError(11, line_number);
}

void typeOfSpacePositionAndTransitionMaker(int *previous_state, 
        int saved_string_code, char *character_accumulation, int line_number)
{
    /* This function is the one that will simulate state transitions, 
    normally it should be used whenever a type of space is being read, but also
    to avoid repeating more code, it will be used again when a comma, period or 
    some other similar character is being read. The reason for this is because 
    when a point is read, we could have accumulated characters that are our 
    next state. But we could see this function directly as the one that 
    fulfills the state transitions. */
    
    if(saved_string_code==17) //Empty.
        return;
    
    //Start state or loop start line state.
    if(*previous_state==0 || *previous_state==9)
    {
        if(saved_string_code==0) //Saved string := int
            *previous_state=1; //Previous state := int

        else if(saved_string_code==4) //Saved string := move
            *previous_state=3; //Previous state := move

        else if(saved_string_code==9) //Saved string := loop
            *previous_state=7; //Previous state := loop

        else if(saved_string_code==11) //Saved string := out
            *previous_state=10; //Previous state := out

        else if(saved_string_code==13) //Saved string := add
            *previous_state=12; //Previous state := add

        else if(saved_string_code==14) //Saved string := sub
            *previous_state=16; //Previous state := sub   

        else 
            alertError(0, line_number);
    }
    
    else if(*previous_state==1) //int
    {
        //The saved string is a valid variable name.
        if(saved_string_code==1) 
        {
            *previous_state=2; //Previous state := variable name being declared.
            variable_name=malloc(21);
            strcpy(variable_name, character_accumulation);
        }

        //The saved string is an invalid variable name 
        //(bigger then 20 characters).
        else if(saved_string_code==2) 
            alertError(1, line_number);
        
        //The saved string is an invalid variable name.
        else if(saved_string_code==3) 
            alertError(2, line_number);
        
        //Output invalid variable declaration message.
        else
            alertError(3, line_number);
    }
    
    else if(*previous_state==3) //Previous state := move
    {
        //The saved string is a valid intConstant .
        if(saved_string_code==5 ) 
        {
            *previous_state=4; //Previous state := value that is being moved
            natural_number=malloc(103);
            strcpy(natural_number, character_accumulation);
        }

        //The saved string is an invalid intConstant (example : -).
        else if(saved_string_code==7) 
            alertError(7, line_number);
        
        //The saved string is a valid variable name.
        else if(saved_string_code==1) 
        {
            if(variableExists(memory, character_accumulation))
            {
                *previous_state=4;
                natural_number=malloc(103);
                strcpy(natural_number, 
                                   obtainVariableValue(character_accumulation));
            }

            else //Output use of undeclared variable message.
                alertError(5, line_number);
        }
 
        else //Output invalid value assigment message.
            alertError(4, line_number);
    }
    
    else if(*previous_state==4) //Previous state := value that is being moved
    {
        if (saved_string_code==8) //The saved string is the keyword to. 
            *previous_state=5; //Previous state := to (move) 

        else //Output invalid value assigment message.
            alertError(4, line_number);
    }
    
    else if(*previous_state==5) //Previous state := to (move)
    {
        if (saved_string_code==1) //Saved string := Valid variable name 
        {
            //Previous state := variable to which a value is being moved
            if(variableExists(memory, character_accumulation))
            {
                *previous_state=6;
                variable_name=malloc(21);
                strcpy(variable_name, character_accumulation);
            }

            else //Output use of undeclared variable message.
                alertError(5, line_number);
        }

        else //Output invalid value assigment message.
            alertError(4, line_number);
    }
    
    else if(*previous_state==12) //Previous state := add
    {
        //Saved string := Valid intConstant
        if (saved_string_code==5) 
        {
            *previous_state=13; //Previous state := increase or decrease
                                                    //value (sum)
            natural_number=malloc(103);
            strcpy(natural_number, character_accumulation);                 
        }
        
        //The saved string is an invalid intConstant (example : -).
        else if(saved_string_code==7) 
            alertError(7, line_number);

        else if(saved_string_code==1) //Saved string := Valid variable name
        {
            if(variableExists(memory, character_accumulation))
            {
                *previous_state=13; //Previous state := increase or decrease
                                    //                  value (sum)
                natural_number=malloc(103);
                strcpy(natural_number, 
                                   obtainVariableValue(character_accumulation)); 
            }

            else //Output use of undeclared variable message.
                alertError(5, line_number);              
        }

        else //Output invalid sum statement message.
            alertError(10, line_number);
    }
    
    else if(*previous_state==13) //Previous state := increase or decrease 
                                                   //value (sum)
    {
        if (saved_string_code==8) //Saved string := to 
            *previous_state=14; //Previous state := to (add)

        else //Output invalid sum statement message.
            alertError(10, line_number);
    }
    
    else if(*previous_state==14) //Previous state := to (add)
    {
        if(saved_string_code==1) //Saved string := Valid variable name
        {
            if(variableExists(memory, character_accumulation))
            {
                *previous_state=15;
                variable_name=malloc(21);
                strcpy(variable_name, character_accumulation);     
            }
            
            else
                alertError(2, line_number);              
        }

        else //Output invalid sum statement message.
            alertError(10, line_number);
    }
    
    else if(*previous_state==16) //Previous state := sub
    {
        //Saved string := Valid intConstant
        if (saved_string_code==5) 
        {
            *previous_state=17; //Previous state := increase or decrease
                                                  // value (subtraction)         
            natural_number=malloc(103);
            strcpy(natural_number, character_accumulation);   
        }
        
        //Saved string := invalid intConstant (example: --)
        else if (saved_string_code==7) 
            alertError(7, line_number);

        else if(saved_string_code==1) //Saved string := Valid variable name
        {
            if(variableExists(memory, character_accumulation))
            {
                *previous_state=17; //Previous state := increase or decrease 
                                                      //value (subtraction)
                natural_number=malloc(103);
                strcpy(natural_number, 
                                   obtainVariableValue(character_accumulation)); 
            }

            else
                alertError(5, line_number);      
        }

        else //Output invalid subtraction statement message.
            alertError(11, line_number);
    }
    
    else if(*previous_state==17) //Previous state := increase or decrease
                                                   //value (subtraction)
    {
        if (saved_string_code==15) //Saved string := from 
            *previous_state=18; //Previous state := from 

        else //Output invalid subtraction statement message.
            alertError(11, line_number);
    }
    
    else if(*previous_state==18) //Previous state := from
    {
        if(saved_string_code==1) //Saved string := Valid variable name
        {
            if(variableExists(memory, character_accumulation))
            {
                *previous_state=19;
                variable_name=malloc(21);
                strcpy(variable_name, character_accumulation); 
            }
            
            else
                alertError(2, line_number);
        }

        else //Output invalid subtraction statement message.
            alertError(11, line_number);
    }
    
    else if(*previous_state==7) //Previous state := loop
    {
        //Saved string := Valid intConstant 
        if (saved_string_code==5 ) 
        {
            *previous_state=8; //Previous state := loop n times value
            variable_name=NULL;
            natural_number=malloc(103);
            strcpy(natural_number, character_accumulation); 
        }
        
        //Saved string := invalid intConstant (example: --)
        else if (saved_string_code==7) 
            alertError(7, line_number);

        else if(saved_string_code==1) //Saved string := Valid variable name
        {
            if(variableExists(memory, character_accumulation))
            {
                *previous_state=8; //Previous state := loop n times value
                natural_number=NULL;
                variable_name=malloc(21);
                strcpy(variable_name, character_accumulation);
            }

            else
                alertError(5, line_number);      
        }

        else //Output invalid loop statement message.
            alertError(8, line_number);
    }
    
    else if(*previous_state==8) //Previous state := loop n times value
    {
        if (saved_string_code==10) //Saved string := times 
            *previous_state=9; //Previous state := times   

        else //Output invalid loop statement message.
            alertError(8, line_number);
    } 
    
    else if(*previous_state==10 || *previous_state==11) //out
    {
        if(saved_string_code==1)//Saved string := Valid variable name
        {
            if(variableExists(memory, character_accumulation))
            {
                *previous_state=11; //outlist element
                addVariableToOutList(character_accumulation, &out_list); 
            }

            else //Output use of undeclared variable message.
                alertError(5, line_number);        
        }
        
        else if(saved_string_code==16) //String
        {
            *previous_state=11;
            addStringElementToOutList(&string, &out_list);
        }
        
        else if(saved_string_code==5) //valid integer
        {
            *previous_state=11;
            addCharArrayToOutList(character_accumulation, &out_list);
        }
        
        else if(saved_string_code==12) //Newline
        {
            *previous_state=11;
            addNewlineToOutList(&out_list);
        }
           
        //Saved string := empty
        else if(saved_string_code==17)
            *previous_state=11;
        
        else if(saved_string_code==7) //Saved string := invalid integer (--)
            alertError(7, line_number);

        else
            alertError(9, line_number);
    }   
    /* If you get this far, we know that we are in one of the states 
    'variable name being declared ', 
    'variable to which a value is being moved',
    'variable being increased or decreased (sum)' or 
    'variable being increased or decreased (subtraction )' and that we have 
    stored characters. Then, as this accumulation of characters is 
    invalidating the completion of those statements, a type of error message 
    will be sent to the user depending on the situation. */
    
    else
        alertErrorPerState(line_number, previous_state); 
}

void commaPosition(int *previous_state, int saved_string_code, 
        char *character_accumulation, int line_number)
{
    typeOfSpacePositionAndTransitionMaker(previous_state, saved_string_code,
            character_accumulation, line_number);
    
    if(*previous_state!=11) //if the previous state is not list element.
        alertErrorPerState(line_number, previous_state); 
}

void dotPosition(int *previous_state, int saved_string_code, 
        char *character_accumulation, int line_number, 
        int can_perform_the_operation)
{
    //State transition.
    typeOfSpacePositionAndTransitionMaker(previous_state, saved_string_code, 
        character_accumulation, line_number);
    
    if(*previous_state==2)
    {
        if(can_perform_the_operation) //For loop.
            addElement(&memory, character_accumulation); 
    }
    
    else if(*previous_state==6)
    {
        if(can_perform_the_operation) //For loop.
            moveValue(character_accumulation, natural_number);
    }
    
    else if(*previous_state==11)
    {
        if(can_perform_the_operation) //For loop.
            printOutList(&out_list);
    }

    else if(*previous_state==15)
    {
        if(can_perform_the_operation) //For loop.
            sumStringIntegerToVariable(character_accumulation, natural_number,
                    line_number);
    }
    
    else if(*previous_state==19)
    {
        if(can_perform_the_operation) //For loop.
            subtractStringIntegerToVariable(character_accumulation, 
                    natural_number, line_number);
    }
    
    else
    {
        alertErrorPerState(line_number, previous_state); 
        return;
    }

    /* In the above if statement, the previous state 0 (initial) could have 
       been added to accept empty lines, but in the project it says 
       "Lines of Code: Point ('.') Is the end of line character. 
       Lines can contain keywords, brackets, variables and constants.", 
       nothing about empty lines. */
    
    *previous_state=0; 
}

void endOfLine(int *previous_state, int string_flag, int comment_flag, 
        int is_square_bracket_open)
{    
    /* Here we will check if the double quotation marks, square brackets or 
    curly brackets were closed, it will also be checked if an end of line was
    put in the last line of code. */
    
    if(string_flag || comment_flag || is_square_bracket_open)
        alertError(0, -1);
    
    else if(*previous_state!=0) //No finalizado
        alertError(1, -1);
}

void stringCharacterPosition(int *string_constant_flag, int *previous_state, 
        int line_number)
{   
    if (*previous_state==10 || *previous_state==11)    
        *string_constant_flag=!(*string_constant_flag);
   
    else    
        alertError(0, line_number);
}


/*---------------- Character and String Comparison Functions -------------------
 
Some functions of the standard C library could be used, but as they are 
functions that can be written in a few lines and also as additional special 
functions will be needed for some cases it has been decided to write all of 
them and thus have greater control in case changes are necessary. */
 
int isAlphaChar(char character)
{  
    if( (character<65 || character>90) && (character<97 || character>122))
        return 0;                        
    return 1;
}

int isConstantChar(char character)
{   
    if (character<48 || character>57) 
        return 0;                                                      
    return 1;
}

int isPauseChar(char character)
{    
    /* In order to build the Lexical Analyzer, it has been obtained by 
    using the strategy of dividing each problem by subproblems. 

    When reading any '.ba' document, it has been decided to make a 
    linear character-by-character reading and, as you go forward, save the 
    characters seen in a temporary variable. 

    When wondering when and how it should be controlled what has been 
    obtained in the reading of the document, it has been observed that there
    will always be characters of the pause type at all times that when one 
    of them is found, it will almost certainly be that a string is being 
    saved in the temporary variable. Some of these characters are line break, 
    space, comma, etc. */
    
    if(character=='\t' || character=='\n' || character==' ' || character==',' ||
            character=='.' || character=='[' || character==']')
        return 1;
    return 0;
}

int encodeSavedString(char* word) 
{      
    /*---------------------------------------------------------|
    |                      Encode Values                       |
    |----------------------------------------------------------|
    | Keyword Int                                         |  0 |
    | Valid Variable Name                                 |  1 |
    | Invalid Variable Name (bigger then 20 characters)   |  2 |
    | Invalid Variable Name (invalid name)                |  3 |
    | Keyword Move                                        |  4 |
    | Valid IntConstant                                   |  5 |
    | Invalid IntConstant (contains more than 100 digits) |  6 |
    | Invalid IntConstant (example : -, 5-5, 5--53)       |  7 |
    | Keyword To                                          |  8 |
    | Keyword Loop                                        |  9 |
    | Keyword Times                                       | 10 |  
    | Keyword Out                                         | 11 |  
    | Keyword Newline                                     | 12 | 
    | Keyword Add                                         | 13 | 
    | Keyword Sub                                         | 14 | 
    | Keyword From                                        | 15 | 
    | Valid String                                        | 16 |
    | Empty                                               | 17 |
    |---------------------------------------------------------*/ 
    
    if(*word=='\0') //Empty String
        return 17;
    
    if(*word=='"') 
        return 16;
    
    /*----------------------Keyword Fast Control--------------------------------
     
    There are 10 possible keywords, instead of comparing each word that 
    can be a keyword 10 times (with the possible words) it has been decided
    to do it with the pull only once, so the performance will have 
    increased. */
    
    if(*word=='i')         
    {
        if(*(word+1)=='n' && *(word+2)=='t' && *(word+3)==0)
            return 0;
    }  
    
    else if(*word=='m')         
    {
        if(*(word+1)=='o' && *(word+2)=='v' && *(word+3)=='e' && *(word+4)==0)
            return 4;
    }   
    
    else if(*word=='l')         
    {
        if(*(word+1)=='o' && *(word+2)=='o' && *(word+3)=='p' && *(word+4)==0)
            return 9;
    }   
    
    else if(*word=='o')         
    {
        if(*(word+1)=='u' && *(word+2)=='t' && *(word+3)==0)
            return 11;
    }  
    
    else if(*word=='t')         
    {
        if(*(word+1)=='i' && *(word+2)=='m' && *(word+3)=='e' &&
                *(word+4)=='s' && *(word+5)==0)
            return 10;
        
        else if(*(word+1)=='o' && *(word+2)==0)
            return 8;
    } 
    
    else if(*word=='n')         
    {
        if(*(word+1)=='e' && *(word+2)=='w' && *(word+3)=='l' &&
            *(word+4)=='i' && *(word+5)=='n' && *(word+6)=='e' )
            return 12;
    }   
    
    else if(*word=='a')         
    {
        if(*(word+1)=='d' && *(word+2)=='d' && *(word+3)==0)
            return 13;
    }  
    
    else if(*word=='s')         
    {
        if(*(word+1)=='u' && *(word+2)=='b' && *(word+3)==0)
            return 14;
    }  
    
    else if(*word=='f')         
    {
        if(*(word+1)=='r' && *(word+2)=='o' && *(word+3)=='m' && *(word+4)==0)
            return 15;
    }  
    
    int string_index;
    
    //-------------------Valid/Invalid IntConstant Value------------------------
    if(isConstantChar(*word) || *word=='-')
    {
        if(*word=='-' && *(word+1)=='\0')
            return 7; //Invalid integer code.
               
        for(string_index=1; *(word+string_index)!='\0'; string_index++)
        {
            if(*(word+string_index)=='-')
                return 7; //Invalid integer code.
            if(48>*(word+string_index) || 57<*(word+string_index))
                return 3; //Invalid variable name.
        }
        
        if(*word!='-' && string_index>100) //incluiding '\0'.
            return 6; //if it is a positive number passing 100 decimal places
         
        return 5; //Valid integer code.
    } 
     
    //---------------------Valid/Invalid Variable Name--------------------------
    if(isAlphaChar(*word))
    {
        for(string_index=1; *(word+string_index)!='\0' && string_index<=19; 
                string_index++)
        {
            if(!isAlphaChar(*(word+string_index)) && 
                !isConstantChar(*(word+string_index)) && 
                *(word+string_index)!='_')
            {
                return 3; //Invalid variable name code.
            }       
        }
        
        if(string_index==20 && *(word+string_index)!='\0')
           return 2; //Invalid variable name (length) code.
        
        else
            return 1; //Valid variable name code.
    }
    
    return 3; //Invalid variable name code.
}


/*--------------------- Analyze and Execute Code Blocks ------------------------
  
In order to make the interpreter as correct as possible, it has been decided to 
design 2 different types of readings for the main block and a loop block.

Being an interpreter the program must read the lines of code one by one and at 
the same time execute them one by one, this in the main block does not generate 
a problem but in a loop block it does, that is why a function has been made 
especially for loops such that if the value of a loop is positive it will read 
and execute each line and if the value of the loop is equal to or less than 0 
it will ONLY READ AND CONTROL the lines of code that the loop contains. If the 
value of the loop is positive at the end of the loop, the program will subtract
one from the value and control if it is equivalent to 0, if it is equivalent to
0 the program will end and if not it will return to the reading of the line of 
code whichever is the first within the loop you are working with.

When designing this function, all kinds of nested or nested loops that may 
occur have also been taken into account. */

void runLoopBlock(FILE *document_to_be_read, int *previous_state, 
        int *line_number, char *loop_value_variable, char *loop_value, 
        int is_previous_loop_working)
{
    int is_loop_value_a_constant=(loop_value_variable==NULL)?1:0;
    int initial_index_of_file=ftell(document_to_be_read);
    int initial_line_number=*line_number;
    int is_there_a_bracket=0;
    char* loop_counter=malloc(102);
    
    char* one_value=malloc(3);
    strcpy(one_value, "1\0");
    
    char character_that_is_being_read;
    char character_accumulation[102];  
    int saved_string_code;
    int current_index_counter=0;
    int comment_flag=0;
    int string_constant_flag=0;
    int type_of_space_flag=0; //Reader speed booster.  
    
    if(!is_loop_value_a_constant)
        strcpy(loop_counter, obtainVariableValue(loop_value_variable));

    else 
        strcpy(loop_counter, loop_value);
    
    if(*loop_counter=='0' || *loop_counter=='-')
        is_previous_loop_working=0;
   
    while((character_that_is_being_read = fgetc(document_to_be_read)) != EOF)
    {                
        if( (isAlphaChar(character_that_is_being_read) || 
                isConstantChar(character_that_is_being_read) || 
                character_that_is_being_read=='_' || 
                character_that_is_being_read=='-') 
                && !comment_flag && !string_constant_flag)
        {
            
            if(current_index_counter<101)
            {
                character_accumulation[current_index_counter++]=
                    character_that_is_being_read;   

                type_of_space_flag=0;
            }
            
            else 
            {
                character_accumulation[current_index_counter++]='\0';
                strcpy(character_accumulation, 
                                          skipTheZeros(character_accumulation));
                current_index_counter=strlen(character_accumulation);
                
                if((current_index_counter+1)< 101) //plus actual character
                {
                    character_accumulation[current_index_counter++]=
                                                   character_that_is_being_read;
                    continue;
                }
                
                alertErrorPerState(*line_number, previous_state); //ARREGLAR PARA NUMEROS O TOKENS
            }
        }
        
        else if(string_constant_flag && character_that_is_being_read!='"')
        {
            addCharToString(&string, character_that_is_being_read);
            
            if(character_that_is_being_read=='\n')
                *line_number=(*line_number)+1;
            
            continue;
        }

        else if(comment_flag && character_that_is_being_read!='}')
        {
            if(character_that_is_being_read=='\n')
                *line_number=(*line_number)+1;
            
            continue;  
        }

        else if(isPauseChar(character_that_is_being_read))
        { 
            character_accumulation[current_index_counter++]='\0'; 
            saved_string_code=encodeSavedString(character_accumulation);
            
            if(saved_string_code==5) //valid integer
                strcpy(character_accumulation, 
                                         skipTheZeros(character_accumulation));
            
            if((character_that_is_being_read==' ' || 
                    character_that_is_being_read=='\t' || 
                    character_that_is_being_read=='\n' ) && 
                    !type_of_space_flag)
            {
                type_of_space_flag=1;   
                
                typeOfSpacePositionAndTransitionMaker(previous_state, 
                       saved_string_code, character_accumulation, *line_number);
                
                if(strcmp(character_accumulation, "times")==0)
                {
                    runLoopBlock(document_to_be_read, previous_state, 
                            line_number, variable_name, natural_number, 
                            is_previous_loop_working);
                    
                    //For nested loop.
                    if(*previous_state==0 && !is_there_a_bracket) 
                    {
                        if(is_previous_loop_working)
                        {                          
                            if(is_loop_value_a_constant)
                                strcpy(loop_counter, 
                                     doTheSubtraction(loop_counter, one_value));

                            else
                            {
                                subtractStringIntegerToVariable(
                                        loop_value_variable, one_value, 
                                                                   *line_number);
                                strcpy(loop_counter, 
                                      obtainVariableValue(loop_value_variable));
                            }
                            
                            if(*loop_counter=='0' || *loop_counter=='-')
                            {
                                previous_state=0;
                                return;
                            }
                            
                            fseek(document_to_be_read, initial_index_of_file,
                                    SEEK_SET);

                            is_there_a_bracket=0;
                            *previous_state=9;
                            is_there_a_bracket=0;
                            *line_number=initial_line_number;
                        }
                    
                        else
                            return; //End of loop.
                    }
                }
                
                if(character_that_is_being_read=='\n')
                    *line_number=(*line_number)+1;
            }
            
            else if(character_that_is_being_read==',')
                commaPosition(previous_state, saved_string_code, 
                                          character_accumulation, *line_number);
            
            else if(character_that_is_being_read=='[')
            {
                character_accumulation[current_index_counter++]='\0'; 
                saved_string_code=encodeSavedString(character_accumulation);
                typeOfSpacePositionAndTransitionMaker(previous_state, 
                       saved_string_code, character_accumulation, *line_number);
                
                if(*previous_state==9)
                {                   
                    if(saved_string_code==17) //Empty code.
                    {
                        if(!is_there_a_bracket)
                        {
                            *previous_state=0;
                            is_there_a_bracket=1;
                        }
                        
                        else
                            alertError(13, *line_number); //ARREGLARRRRRRRRRRRRRRRRRRRRRRRRRR
                    }
  
                    else // 'times[' case
                    {
                        fseek(document_to_be_read, ftell(document_to_be_read)-1,
                                SEEK_SET);
                        
                        runLoopBlock(document_to_be_read, previous_state, 
                                line_number, variable_name, 
                                natural_number, is_previous_loop_working);
                        
                        //For nested loop.
                        if(*previous_state==0 && !is_there_a_bracket) 
                        {
                            if(is_previous_loop_working)
                            {                          
                                if(is_loop_value_a_constant)
                                    strcpy(loop_counter, 
                                     doTheSubtraction(loop_counter, one_value));

                                else
                                {
                                    subtractStringIntegerToVariable(
                                            loop_value_variable, one_value, 
                                                                  *line_number);
                                    strcpy(loop_counter, 
                                      obtainVariableValue(loop_value_variable));
                                }

                                if(*loop_counter=='0' || *loop_counter=='-')
                                {
                                    previous_state=0;
                                    return;
                                }

                                fseek(document_to_be_read,initial_index_of_file,
                                                                      SEEK_SET);

                                is_there_a_bracket=0;
                                *previous_state=9;
                                is_there_a_bracket=0;
                                *line_number=initial_line_number;
                            }

                            else
                                return; //End of loop.
                        }
                    }
                }
                
                else
                    alertErrorPerState(*line_number, previous_state); //ARREGLARRRRRRRRRRRRRRRRRRRr
            }
            
            else if(character_that_is_being_read==']' )
            {
                if(is_there_a_bracket && *previous_state==0)
                {
                    if(is_previous_loop_working)
                    {                          
                        if(is_loop_value_a_constant)
                            strcpy(loop_counter, doTheSubtraction(loop_counter, 
                                                                    one_value));

                        else
                        {
                            subtractStringIntegerToVariable(loop_value_variable, 
                                                       one_value, *line_number);
                            strcpy(loop_counter, 
                                      obtainVariableValue(loop_value_variable));      
                        }
                        
                        if(*loop_counter=='0' || *loop_counter=='-')
                        {
                            *previous_state=0;
                            return;
                        }

                        fseek(document_to_be_read, initial_index_of_file,
                                SEEK_SET);
                        
                        is_there_a_bracket=0;
                        *previous_state=9;
                        *line_number=initial_line_number;
                    }
                    
                    else
                        return; //End of loop.
                }
                    
                else
                    alertError(13, *line_number); //ARREGLARRRRRRRRRRRRRRRRRRRRRRRRRR
            }
            
            else if(character_that_is_being_read=='.')
            {
                dotPosition(previous_state, saved_string_code, 
                        character_accumulation, *line_number, 
                        is_previous_loop_working);
                
                if(!is_there_a_bracket)
                {
                    if(is_previous_loop_working)
                    {
                        if(is_loop_value_a_constant)
                            strcpy(loop_counter, doTheSubtraction(loop_counter, 
                                                                    one_value));

                        else
                        {
                            subtractStringIntegerToVariable(loop_value_variable, 
                                                       one_value, *line_number);
                            strcpy(loop_counter, 
                                      obtainVariableValue(loop_value_variable));
                        }
                        
                        if(*loop_counter=='0' || *loop_counter=='-')
                        {
                            *previous_state=0;
                            return;
                        }
                        
                        fseek(document_to_be_read, initial_index_of_file,
                                SEEK_SET );
                        *line_number=initial_line_number;
                    }
                    
                    else
                        return;
                }
            }
                        
            character_accumulation[0]=0; //Reset.
            current_index_counter=0; //Reset.
        }
        
        else 
        {
            type_of_space_flag=0; //Reset.
        
            if(character_that_is_being_read==13)
                continue;

            else if(character_that_is_being_read=='{')
            {
                character_accumulation[current_index_counter++]='\0'; 
                current_index_counter=0;
                saved_string_code=encodeSavedString(character_accumulation);
                if(saved_string_code==5) //valid integer
                    strcpy(character_accumulation, 
                                          skipTheZeros(character_accumulation));
                
                typeOfSpacePositionAndTransitionMaker(previous_state, 
                       saved_string_code, character_accumulation, *line_number);
                
                comment_flag=1;
            }
            
            else if(character_that_is_being_read=='"') 
            {
                character_accumulation[current_index_counter++]='\0'; 
                saved_string_code=encodeSavedString(character_accumulation);
                typeOfSpacePositionAndTransitionMaker(previous_state, 
                       saved_string_code, character_accumulation, *line_number);
                current_index_counter=0;
                
                stringCharacterPosition(&string_constant_flag, previous_state, 
                        *line_number);
                 
                if(string_constant_flag)
                {
                    character_accumulation[current_index_counter++]='\0'; 
                    saved_string_code=encodeSavedString(character_accumulation);
                    
                    typeOfSpacePositionAndTransitionMaker(previous_state, 
                       saved_string_code, character_accumulation, *line_number);
                                    
                    character_accumulation[0]=0; //Reset.
                    current_index_counter=0; //Reset.
                }
                
                else
                {
                    saved_string_code=16;
                    
                    typeOfSpacePositionAndTransitionMaker(previous_state, 
                       saved_string_code, character_accumulation, *line_number);   
                    
                    character_accumulation[0]=0; //Reset.
                    current_index_counter=0; //Reset.
                }
            }

            else if(character_that_is_being_read=='}') 
            {   
                if (!comment_flag)
                    alertErrorPerState(*line_number, previous_state); 
                
                comment_flag=0;
            }

            else //Unrecognized character
                alertError(12, *line_number);            
        }
    }
    
    /* Control if at the end of the document something remained in the variable
       that saves the characters. */
    
    if(character_accumulation[current_index_counter-1]!='\0')
    {
        character_accumulation[current_index_counter++]='\0';
        saved_string_code=encodeSavedString(character_accumulation);
        
        typeOfSpacePositionAndTransitionMaker(previous_state, 
                       saved_string_code, character_accumulation, *line_number);
        
        saved_string_code=encodeSavedString(character_accumulation); 
        
        /* Here we will check if the double quotation marks or curly brackets 
           were closed. */
        
        endOfLine(previous_state, string_constant_flag,
                comment_flag, is_there_a_bracket);     
    }   
}

int isLoopValuePositive()
{
    if(variable_name==NULL)
    {
        if(*natural_number!='0' && *natural_number!='-')
            return 1;
        return 0;
    }
    
    char *value_of_variable=obtainVariableValue(variable_name);
    if(*value_of_variable!='0' && *value_of_variable!='-')
        return 1;
    return 0;
}

void runMainCodeBlock(char* document_name_to_be_read)
{
    FILE *document_to_be_read;
    
     if ((document_to_be_read = fopen(document_name_to_be_read, "r")) == NULL) 
        alertError(2, -1);
    
    char character_that_is_being_read;
    
    memory=NULL; 
    
    char character_accumulation[103]; 
    
    int previous_state=0; //Initial.
    int saved_string_code;
    int line_number=1;  
    int current_index_counter=0; /* It will be used to be able to save the 
                                    temporary string linearly */
    
    int comment_flag=0;
    int string_constant_flag=0;
    int square_bracket_counter=0;
    
    int type_of_space_flag=0; //Reader speed booster.

    
    while((character_that_is_being_read = fgetc(document_to_be_read)) != EOF)
    {                
        if( (isAlphaChar(character_that_is_being_read) || 
                isConstantChar(character_that_is_being_read) || 
                character_that_is_being_read=='_' || 
                character_that_is_being_read=='-') 
                && !comment_flag && !string_constant_flag)
        {
            
            if(current_index_counter<101)
            {
                character_accumulation[current_index_counter++]=
                    character_that_is_being_read;   

                type_of_space_flag=0;
            }
            
            else 
            {
                character_accumulation[current_index_counter++]='\0';
                strcpy(character_accumulation, 
                                         skipTheZeros(character_accumulation));
                current_index_counter=strlen(character_accumulation);
                
                if((current_index_counter+1)< 101) //plus actual character
                {
                    character_accumulation[current_index_counter++]=
                                                   character_that_is_being_read;
                    continue;
                }
                
                alertErrorPerState(line_number, &previous_state); //ARREGLAR PARA NUMEROS O TOKENS
            }

        }
        
        else if(string_constant_flag && character_that_is_being_read!='"')
        {
            addCharToString(&string, character_that_is_being_read);
            
            if(character_that_is_being_read=='\n')
                line_number++;
            
            continue;
        }

        else if(comment_flag && character_that_is_being_read!='}')
        {
            if(character_that_is_being_read=='\n')
                line_number++;
            
            continue;  
        }

        else if(isPauseChar(character_that_is_being_read))
        { 
            character_accumulation[current_index_counter++]='\0'; 
            saved_string_code=encodeSavedString(character_accumulation);
            
            if(saved_string_code==5) //valid integer
                strcpy(character_accumulation, 
                                          skipTheZeros(character_accumulation));
            
            if((character_that_is_being_read==' ' || 
                    character_that_is_being_read=='\t' || 
                    character_that_is_being_read=='\n' )&& 
                    !type_of_space_flag)
            {
                type_of_space_flag=1;   
                
                typeOfSpacePositionAndTransitionMaker(&previous_state, 
                        saved_string_code, character_accumulation, line_number);
                
                if(strcmp(character_accumulation, "times")==0)
                {
                    runLoopBlock(document_to_be_read, &previous_state, 
                            &line_number, variable_name, natural_number, 
                            isLoopValuePositive());
                }
                
                if(character_that_is_being_read=='\n')
                    line_number++;
            }
            
            else if(character_that_is_being_read==',')
                commaPosition(&previous_state, 
                        saved_string_code, character_accumulation, line_number);
            
            else if(character_that_is_being_read=='[')
            {
                character_accumulation[current_index_counter++]='\0'; 
                saved_string_code=encodeSavedString(character_accumulation);
                typeOfSpacePositionAndTransitionMaker(&previous_state, 
                        saved_string_code, character_accumulation, line_number);
                
                if(previous_state==9)
                {                             
                   
                    fseek(document_to_be_read, ftell(document_to_be_read)-1,
                            SEEK_SET);

                    runLoopBlock(document_to_be_read, &previous_state, 
                            &line_number, variable_name, natural_number, 
                            isLoopValuePositive());
                }
                
                else
                    alertErrorPerState(line_number, &previous_state); //ARREGLARRRRRRRRRRRRRRRRRRRr
            }
            
            else if(character_that_is_being_read==']' )
                alertError(13, line_number); //ARREGLARRRRRRRRRRRRRRRRRRR
            
            else if(character_that_is_being_read=='.')
                dotPosition(&previous_state, 
                     saved_string_code, character_accumulation, line_number, 1);
                                    
            character_accumulation[0]=0; //Reset.
            current_index_counter=0; //Reset.
        }
        
        else 
        {
            type_of_space_flag=0; //Reset.
        
            if(character_that_is_being_read==13)
                continue;

            else if(character_that_is_being_read=='{')
            {
                character_accumulation[current_index_counter++]='\0'; 
                current_index_counter=0;
                saved_string_code=encodeSavedString(character_accumulation);
                typeOfSpacePositionAndTransitionMaker(&previous_state, 
                        saved_string_code, character_accumulation, line_number);
                
                if(previous_state==9)
                {                             
                    fseek(document_to_be_read, ftell(document_to_be_read)-1,
                            SEEK_SET);

                    runLoopBlock(document_to_be_read, &previous_state, 
                            &line_number, variable_name, natural_number, 
                            isLoopValuePositive());
                    
                }
                
                comment_flag=1;
            }
            
            else if(character_that_is_being_read=='"') 
            {
                character_accumulation[current_index_counter++]='\0'; 
                saved_string_code=encodeSavedString(character_accumulation);
                typeOfSpacePositionAndTransitionMaker(&previous_state, 
                        saved_string_code, character_accumulation, line_number);
                current_index_counter=0;
                
                stringCharacterPosition(&string_constant_flag, &previous_state,
                        line_number);
                 
                if(string_constant_flag)
                {
                    character_accumulation[current_index_counter++]='\0'; 
                    saved_string_code=encodeSavedString(character_accumulation);
                    
                    typeOfSpacePositionAndTransitionMaker(&previous_state, 
                        saved_string_code, character_accumulation, line_number);
                                    
                    character_accumulation[0]=0; //Reset.
                    current_index_counter=0; //Reset.
                }
                
                else
                {
                    saved_string_code=16;
                    
                    typeOfSpacePositionAndTransitionMaker(&previous_state, 
                        saved_string_code, character_accumulation, line_number);   
                    
                    character_accumulation[0]=0; //Reset.
                    current_index_counter=0; //Reset.
                }
            }

            else if(character_that_is_being_read=='}') 
            {   
                if (!comment_flag)
                    alertErrorPerState(line_number, &previous_state); 
                
                comment_flag=0;
            }

            else //Unrecognized character
                alertError(12, line_number); 
        }
    } 
    
    if(character_accumulation[current_index_counter-1]!='\0')
    {
        character_accumulation[current_index_counter++]='\0';
        saved_string_code=encodeSavedString(character_accumulation);
        
        typeOfSpacePositionAndTransitionMaker(&previous_state, 
                        saved_string_code, character_accumulation, line_number);
        
        saved_string_code=encodeSavedString(character_accumulation); 
        
       /*Here we will check if the double quotation marks or curly brackets were 
         closed. */
        endOfLine(&previous_state, string_constant_flag, comment_flag, 0);     
    }
        
    fclose(document_to_be_read); 
}


/*----------------------------- User Inputs ------------------------------------
 It will only be taken in the name of the document that the user wants to 
 execute, the length of the name does not matter, you can write names of any 
 length. */ 

int document_name_length; //Global variable.
char *document_name; //Global variable.

void get_command_of_read_file(FILE* fp)
{    
    document_name_length=25; //Standart length, can change.
    
    //Allocating the requested memory.
    document_name = realloc(NULL, sizeof(*document_name)*document_name_length);
    
    char character_of_input='\n';
    
    while(EOF!=(character_of_input=fgetc(fp)) && 
            character_of_input==' ' || character_of_input=='\t' )
        continue;
    
    if(character_of_input=='\n')   //CAMBİARRRRRRRRR
    {
        *document_name='.';
        *(document_name+1)='b';
        *(document_name+2)='a';
        *(document_name+3)='\0';
        return;
    }
    
    int index=0;
    *(document_name+index++)=character_of_input; //First letter getted up.
     
    while(EOF!=(character_of_input=fgetc(fp)) && character_of_input != '\n')
    {
        /* Here the characters belonging to the entered file name will be 
        saved.
         
        If any errors occurred due to the realloc function the user would be 
        notified and the program would start over.*/
        
        *(document_name+index++)=character_of_input;
        if(index==document_name_length)
        {
            document_name = 
                    realloc(document_name, 
                    sizeof(*document_name)*(document_name_length+=16));
            if(!document_name_length)
            {
                printf("\033[0;31m"); //Set the text to the color red.
                printf("An error ocurred, press any key to end the program...\n");
                getchar();
                exit(0);
            }
        }
    }
    
    index+=3; //For the extensions.
    document_name_length=index; //It increased one up.
    document_name=realloc(document_name, 
                          sizeof(*document_name)*index);

    /* If any errors occurred due to the realloc function the user would be 
    notified and the program would start over.*/
    if(!document_name)
    {
        printf("\033[0;31m"); //Set the text to the color red.
        printf("An error ocurred, press any key to end the program...\n");
        getchar();
        exit(0);
    }
    
    *(document_name+index-3)='.';
    *(document_name+index-2)='b';
    *(document_name+index-1)='a';
    *(document_name+index)='\0';
    
    /* We know that C always reserves an index at the end for a string 
    end, just in case we will always end character arrays. */
    
    return;
}

int main(int argc, char** argv) 
{
    printf("Source file name to be read (only file name is enough): "); 
    get_command_of_read_file(stdin); 
    printf("\n");

    printf("\033[0;32m"); //Set the text to the color green.
    
    /* The name of the document is sent to our function that acts as a 
       lexical analyzer */
    runMainCodeBlock(document_name);
    
    printf("\033[0m"); //Resets the text to default color.
    
    printf("\nCompleted process! Press any key to end the program!\n");
    getchar();
    
    return (EXIT_SUCCESS);
}