#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// struct for the linked list
typedef struct link
{
    char *line;
    struct link *pNext;
    struct link *pPrevious;
} LINK;

// checks input and output files to check if they have the same name or are the same file
// inspiration to use stat from: https://stackoverflow.com/questions/15650488/determining-if-two-file-paths-point-to-same-file-under-linux-c
// returns void
void checkInputFileNames(const char *firstName, const char *secondName)
{
    struct stat pFirstNameBuf;
    struct stat pSecondNameBuf;

    stat(firstName, &pFirstNameBuf);
    stat(secondName, &pSecondNameBuf);
    if (pFirstNameBuf.st_ino == pSecondNameBuf.st_ino || strcmp(firstName, secondName) == 0)
    {
        fprintf(stderr, "reverse: input and output file must differ\n");
        exit(1);
    }
    return;
}

// checks if the opening of the file was successful if not throws an error and exits the program with code 1
// returns void
void checkFile(FILE *file, char *name)
{
    if (file == NULL)
    {
        fprintf(stderr, "reverse: cannot open file '%s'\n", name);
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
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    return;
}

// checks if the memory allocation for a struct link was successful if not throws an error and exits the program with code 1
// returns void
void checkMallocLink(LINK *input)
{
    if (input == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
}

// function to read a line, stops the reading at a newline character
// returns a string representing the line
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
    line[index] = '\0'; // adding the null character to end the string properly
    return line;
}

// reads a file line by line using the readLineCharbyChar function
// returns pointer to the start of the linked list 
LINK *readFile(FILE *file, LINK *pStart)
{
    LINK *addLink(LINK * pStart, char *line);

    while (feof(file) == 0)
    {
        char *line;
        line = readLineCharbyChar(file);
        if (line == NULL)
        {
            break;
        }

        pStart = addLink(pStart, line);
    }
    return pStart;
}

// adds a link to the linked list containing the file's line values
// returns pointer to the start of the linked list
LINK *addLink(LINK *pStart, char *line)
{
    LINK *getLastLink(LINK * pStart);

    LINK *pOriginalStart = pStart;
    LINK *pNew = (LINK *)malloc(sizeof(LINK) + strlen(line));
    checkMallocLink(pNew);
    pNew->line = line;
    pNew->pNext = NULL;

    if (pStart == NULL)
    {
        pStart = pNew;
        return pStart;
    }
    else
    {
        pStart = getLastLink(pStart);
        pStart->pNext = pNew;
        pNew->pPrevious = pStart;
    }
    return pOriginalStart;
}

// writes to the given output FILE be it stdout or a regular file
// can print regularly or reversed
// returns void
void writeToOutput(FILE *output, LINK *start, int reversed)
{
    LINK *getLastLink(LINK * pStart);
    LINK *pBuffer = NULL;
    if (reversed == 1)
    {
        start = getLastLink(start);
        while (start != NULL)
        {
            fprintf(output, "%s\n", start->line);
            pBuffer = start->pPrevious;
            start = pBuffer;
        }
    }
    else
    {
        while (start != NULL)
        {
            fprintf(output, "%s\n", start->line);
            pBuffer = start->pNext;
            start = pBuffer;
        }
    }
    return;
}

// traverses through the linked list and returns the last link of the linked list
// returns pointer to the linked list's last link can return NULL if pstart is NULL
LINK *getLastLink(LINK *pStart)
{
    while (pStart != NULL && pStart->pNext != NULL)
    {
        pStart = pStart->pNext;
    }
    return pStart;
}

// traverses through the linked list and frees all memory allocated to the links
// returns void
void freeList(LINK *pStart)
{
    LINK *pBuffer = NULL;
    while (pStart != NULL)
    {
        pBuffer = pStart->pNext;
        free(pStart->line);
        free(pStart);
        pStart = pBuffer;
    }
}

// main function
int main(int argc, char const *argv[])
{
    FILE *inputFile;
    FILE *outputFile = stdout;
    LINK *pStart = NULL;

    // switch-case for different command line arguments
    switch (argc)
    {
    case 1: // input from stdin and output to stdout
        inputFile = stdin;
        pStart = readFile(inputFile, pStart);
        break;
    case 2: // input from file and output to stdout
        inputFile = fopen(argv[1], "r");
        checkFile(inputFile, (char *)argv[1]);
        pStart = readFile(inputFile, pStart);
        break;
    case 3: // input from file and output to file
        inputFile = fopen(argv[1], "r");
        outputFile = fopen(argv[2], "w");
        checkFile(inputFile, (char *)argv[1]);
        checkFile(outputFile, (char *)argv[2]);
        checkInputFileNames(argv[1], argv[2]);
        pStart = readFile(inputFile, pStart);
        break;
    default:
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
        break;
    }
    writeToOutput(outputFile, pStart, 1); //hardcoded to make the output reversed
    freeList(pStart);
    fclose(inputFile);
    fclose(outputFile);
    exit(0);
}
