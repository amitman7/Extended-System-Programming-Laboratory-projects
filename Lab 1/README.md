# Overview:

The objective of this assignment was to provide an introduction to C programming in a Unix (Linux 32 bits) environment. The assignment focused on creating an encoder with four main features:

1. Debug mode: if the +D to turn it on and -D to turn it off
For example:
./encoder +D this is my first linux project -D
this is my first linux project

2. Encoder for Sentences:
   
For example:
./encoder +e12345
ABCDEZ
BDFHJA
^D

3. Takes Sentences from a File:
The program reads sentences from a file, encodes them, and prints the results.

4. Takes Sentences from the User and Encodes Them to a File:
The encoder takes input sentences from the user, encodes them, and writes the encoded sentences to a file.

# instructions:

# Part 1: Command-Line Arguments and Debugging
First, re-read and understand the arguments of main(argc, argv), which represent the command-line arguments in the line used to run any program using a "console". Recall that argv is the number of arguments, and that argv is an array of pointers to locations containing "null terminated strings" - the command line arguments, with argv[0] pointing to the program file name used in the command line to run the program.
Second, introduce a debug mode into your program. For this we will develop an easy debugging scheme which can be used with any program and allows for special debugging features for testing. The minimum implementation prints out important information to stderr when in debug mode. Printing out the command-line parameters allows for easy detection of errors in retrieving them. Henceforth, code you write in most labs and assignments will also require adding a debug mode, and it is a good idea to have this option in all programs you write, even if not required to do so!

For this scheme, you must simply loop over the command-line arguments, and if in debug mode, print each argument on a separate "line" to stderr. Debug mode is a variable that you can choose to initialize to "on" or "off" (default: off), but if there is a command line argument "+D" it turns debug mode on, and if there is a command-line argument "-D" it turns the debug mode off. Use fprintf( ) -- see manual -- for simple printing of "strings" on separate lines. Note, that the output should be to stderr, rather than stdout, to differentiate between regular program output (currently null) and debug/error messages from the program.

# Part 2: The Encoder
In this part you will first use the command-line parsing to detect a possible encoding string, and use that to modify the output behavior. With no encoding string, every input character (from stdin) is simply sent to the output (stdout). That is, you read a character using fgetc( ), possibly encode it, and then print it after modification using fputc( ), until detecting an EOF condition in the input, at which point you should close the output stream and exit "normally". We recommend here that you use variables such as infile and outfile as arguments to fgetc() and fputc() respectively, initialized by default to stdin and stdout, respectively. This will allow you to do the last part with very little effort.
The encoding works as follows. The encryption key is of the following structure: +e{key}. The argument {key} stands for a sequence of digits whose value will be added to each input characters in sequence, in a cyclic manner.
This means that each digit in sequence received by the encoder is added to the corresponding character in sequence in the key. When and if the end of the key is reached, re-start reading encoding digits from the beginning of the key. You should support both addition and subtraction, +e{key} is for addition and -e{key} is for subtraction.

Implementation is as follows. The key value, if any, is provided as a command-line argument. As stated above, this is indicated by a command line argument such as "+e1234" or "-e13061". The first is a sequence of numbers to be added to the input characters before they are emitted, while the second is a sequence of numbers to be subtracted from the input characters. Assumptions are: only at most one of "+e" or "-e" are present, and the rest of the command line argument is always (only) a non-empty sequence of decimal digits, terminated as usual by a null character.

Encoding is as follows: to the first character of the input, add the numerical value of the first encoding digit, to the second input character add the (numerical value of the) second digit, etc. If you reach the end of the encoding string (null character!) before you reach EOF in the input, reset to the beginning of the encoding string. Observe that this is ASCII encoding, so it should be very simple to compute the numeric value of each digit, which you should do directly using no special functions. Note that we advance in the encoding key once for each input character, but encoding, if indicated, should only be applied to alpha-numeric characters, that is 0-9, A-Z, and a-z, and should use "wrap around", that is assume z+1 is a, and A-1 is Z. etc. Examples are provided below to fully clarify this.

# Part 3: Input and/or Output to Specific Files
The default operation of your program is to read characters from stdin (the "console" keyboard), encode them as needed, and output the results to stdout (the "console" display). After checking correctness of all the previous parts, now add the option for reading the input from a specified input file: if command-line argument "-ifname" is present, the input should be read from a file called "fname" instead of stdin (or in general the file name starts immediately after the "-i" and ends at the null character). Likewise, if command-line argument "-ofname" is present, the output should go to a file name "fname" (or in general, file name immediately after the "o").

Observe that if you did things right and heeded our advice above, this part is only a few lines of code: while scanning the command-line arguments simply check for "-i" and "-o" and open input and/or output files as needed using fopen( ), and use the file descriptor it returns for the value of "infile" and/or "outfile". The rest of the program does not need to change at all. Just make sure that if fopen( ) fails, print an error message to stderr and exit. Note that your program should support encoding keys, input file setting, output file setting, and debug flag setting, in any combination or order. You may assume that at most one of each will be given (e.g. no more than one encoding key, and no more than one output file setting).

