#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// checks if the opening of the file was successful if not throws an error and exits the program with code 1
// returns void
void checkFile(FILE *file, char *name)
{
    if (file == NULL)
    {
        fprintf(stdout, "wcat: cannot open file\n");
        exit(1);
    }
    return;
}

// checks if the memory allocation for a char was successful if not throws an error and exits the program with code 1
// returns void
void checkMallocChar(char *input)
{
    if (input == NULL)
    {
        fprintf(stdout, "malloc failed\n");
        exit(1);
    }
    return;
}

// function to read a line, stops the reading at a newline character
// returns line in case of successful line read, NULL at EOF
char *readLineCharbyChar(FILE *file)
{
    int c;
    int index = 0;
    char *line = (char *)malloc(2);
    checkMallocChar(line);

    while ((c = getc(file)) != 10) // in ASCII 10 == '\n'
    {
        if (c == EOF)
        {
            free(line);
            return NULL;
        }
        line = realloc(line, (strlen(line) + 1));
        checkMallocChar(line);
        line[index++] = (char)c;
    }
    line[index] = c;
    line[index + 1] = '\0'; // adding the null character to end the string properly
    return line;
}

// reads a file line by line using the readLineCharbyChar function
// returns void 
void readFile(FILE *file)
{
    while (feof(file) == 0)
    {
        char *line;
        line = readLineCharbyChar(file);
        if (line == NULL)
        {
            break;
        }
        fprintf(stdout, "%s", line);
        free(line);
    }
    return;
}

int main(int argc, char const *argv[])
{
    FILE *file;
    char *fileName = NULL;

    // switch-case for different command line arguments
    switch (argc)
    {
    case 1:
        exit(0);
    case 2:
        fileName = (char *)argv[1];
        file = fopen(fileName, "r");
        checkFile(file, fileName);
        readFile(file);
        fclose(file);
        break;
    default:
        for (int i = 1; i < argc; i++)
        {
            fileName = (char *)argv[i];
            file = fopen(fileName, "r");
            checkFile(file, fileName);
            readFile(file);
            fclose(file);
        }
        break;
    }
    exit(0);
}
