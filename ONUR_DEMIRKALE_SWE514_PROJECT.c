#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// ==== Data structures. ====

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

// ==== Data structures. ====


// ==== Sub-routines and functions to convert postfix expression into an assembly code. ====

struct InstructionList *initList(unsigned initialSize) // Used to initialize an instance of the struct InstructionList.
{
    struct InstructionList *instructionList; 

    instructionList = malloc(sizeof(struct InstructionList)); // Allocate space in memory for the struct InstructionList.

    if (!instructionList) // If no memory was allocated, return NULL.
    {
        return NULL;
    }

    // Assign variables for the struct instructionList.

    instructionList -> size = initialSize;
    instructionList -> used = 0; // Signifies that no space is used in the list so far.
    instructionList -> instructions = malloc(instructionList->size * sizeof(int)); // Allocate space in memory for the size of a char times the size of the stack.

    return instructionList;
}

void insertInstruction(struct InstructionList *instructionList, char inst[]) // Inserts instructions into the given list while re-allocating memory for the instruction array and updates size and used.
{
    int i;

    for(i = 0; i < strlen(inst); i++)
    {
        if (instructionList->used == instructionList->size) // If the list is full, then new space should be made.
        {
            instructionList->size *= 2; // Increase size.
            instructionList->instructions = realloc(instructionList->instructions, instructionList->size * sizeof(char)); // Re-allocate memory.
        }

        instructionList->instructions[instructionList->used++] = inst[i]; // Add the new instructions to the instructions array.
    }
}

void returnToOS(struct InstructionList* instructionList) // Appends return to OS instruction to the given instruction list.
{
    // The relevant string imitating the instruction of the operation is initialised.

    char interrupt[] = "INT 20h\n";

    insertInstruction(instructionList, interrupt); // Appends the generated instruction to the instruction list.
}

void MOV(struct InstructionList* instructionList, char reg[], char value[]) // Appends MOV operation to the given instruction list with the given value and register.
{
    // The relevant string imitating the instruction of the operation is initalised and then the register value is concatenated.

    char mov[20] = "MOV ";

    strcat(mov, reg);
    strcat(mov, ",0x");
    strcat(mov, value);
    strcat(mov, "\n");

    insertInstruction(instructionList, mov); // Appends the generated instruction to the instruction list.
}

void POP(struct InstructionList* instructionList, char reg[]) // Appends POP operation to the given instruction list and register.
{
    // The relevant string imitating the instruction of the operation is initialised and then the register value is concatenated.

    char pop[20] = "POP ";

    strcat(pop, reg);
    strcat(pop, "\n");

    insertInstruction(instructionList, pop); // Appends the generated instruction to the instruction list.
}

void PUSH(struct InstructionList* instructionList, char value[]) // Appends PUSH operation to the given instruction list and register.
{
    // The relevant string imitating the instruction of the operation is initialised and then the register value is concatenated.

    char push[20] = "PUSH ";

    strcat(push, value);
    strcat(push, "\n");

    insertInstruction(instructionList, push); // Appends the generated instruction to the instruction list.

}

void MUL(struct InstructionList* instructionList, char CX[], char AX[]) // Appends MUL operation to the given instruction list with the given registers.
{
    // The MUL instruction should be performed on two registers that are on top of the stack, so the POP operation is called twice, once for the given register and once for the accumulator register.

    POP(instructionList, CX);
    POP(instructionList, AX);

    // The relevant string imitating the instruction of the operation is initialised and then the register value is concatenated.

    char MUL[20] = "MUL ";

    strcat(MUL, CX);
    strcat(MUL, "\n");

    insertInstruction(instructionList, MUL); // Appends the generated instruction to the instruction list.

    PUSH(instructionList, AX); // The result of this operation which resides on the accumulator register is pushed on to the stack using PUSH sub-routine.
    
}
 
void ADD(struct InstructionList* instructionList, char CX[], char AX[]) // Appends ADD operation to the given instruction list with the given registers.
{
    // The ADD instruction should be performed on two registers that are on top of the stack, so the POP operation is called twice, once for the given register and once for the accumulator register.

    POP(instructionList, CX);
    POP(instructionList, AX);

    // The relevant ostring imitating the instruction of the operation is initialised and then the register value is concatenated.

    char add[20] = "ADD ";

    strcat(add, AX);
    strcat(add, ",");
    strcat(add, CX);
    strcat(add, "\n");

    insertInstruction(instructionList, add); // Appends the generated instruction to the instruction list.

    PUSH(instructionList, AX); // The result of this operation which resides on the accumulator register is pushed on to the stack PUSH sub-routine.
    
}

void DIV(struct InstructionList* instructionList, char CX[], char AX[])
{
    MOV(instructionList, "DX", "0"); // 0 is moved to DX, to make sure that the division performed is word by word. (project limit is 16 bits)

    // The MUL instruction should be performed on two registers that are on top of the stack, so the POP operation is called twice, once for the given register and once for the accumulator register.

    POP(instructionList, CX);
    POP(instructionList, AX);

    // The relevant operation instruction is generated and then the register value is concatenated.

    char div[20] = "DIV ";

    strcat(div, CX);
    strcat(div, "\n");

    insertInstruction(instructionList, div); // Appends the generated instruction to the instruction list.

    PUSH(instructionList, AX); // The result of this operation which resides on the accumulator register is pushed on to the stack PUSH sub-routine.
}

void freeArray(struct InstructionList *instructionList) // Used to free memory allocated for the instruction list.
{
  free(instructionList->instructions);
  instructionList->instructions = NULL;
  instructionList->used = instructionList->size = 0;
}

struct InstructionList* convertPostfixToAssembly(char* postfixExpression) // Used to convert the passed in postfix expression into 8086 assembly code.
{
    struct InstructionList *instructionList = initList(0); // Create the instruction list.

    int i, n;

    for(i = 0; postfixExpression[i] != '\0'; i++) // Parse until null character is reached.
    {

        if(isxdigit(postfixExpression[i])) // If the encountered character is an hexadecimal, place the character to the operand array.
        {  
            char operand[8];

            n = 0;

            operand[n] = postfixExpression[i];
            
            while(isxdigit(postfixExpression[i+1])) // If the next character is an operand as well, keep placing the operands until a non-hexadecimal value is encountered.
            {
                i++;
                n++;

                operand[n] = postfixExpression[i];
            }

            n++;

            operand[n] = '\0'; // Add the null character to signfiy the end of the operand.

            MOV(instructionList, "BX", operand); // Store the operand on BX register.
            PUSH(instructionList, "BX"); // Push the BX register on to the stack so that the operand is placed on stack.

            continue;
        }
        else // If a non-hexadecimal character was encountered, it means that an operator must be encountered.
        {
                switch(postfixExpression[i]) // Depending on the type of the operator, call the relevant sub-routine with the register CX and the accumulator register AX as the parameters.
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
                    MUL(instructionList, "CX", "AX");
                    continue;
                }

                continue;
            }
        }
    
    }

    returnToOS(instructionList); // Append the return to OS instruction to the list of instructions.

    strcat(instructionList->instructions, "\0"); // End with a null character to ensure the end of the list.

    printf("\nGenerating 8086 Assembly Instructions: \n\n"); 

    printf("%s", instructionList->instructions); // Print the instructions into the console.

    return instructionList; // Return the generated instruction list to the main function so that it can be used to be written onto a text file or other.
}

// ==== Sub-routines to convert postfix expression into an assembly code. ====


// ==== Sub-routines to convert infix notation into postfix notation. ====

struct Stack *createStack(unsigned size) // Used to initialize the stack.
{
    struct Stack *stack; // Stack is a pointer of the type struct Stack.

    stack = malloc(sizeof(struct Stack)); // Space in memory is allocated for the size of the struct Stack and assigned to the variable stack.

    if (!stack) // If no memory was allocated, return NULL.
    {
        return NULL;
    }

    // Assign the variables for the struct stack.

    stack -> topOfStack = -1; // -1 is used to signify that the stack is empty because 0 is a valid index for the stack.
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

char popStack(struct Stack *stack) // Pop an item from the stack. If the stack is not void, return the item on top.
{
    if (!isVoid(stack))
    {
        return stack->stackArray[stack->topOfStack--]; // Return the item on top of the stack and then decrement the stack counter.
    }
    return 'X'; // ! signifies than an error has occured and the stack
}

void pushStack(struct Stack *stack, char operand) // Push an item on the stack.
{
    stack->stackArray[++stack->topOfStack] = operand; // Increments the stackArray and assigns the operand to the top of the stack.
}

char checkTopItem(struct Stack *stack) // Used to check the item on top of the stackArray. Returns the item on top. Used for peeking.
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

//In order to seperate operands while evaluating the postfix expression into assembly code, spaces are added between operands durıng infix to postfix conversıon.

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
        else // An operator must have been encountered if no operand is encountered so far.
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

    outputExpression[++k] = '\0'; // Add the null character to represent the end of the expression.

    printf("\nConverted Postfix Expression: %s\n", outputExpression);

    freeStack(stack); // Free the memory allocated for the stack.

    return 0;
}

// ==== Sub-routines and functions to convert infix notation into postfix notation. ====

// ==== Sub-routines to receive input from user and print the output into a text file. ====

void writeAssemblyFile(char* assemblyInstructions) // Prints the contents of the instruction list array which contain assembly instructions into a text file.
{
    int i;

    FILE *fp;

    fp = fopen ("result.txt", "w");   

    for (i = 0; assemblyInstructions[i] != '\0'; i++) 
    {
        fputc(assemblyInstructions[i], fp);
    }

    fclose(fp);

    printf("\nA copy of these instructions can be found in a file in the same directory as the generated executable.\n");
}

void readInput(char *inputExpression) // Reads the input.txt file generated by the user and places its contents into inputExpression char array.
{
    FILE *fp;

    fp = fopen("input.txt", "r");

    fgets(inputExpression, 100, fp);

    fclose(fp);

    printf("\nInput Infix Experssion: %s\n", inputExpression);
}

// ==== Sub-routines to receive input from user and print the output into a text file. ====

int main()
{
    struct InstructionList* assemblyInstructions; // Initialize the struct to store generated assembly instructions.

    char outputExpression[100]; // Char array to store output expression that contains the postfix expression.

    char inputExpression[100]; // Char array to store infix experession input of the user.

    readInput(inputExpression); // Reads input from the input file created by the user in the same directory.

    convertInfixToPostfix(inputExpression, outputExpression); // The infix expression is converted into a postfix expression.

    assemblyInstructions = convertPostfixToAssembly(outputExpression); // Converts postfix expression into 8086 assembly code.

    writeAssemblyFile(assemblyInstructions->instructions); // Writes the 8086 assembly code into a text file located in the same folder as the program.
    
    freeArray(assemblyInstructions); // Free the allocated memory for the generated struct.

    return 0;
}