#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
Documentation notes:

In order to seperate operands while evaluating the postfix expression into assembly code, spaces are added between
operands durıng infix to postfix conversıon.

*/

struct Stack // A struct to serve as a stack when converting infix notation to postfix notation.
{
    int *stackArray; // Pointer to the stack array that will be generated.
    int topOfStack; // Used as the stack pointer, always points to the top of the stack.
    unsigned size;
};

struct InstructionList // A struct to serve as a list to store assembly instructions before being printed out. 
{
    char* instructions; // Pointer to the instructions array that will be generated.
    unsigned size; // Used to keep track of the size so that the memory of instructions array can be updated dynamically by using realloc.
    unsigned used; 
};


// ==== Sub-routines to convert postfix expression into an assembly code. ====

struct InstructionList *initList(unsigned initialSize) // Used to create an instance of the struct InstructionList.
{
    struct InstructionList *array;

    array = malloc(sizeof(struct InstructionList));

    array -> size = initialSize;
    array -> used = 0;
    array -> instructions = malloc(array->size * sizeof(int));

    return array;
}

void insertInstruction(struct InstructionList *instructionList, char inst[]) // Enables to insert instructions into the instruction list. Re-allocates memory for the instruction array and updates size and used.
{
    int i;

    for(i = 0; i < strlen(inst); i++)
    {
        if (instructionList->used == instructionList->size) 
        {
            instructionList->size *= 2;
            instructionList->instructions = realloc(instructionList->instructions, instructionList->size * sizeof(char));
        }

        instructionList->instructions[instructionList->used++] = inst[i];
    }
}

void returnToOS(struct InstructionList* instructionList)
{
    char interrupt[] = "INT 20h\n";

    insertInstruction(instructionList, interrupt);
}

void MOV(struct InstructionList* instructionList, char reg[], char value[])
{
    char mov[20] = "MOV ";
    strcat(mov, reg);
    strcat(mov, ",0x");
    strcat(mov, value);
    strcat(mov, "\n");
    insertInstruction(instructionList, mov); 
}

void POP(struct InstructionList* instructionList, char reg[])
{
    char pop[20] = "POP ";
    strcat(pop, reg);
    strcat(pop, "\n");
    insertInstruction(instructionList, pop);
}

void PUSH(struct InstructionList* instructionList, char value[])
{
    char push[20] = "PUSH ";

    strcat(push, value);
    strcat(push, "\n");

    insertInstruction(instructionList, push);

}

void MULT(struct InstructionList* instructionList, char CX[], char AX[]) // Pops the last 2 items from the stack and 
{
    POP(instructionList, CX);
    POP(instructionList, AX);

    char mult[20] = "MUL ";

    strcat(mult, CX);
    strcat(mult, "\n");

    insertInstruction(instructionList, mult);

    PUSH(instructionList, AX);
}

void ADD(struct InstructionList* instructionList, char CX[], char AX[])
{
    POP(instructionList, CX);
    POP(instructionList, AX);

    char add[20] = "ADD ";

    strcat(add, AX);
    strcat(add, ",");
    strcat(add, CX);
    strcat(add, "\n");

    insertInstruction(instructionList, add);

    PUSH(instructionList, AX);
    
}

void DIV(struct InstructionList* instructionList, char CX[], char AX[])
{
    MOV(instructionList, "DX", "0");
    POP(instructionList, CX);
    POP(instructionList, AX);

    char div[20] = "DIV ";
    strcat(div, CX);
    strcat(div, "\n");
    insertInstruction(instructionList, div);

    PUSH(instructionList, AX);
}

struct InstructionList* convertPostfixToAssembly(char* postfixExpression) // Used to convert the passed in postfix expression into 8086 assembly code.
{
    struct InstructionList *instructionList = initList(0);

    int i, n;

    for(i = 0; postfixExpression[i] != '\0'; i++)
    {

        if(isxdigit(postfixExpression[i]))
        {  
            char operand[8];

            n = 0;

            operand[n] = postfixExpression[i];
            
            while(isxdigit(postfixExpression[i+1]))
            {
                i++;
                n++;

                operand[n] = postfixExpression[i];
            }

            n++;

            operand[n] = '\0';

            MOV(instructionList, "BX", operand);
            PUSH(instructionList, "BX");

            continue;
        }
        else
        {
                switch(postfixExpression[i])
                {

                case'+':
                {
                    ADD(instructionList, "CX", "AX");
                    continue;
                }
                case '/':
                {
                    DIV(instructionList, "CX", "AX");
                    continue;   
                }
                case '*':
                {
                    MULT(instructionList, "CX", "AX");
                    continue;
                }

                continue;
            }
        }
    
    }

    returnToOS(instructionList);

    strcat(instructionList->instructions, "\0");

    printf("\nGenerating 8086 Assembly Instructions: \n\n");

    printf("%s", instructionList->instructions);

    return instructionList;
}

// ==== Sub-routines to convert postfix expression into an assembly code. ====


// ==== Sub-routines to convert infix notation into postfix notation. ====

void freeArray(struct InstructionList *instructionList) 
{
  free(instructionList->instructions);
  instructionList->instructions = NULL;
  instructionList->used = instructionList->size = 0;
}

struct Stack *createStack(unsigned size) // Used to initialize the stack.
{
    struct Stack *stack; // stack is a pointer of the type struct Stack.

    stack = malloc(sizeof(struct Stack)); // space in memory is allocated for the size of the struct Stack and assigned to the variable stack.

    if (!stack) // If no memory was allocated, return NULL.
    {
        return NULL;
    }

    // Assign the variables for the struct stack.

    stack -> topOfStack = -1; // -1 is used to signify that the stack is empty because 0 is a valid index.
    stack -> size = size;
    stack -> stackArray = malloc(stack->size * sizeof(int)); // Allocate space in memory for the size of an integer times the size of the stack.

    return stack;
}

void freeStack(struct Stack *stack) // Used to free the memory allocated for the stack.
{
    free(stack->stackArray);

    free(stack);
}

int isVoid(struct Stack *stack) // Returns true if top of the stack is -1, signifying that it is void.
{
    return stack->topOfStack == -1;
}

char popStack(struct Stack *stack) // If the stack is not void, return the item on top.
{
    if (!isVoid(stack))
    {
        return stack->stackArray[stack->topOfStack--]; // Return the item on top of the stack and then decrement the stack counter.
    }
    return 'X'; // ! signifies than an error has occured and the stack
}

void pushStack(struct Stack *stack, char operand)
{
    stack->stackArray[++stack->topOfStack] = operand; // Increments the stackArray and assigns the operand to the top of the stack.
}

char checkTopItem(struct Stack *stack) // Used to check the item on top of the stackArray. Returns the item on top.
{
    return stack->stackArray[stack->topOfStack];
}

int checkPrecedence(char c) // Returns the precedence of operator that is passed in. If the operator does not exist, -1 is returned.
{
    switch (c)
    {
    case '+':
    {
        return 1;
    }
    case '-':
    {
        return 1;
    }
    case '*':
    case '/':
    {
        return 2;
    }
    }

    return -1;
}

int checkOperand(char c) // Function used to check if the passed in character is an operand, returns true or false.
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/*

The algorithm of the infix to postfix conversion works in the following way:

1) Iterate the expression from left the right.
2) If the scanned character is an operand, add it to the output (replace the current character in the expression).
3) Else
    3.1) If the top of the stack contains an opening parenthesis push it to the stackArray.
    3.2) If the current chracter is a closing parenthesis, pop from the stack and replace the current character of the 
         expression until an opening parenthesis is encountered and then both parenthesis are discarded
    3.3) Else, all the operators on the stack whose precedences are equal to or greater than the precedence of the current 
         operator are popped and added to output. Finally, the current operator is pushed to the stack.
4) Steps 2-3 are repeated until the whole infix expression is iterated.
5) Finally, the items are popped from the stack and added to the output until stack is empty. 

*/

int convertInfixToPostfix(char *inputExpression, char* outputExpression) // Used to convert infix notation to postfix notation. During this function the input expression is reassigned to become postfix expression.
{
    int i, k;

    struct Stack *stack = createStack(strlen(inputExpression));

    if (!stack) // Check if the stack was created successfully.
    {
        printf("\nStack could not be created!\n");
        return -1;
    }

    for (i = 0, k = -1; inputExpression[i]; ++i) // Iterate through the expression and check every single character.
    {
        if(inputExpression[i] == ' ')
        {
            continue;
        }

        if (isxdigit(inputExpression[i])) // If the character is an operand, add it to output by replacing the character of the expression.
        {
            outputExpression[++k] = inputExpression[i];  
        }
        else if (inputExpression[i] == '(') // If the current character is an opening parenthesis, push it to the stack.
        {
            pushStack(stack, inputExpression[i]);
        }
        else if (inputExpression[i] == ')') // Else if the current character is a closing parenthesis, pop from the stack and replace the character of the current expression until an opening parenthesis is encounterd.
        {
            while (!isVoid(stack) && checkTopItem(stack) != '(')
            {
                outputExpression[++k] = popStack(stack);
            }

            if (!isVoid(stack) && checkTopItem(stack) != '(')
            {
                printf("\nEncountered an invalid character!\n");
                return -1; 
            }

            else
            {
                popStack(stack);
            }
        }
        else // An operator must be encountered.
        {
            outputExpression[++k] = ' ';
            while (!isVoid(stack) && checkPrecedence(inputExpression[i]) < checkPrecedence(checkTopItem(stack))) // Encounterd operators on the stack whose precedences are equal to or greater than the precedence of the current operator are popped and added to output by replacing the character in the current expression.
            {
                outputExpression[++k] = popStack(stack);
            }

            pushStack(stack, inputExpression[i]); // The current operator is pushed to the stack.
        }
    }

    while (!isVoid(stack)) // Pop all the operators from the stack until the stack is empty.
    {
        outputExpression[++k] = popStack(stack);
    }

    outputExpression[++k] = '\0';

    printf("\nConverted Postfix Expression: %s\n", outputExpression);

    freeStack(stack);

    return 0;
}

// ==== Sub-routines to convert infix notation into postfix notation. ====


void writeAssemblyFile(char* assemblyInstructions) // Sub-routine used to extract assembly code into a text file.
{
    int i;

    FILE *fp;

    fp = fopen ("example.txt", "w");   

    for (i = 0; assemblyInstructions[i] != '\0'; i++) 
    {
        fputc(assemblyInstructions[i], fp);
    }

    fclose(fp);

    printf("\nA copy of these instructions can be found in a file in the same directory as the generated executable.\n");
}

void readInput(char *inputExpression)
{
    FILE *fp;

    fp = fopen("input.txt", "r");

    fgets(inputExpression, 100, fp);

    fclose(fp);

    printf("\nInput Infix Experssion: %s\n", inputExpression);
}


int main()
{
    // TODO: The program may obtain a text from input or a file.

    struct InstructionList* assemblyInstructions;

    char outputExpression[100];

    char inputExpression[100];

    readInput(inputExpression);

    convertInfixToPostfix(inputExpression, outputExpression); // The infix expression is converted into a postfix expression.

    assemblyInstructions = convertPostfixToAssembly(outputExpression); // Converts postfix expression into 8086 assembly code.

    writeAssemblyFile(assemblyInstructions->instructions); // Writes the 8086 assembly code into a text file located in the same folder as the program.
    
    return 0;
}