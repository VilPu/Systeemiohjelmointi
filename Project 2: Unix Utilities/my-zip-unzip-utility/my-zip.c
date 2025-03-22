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

// writes a 5 byte entry to stdout that consists of count(int) and c(ASCII char)
// returns void
void writeBinary(int count, int c)
{
    fwrite(&count, sizeof(int), 1, stdout);
    fwrite(&c, sizeof(char), 1, stdout);
}

// compresses a string with run-length encoding (RLE)
// returns void
void compress(char *content)
{
    int count = 0;
    int c = -1;
    long lineSize = strlen(content);

    for (int i = 0; i < lineSize; i++)
    {
        // handle 1st char in content
        if (c == -1)
        {
            c = content[i];
            count++;
            // edge case where content has length of 1
            if (i + 1 == lineSize)
            {
                writeBinary(count, c);
            }
            continue;
        }
        // if char is the same and not last char in content
        if (c == content[i] && lineSize != i + 1)
        {
            count++;
        }
        // else when char not the same or at the last char in content
        else
        {
            // edge case if only 1 line of text and no newline at end of file
            if (c == content[i])
            {
                count++;
            }
            writeBinary(count, c);

            // edge case if the last char in content and the char is a newline '\n'
            if (strlen(content) == i + 1 && content[i] == 10) // ASCII 10 = '\n'
            {
                c = content[i];
                writeBinary(1, c);
            }
            count = 1;
            c = content[i];
        }
    }
}

// reads a file as a whole and saves it to content
// returns pointer to file(s) content
char *readFile(FILE *file, char *content)
{
    checkFile(file);
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    content = realloc(content, (strlen(content) + size));
    char *buffer = malloc(size);
    checkMallocChar(content);
    checkMallocChar(buffer);
    fread(buffer, size, 1, file);
    strcat(content, buffer);
    return content;
}

int main(int argc, char const *argv[])
{
    FILE *file;
    char *fileName = NULL;
    char *content = malloc(3);
    checkMallocChar(content);

    // switch-case for different command line arguments
    switch (argc)
    {
    case 1:
        free(content);
        fprintf(stdout, "wzip: file1 [file2 ...]\n");
        exit(1);
    case 2:
        fileName = (char *)argv[1];
        file = fopen(fileName, "r");
        content = readFile(file, content);
        fclose(file);
        break;
    default:
        for (int i = 1; i < argc; i++)
        {
            fileName = (char *)argv[i];
            file = fopen(fileName, "r");
            content = readFile(file, content);
            fclose(file);
        }
        break;
    }
    compress(content);
    free(content);
    exit(0);
}
