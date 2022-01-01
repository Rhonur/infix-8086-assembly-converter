Overview:

This application was developed for a course to convert infix expressions inside a text file to 8086 assembly code and print the results inside another text file.

Operation:

1. Generate a text file for input in the same folder as the C code. The input file should contain an infix expression that is to be used as an input. The file name should be “input.txt”.

2. Compile and run the code.

3. Chose what type of 8086 assembly emulator will be used, whether online or native. The online emulator that was used while developing this project does not include OS commands. For example, online assembly emulator does not have the option to output and results in errors if OS commands are encountered. Choosing an online emulator ignores the OS commands in the output file.

4. The program prints the generated assembly code into the terminal and generates a file “result.txt” which contains the generated assembly code. The converted postfix expression is printed out on to the terminal as well.

5. Run the generated assembly code using an assembler. During the development and testing of this project, the following online 8086 emulator was used: "https://yjdoc2.github.io/8086-emulator-web/compile".
