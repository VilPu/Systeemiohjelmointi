#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// checks if the opening of the file was successful if not throws an error and exits the program with code 1
// returns void
void checkFile(FILE *file)
{
    if (file == NULL)
    {
        fprintf(stdout, "wgrep: cannot open file\n");
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

// checks if the subString is empty
// returns 0 if subString is empty otherwise 1
int checkSubstring(char *subString)
{
    if (subString[0] == '\0')
    {
        return 0;
    }
    return 1;
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
        line = realloc(line, (strlen(line) + sizeof(c)));
        checkMallocChar(line);
        line[index++] = (char)c;
    }
    line[index] = c;
    line[index + 1] = '\0'; // adding the null character to end the string properly
    return line;
}

// reads a file line by line using the readLineCharbyChar function
// and tries to find the subString which is case sensitive.
// If subString is found the line is printed otherwise the line is skipped.
// returns void
void searchFile(FILE *file, char *subString)
{
    if (!checkSubstring(subString))
    {
        return;
    }

    // checks if file is opened
    checkFile(file);
    while (feof(file) == 0)
    {
        char *line;
        line = readLineCharbyChar(file);
        if (line == NULL)
        {
            break;
        }
        if (strstr(line, subString) != NULL)
        {
            fprintf(stdout, "%s", line);
        }
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
        fprintf(stdout, "wgrep: searchterm [file ...]\n");
        exit(1);
        break;
    case 2:
        searchFile(stdin, (char *)argv[1]);
        break;
    case 3:
        fileName = (char *)argv[2];
        file = fopen(fileName, "r");
        searchFile(file, (char *)argv[1]);
        fclose(file);
        break;
    default:
        for (int i = 2; i < argc; i++)
        {
            fileName = (char *)argv[i];
            file = fopen(fileName, "r");
            searchFile(file, (char *)argv[1]);
            fclose(file);
        }
        break;
    }
    exit(0);
}
