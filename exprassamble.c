#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// ==== Data structures. ====

/**
 * @brief 
 * A struct to serve as a stack when converting infix notation to postfix notation.
 */
struct Stack
{
    int *stackArray;
    int topOfStack;
    unsigned size;
};

/**
 * @brief 
 * A struct to serve as a list to store assembly instructions before being printed out.
 */
struct InstructionList
{
    char *instructions;
    unsigned size;
    unsigned used;
};

// ==== Sub-routines and functions to convert postfix expression into an assembly code. ====

/**
 * @brief 
 * Used to initialize an instance of the struct InstructionList.
 * @param initialSize 
 * @return struct InstructionList* 
 */
struct InstructionList *initList(unsigned initialSize)
{
    struct InstructionList *instructionList;

    instructionList = malloc(sizeof(struct InstructionList)); // Allocate space in memory for the struct InstructionList.

    if (!instructionList) // If no memory was allocated, return NULL.
    {
        return NULL;
    }

    // Assign variables for the struct instructionList.

    instructionList->size = initialSize;
    instructionList->used = 0;
    instructionList->instructions = malloc(instructionList->size * sizeof(int)); // Allocate space in memory for the size of a char times the size of the stack.

    return instructionList;
}

/**
 * @brief 
 * Inserts instructions into the given list while re-allocating memory for the instruction array and updates size and used.
 * @param instructionList 
 * @param inst 
 */
void insertInstruction(struct InstructionList *instructionList, char inst[])
{
    int i;

    for (i = 0; i < strlen(inst); i++)
    {
        if (instructionList->used == instructionList->size) // If the list is full, then new space should be made.
        {
            instructionList->size *= 2;                                                                                   // Increase size.
            instructionList->instructions = realloc(instructionList->instructions, instructionList->size * sizeof(char)); // Re-allocate memory.
        }

        instructionList->instructions[instructionList->used++] = inst[i]; // Add the new instructions to the instructions array.
    }
}

/**
 * @brief 
 * Appends return to OS instruction to the given instruction list.
 * @param instructionList 
 */
void returnToOS(struct InstructionList *instructionList)
{
    // The relevant string imitating the instruction of the operation is initialised.

    char interrupt[] = "INT 20h\n";

    insertInstruction(instructionList, interrupt); // Appends the generated instruction to the instruction list.
}

/**
 * @brief 
 * Appends MOV operation to the given instruction list with the given value and register.
 * @param instructionList 
 * @param reg 
 * @param value 
 */
void MOV(struct InstructionList *instructionList, char reg[], char value[])
{
    // The relevant string imitating the instruction of the operation is initalised and then the register value is concatenated.

    char mov[20] = "MOV ";

    strcat(mov, reg);
    strcat(mov, ",0x");
    strcat(mov, value);
    strcat(mov, "\n");

    insertInstruction(instructionList, mov); // Appends the generated instruction to the instruction list.
}

/**
 * @brief 
 * Appensd the print output instruction to the instruction list.
 * @param instructionList 
 */
void printOutput(struct InstructionList *instructionList)
{
    char mov[20] = "POP DX\n";

    char interrupt[] = "INT 21h\n";

    insertInstruction(instructionList, mov);
    insertInstruction(instructionList, interrupt);
}

/**
 * @brief 
 * Appends POP operation to the given instruction list and register.
 * @param instructionList 
 * @param reg 
 */
void POP(struct InstructionList *instructionList, char reg[])
{
    // The relevant string imitating the instruction of the operation is initialised and then the register value is concatenated.

    char pop[20] = "POP ";

    strcat(pop, reg);
    strcat(pop, "\n");

    insertInstruction(instructionList, pop); // Appends the generated instruction to the instruction list.
}

/**
 * @brief 
 * Appends PUSH operation to the given instruction list and register.
 * @param instructionList 
 * @param value 
 */
void PUSH(struct InstructionList *instructionList, char value[])
{
    // The relevant string imitating the instruction of the operation is initialised and then the register value is concatenated.

    char push[20] = "PUSH ";

    strcat(push, value);
    strcat(push, "\n");

    insertInstruction(instructionList, push); // Appends the generated instruction to the instruction list.
}

/**
 * @brief 
 * Appends MUL operation to the given instruction list with the given registers.
 * @param instructionList 
 * @param CX 
 * @param AX 
 */
void MUL(struct InstructionList *instructionList, char CX[], char AX[])
{
    // The MUL instruction should be performed on two registers that are on top of the stack, so the POP sub-routine is called twice, once for the given register and once for the accumulator register.

    POP(instructionList, CX);
    POP(instructionList, AX);

    // The relevant string imitating the instruction of the operation is initialised and then the register value is concatenated.

    char MUL[20] = "MUL ";

    strcat(MUL, CX);
    strcat(MUL, "\n");

    insertInstruction(instructionList, MUL); // Appends the generated instruction to the instruction list.

    PUSH(instructionList, AX); // The result of this operation which resides on the accumulator register is pushed on to the stack using PUSH sub-routine.
}

/**
 * @brief 
 * Appends ADD operation to the given instruction list with the given registers.
 * @param instructionList 
 * @param CX 
 * @param AX 
 */
void ADD(struct InstructionList *instructionList, char CX[], char AX[])
{
    // The ADD instruction should be performed on two registers that are on top of the stack, so the POP sub-routine is called twice, once for the given register and once for the accumulator register.

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

/**
 * @brief 
 * Appends DIV operation to the given instruction list with the given registers.
 * @param instructionList 
 * @param CX 
 * @param AX 
 */
void DIV(struct InstructionList *instructionList, char CX[], char AX[])
{
    MOV(instructionList, "DX", "0"); // 0 is moved to DX, to make sure that the division performed is word by word. (project limit is 16 bits)

    // The MUL instruction should be performed on two registers that are on top of the stack, so the POP sub-routine is called twice, once for the given register and once for the accumulator register.

    POP(instructionList, CX);
    POP(instructionList, AX);

    // The relevant operation instruction is generated and then the register value is concatenated.

    char div[20] = "DIV ";

    strcat(div, CX);
    strcat(div, "\n");

    insertInstruction(instructionList, div); // Appends the generated instruction to the instruction list.

    PUSH(instructionList, AX); // The result of this operation which resides on the accumulator register is pushed on to the stack PUSH sub-routine.
}

/**
 * @brief 
 * Used to free memory allocated for the instruction list.
 * @param instructionList 
 */
void freeArray(struct InstructionList *instructionList)
{
    free(instructionList->instructions);
    instructionList->instructions = NULL;
    instructionList->used = instructionList->size = 0;
}

/**
 * @brief 
 * Used to convert the passed in postfix expression into 8086 assembly code.
 * @param postfixExpression 
 * @param assemblerType 
 * @return struct InstructionList* 
 */
struct InstructionList *convertPostfixToAssembly(char *postfixExpression, int assemblerType)
{
    struct InstructionList *instructionList = initList(0); // Create the instruction list.

    int i, n;

    for (i = 0; postfixExpression[i] != '\0'; i++) // Parse until null character is reached.
    {

        if (isxdigit(postfixExpression[i])) // If the encountered character is an hexadecimal, place the character to the operand array.
        {
            char operand[8];

            n = 0;

            operand[n] = postfixExpression[i];

            while (isxdigit(postfixExpression[i + 1])) // If the next character is an operand as well, keep placing the operands on the array until a non-hexadecimal value is encountered.
            {
                i++;
                n++;

                operand[n] = postfixExpression[i];
            }

            n++;

            operand[n] = '\0'; // Add the null character to signfiy the end of the operand.

            MOV(instructionList, "BX", operand); // Store the operand on BX register.
            PUSH(instructionList, "BX");         // Push the BX register on to the stack so that the operand is placed on stack.

            continue;
        }
        else // If a non-hexadecimal character was encountered, it means that an operator must be encountered.
        {
            switch (postfixExpression[i]) // Depending on the type of the operator, call the relevant sub-routine with the register CX and the accumulator register AX as the parameters.
            {

            case '+':
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

    if (!assemblerType) // If an online assembler is used that does not support OS instructions, no OS instructions are added,
    {
        printOutput(instructionList);

        returnToOS(instructionList); // Append the return to OS instruction to the list of instructions.
    }

    strcat(instructionList->instructions, "\0"); // End with a null character to ensure the end of the list.

    printf("\nGenerating 8086 Assembly Instructions: \n\n");

    printf("%s", instructionList->instructions); // Print the instructions into the console.

    return instructionList; // Return the generated instruction list to the main function so that it can be used to be written onto a text file or other.
}

// ==== Sub-routines to convert infix notation into postfix notation. ====

/**
 * @brief 
 * Used to initialize the stack.
 * @param size 
 * @return struct Stack* 
 */
struct Stack *createStack(unsigned size)
{
    struct Stack *stack; // Stack is a pointer of the type struct Stack.

    stack = malloc(sizeof(struct Stack)); // Space in memory is allocated for the size of the struct Stack and assigned to the variable stack.

    if (!stack) // If no memory was allocated, return NULL.
    {
        return NULL;
    }

    // Assign the variables for the struct stack.

    stack->topOfStack = -1; // -1 is used to signify that the stack is empty because 0 is a valid index for the stack.
    stack->size = size;
    stack->stackArray = malloc(stack->size * sizeof(int)); // Allocate space in memory for the size of an integer times the size of the stack.

    return stack;
}

/**
 * @brief 
 * Used to free the memory allocated for the stack.
 * @param stack 
 */
void freeStack(struct Stack *stack)
{
    free(stack->stackArray);

    free(stack);
}

/**
 * @brief 
 * Checks if the top of the stack is void.
 * @param stack 
 * @return true if top of the stack is -1, signifying that it is void.
 */
int isVoid(struct Stack *stack)
{
    return stack->topOfStack == -1;
}

/**
 * @brief 
 * Pop an item from the stack. If the stack is not void, return the item on top.
 * @param stack 
 * @return char 
 */
char popStack(struct Stack *stack)
{
    if (!isVoid(stack))
    {
        return stack->stackArray[stack->topOfStack--]; // Return the item on top of the stack and then decrement the stack counter.
    }
    return 'X'; // Signifies that an error has occured.
}

/**
 * @brief 
 * Push an item on the stack.
 * @param stack 
 * @param operand 
 */
void pushStack(struct Stack *stack, char operand)
{
    stack->stackArray[++stack->topOfStack] = operand; // Increments the stackArray and assigns the operand to the top of the stack.
}

/**
 * @brief 
 * Used to check the item on top of the stackArray. Used for peeking.
 * @param stack 
 * @return the item on top.
 */
char checkTopItem(struct Stack *stack)
{
    return stack->stackArray[stack->topOfStack];
}

/**
 * @brief 
 * Checks the precedence of the given character.
 * @param c 
 * @return the precedence of operator that is passed in. If the operator does not exist, -1 is returned. 
 */
int checkPrecedence(char c)
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

/**
 * @brief 
 * Function used to check if the passed in character is an operand.
 * @param c 
 * @return boolean 
 */
int checkOperand(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * @brief 
 * Used to convert infix notation to postfix notation. Accepts the input expression to read and the output experssion to write to as parameters.
 * In order to seperate operands while evaluating the postfix expression into assembly code, spaces are added between operands durıng infix to postfix conversıon.
 * @param inputExpression 
 * @param outputExpression 
 * @return int 
 */
int convertInfixToPostfix(char *inputExpression, char *outputExpression)
{
    int i, k;

    struct Stack *stack = createStack(strlen(inputExpression));

    if (!stack) // Check if the stack was created successfully.
    {
        printf("\nStack could not be created!\n");
        return -1;
    }

    for (i = 0, k = -1; inputExpression[i]; ++i) // Iterate through the expression left to right and check every single character.
    {
        if (inputExpression[i] == ' ') // Ignore empty spaces.
        {
            continue;
        }

        if (isxdigit(inputExpression[i])) // If the character is an operand, add it to the output expression.
        {
            outputExpression[++k] = inputExpression[i];
        }
        else if (inputExpression[i] == '(') // Else, if the current character is an opening parenthesis, push it to the stack.
        {
            pushStack(stack, inputExpression[i]);
        }
        else if (inputExpression[i] == ')') // If a closing paranthesis is encountered, stack must be cleared and necessary operands/operators must be added to the output expression.
        {
            while (!isVoid(stack) && checkTopItem(stack) != '(') // If an opening paranthesis is not on top of the paranthesis yet, then there must be operands or operators that should be written to output expression.
            {
                outputExpression[++k] = popStack(stack); // Pop and add operands/operators into the output expression.
            }

            popStack(stack); // Pop the encountered paranthesis to be removed from the stack.
        }
        else // An operator must have been encountered if no operand is encountered so far.
        {
            outputExpression[++k] = ' '; // If an operator is encountered, to seperate operands from one another, leave a space between them.

            // While the precedence of the operator in the input expression is lesser than the operators on the stack, pop all operators on the stack and add to the output expression.

            while (!isVoid(stack) && checkPrecedence(inputExpression[i]) < checkPrecedence(checkTopItem(stack)))
            {
                outputExpression[++k] = popStack(stack);
            }

            pushStack(stack, inputExpression[i]); // Finally, the current operator is pushed to the stack.
        }
    }

    while (!isVoid(stack)) // Now that the iteration of the expression is over, pop all the operators from the stack until the stack is empty.
    {
        outputExpression[++k] = popStack(stack);
    }

    outputExpression[++k] = '\0'; // Add the null character to represent the end of the expression.

    printf("\nConverted Postfix Expression: %s\n", outputExpression);

    freeStack(stack); // Free the memory allocated for the stack.

    return 0;
}

// ==== Sub-routines to receive input from user and print the output into a text file. ====

/**
 * @brief 
 * Prints the contents of the instruction list array which contain assembly instructions into a text file.
 * @param assemblyInstructions 
 * @param outputFormat 
 */
void writeAssemblyFile(char *assemblyInstructions, int outputFormat)
{
    int i;

    FILE *fp;

    if (outputFormat)
    {
        fp = fopen("result.txt", "w");
    }
    else
    {
        fp = fopen("result.asm", "w");
    }

    for (i = 0; assemblyInstructions[i] != '\0'; i++)
    {
        fputc(assemblyInstructions[i], fp);
    }

    fclose(fp);

    printf("\nA copy of these instructions can be found in a file in the same directory as the generated executable.\n");
}

/**
 * @brief 
 * eads the input.txt file generated by the user and places its contents into inputExpression char array.
 * @param inputExpression 
 */
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
    int outputFormat = -1; // 0 for native, 1 for online.

    int assemblerType = -1; // 0 for native, 1 for online.

    struct InstructionList *assemblyInstructions; // Initialize the struct to store generated assembly instructions.

    char outputExpression[100]; // Char array to store output expression that contains the postfix expression.

    char inputExpression[100]; // Char array to store infix experession input of the user.

    while (outputFormat != 0 || outputFormat != 1)
    {
        printf("\nPlease choose the output file extension: \n");
        printf("0- .asm\n");
        printf("1- .txt\n");
        scanf("%d", &outputFormat);

        if (outputFormat == 0 || outputFormat == 1)
        {
            break;
        }
    }

    while (assemblerType != 0 || assemblerType != 1)
    {
        printf("\nPlease choose the assembler type. Online 8086 assembly emulator may not include OS commands.: \n");
        printf("0- Native\n");
        printf("1- Online\n");
        scanf("%d", &assemblerType);

        if (assemblerType == 0 || assemblerType == 1)
        {
            break;
        }
    }

    readInput(inputExpression); // Reads input from the input file created by the user in the same directory.

    convertInfixToPostfix(inputExpression, outputExpression); // The infix expression is converted into a postfix expression.

    assemblyInstructions = convertPostfixToAssembly(outputExpression, assemblerType); // Converts postfix expression into 8086 assembly code.

    writeAssemblyFile(assemblyInstructions->instructions, outputFormat); // Writes the 8086 assembly code into a text file located in the same folder as the program.

    freeArray(assemblyInstructions); // Free the allocated memory for the generated struct.

    return 0;
}