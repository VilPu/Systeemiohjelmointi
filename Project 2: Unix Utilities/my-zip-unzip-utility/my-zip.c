#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// checks if the opening of the file was successful if not throws an error and exits the program with code 1
// returns void
void checkFile(FILE *file)
{
    if (file == NULL)
    {
        fprintf(stdout, "wzip: cannot open file\n");
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
        if (c == EOF && strlen(line) == 0)
        {
            free(line);
            return NULL;
        }
        if (c == EOF)
        {
            line[index] = '\0'; // adding the null character to end the string properly
            return line;
        }
        
        line = realloc(line, (strlen(line) + sizeof(c)));
        checkMallocChar(line);
        line[index++] = (char)c;
    }
    line[index] = c;
    line[index + 1] = '\0'; // adding the null character to end the string properly
    return line;
}

void compress(char *line)
{
    int count = 0;
    int c = -1;
    size_t nitems = 1;
    for (int i = 0; i < strlen(line); i++)
    {
        // handle 1st char in line
        if (c == -1)
        {
            c = line[i];
            count++;
            continue;
        }
        // if char is the same
        if (c == line[i] && strlen(line) != i + 1)
        {
            count++;
        }
        // else when char not the same or at the EOF
        else
        {
            // add current char to count in case of EOF
            if (strlen(line) == i + 1)
            {
                count++;
            }
            char str[2];
            sprintf(str, "%c", c);
            fwrite(&count, sizeof(int), nitems, stdout);
            fwrite(str, sizeof(char), nitems, stdout);
            count = 1;
            c = line[i];
        }
    }
    
}

// reads a file line by line using the readLineCharbyChar function
// returns void
void readFile(FILE *file)
{
    checkFile(file);
    while (feof(file) == 0)
    {
        char *line;
        line = readLineCharbyChar(file);
        if (line == NULL)
        {
            break;
        }
        compress(line);
        
        if (ferror(stdout))
        {
            fprintf(stdout, "Error occurred writing file\n");
            free(line);
            break;
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
        fprintf(stdout, "wzip: file1 [file2 ...]\n");
        exit(1);
    case 2:
        fileName = (char *)argv[1];
        file = fopen(fileName, "r");
        readFile(file);
        fclose(file);
        break;
    default:
        for (int i = 1; i < argc; i++)
        {
            fileName = (char *)argv[i];
            file = fopen(fileName, "r");
            readFile(file);
            fclose(file);
        }
        break;
    }
    exit(0);
}
