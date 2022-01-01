Overview:

This application was developed for a course to convert infix expressions inside a text file to 8086 assembly code and print the results inside another text file.

This program consists of 2 main functions and 2 sub-routines, which are called in the main function. All of these functions and sub-routines were developed and can be used separately, which provides modularity and re-usability.

Two of these sub-routines, readInput and writeAssemblyFile are responsible of receiving input from the user and producing an output for the user by generating a text file containing the generated assembly code.

The convertInfixToPostfix and convertPostfixToAssembly functions call several smaller sub-routines to perform conversion from infix to postfix notation and then generate assembly code with the converted postfix expression.

Operation:

1. Generate a text file for input in the same folder as the C program. The input file should contain an infix expression that is to be used as an input. The file name should be “input.txt”.

2. Use a compiler to generate the compiled program. (For example if Clang is the compiler, run “ clang exprassamble.c”)

3. Run the compiled code using “./a.out”.

4. Chose the output file extension, it can either be a .txt file or an .asm file.

5. Chose what type of 8086 assembly emulator will be used, whether online or native. The online emulator that was used while developing this project does not include OS commands. For example, online assembly emulator does not have the option to output. Choosing an online emulator ignores the OS commands in the output file.

6. The program prints the generated assembly code into the terminal and generates a file “result.txt” which contains the generated assembly code. The converted postfix expression is printed out on to the terminal as well.

7. Run the generated assembly code using an assembler. During the development and testing of this project, the following online 8086 emulator was used: "https://yjdoc2.github.io/8086-emulator-web/compile".
