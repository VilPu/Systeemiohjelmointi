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

void uncompress(char str[])
{
    int count = str[0] - '0';
    
    for (int i = 0; i < count; i++)
    {
        printf("%c", str[1]);
    }
    
}

// reads a file line by line using the readLineCharbyChar function
// returns void
void readFile(FILE *file)
{
    int test;
    size_t bytesRead;

    checkFile(file);
    while ((bytesRead = fread(&test, sizeof(int), 1, file)) > 0)
    {
        char c;
        fread(&c, 1, 1, file);
        for (size_t i = 0; i < test; i++)
        {
            printf("%c", c);
        }
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
        fprintf(stdout, "wunzip: file1 [file2 ...]\n");
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
