#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct link
{
    char *line;
    struct link *pNext;
    struct link *pPrevious;
} LINK;

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

void checkFile(FILE *file, char *name)
{
    if (file == NULL)
    {
        fprintf(stderr, "reverse: cannot open file '%s'\n", name);
        exit(1);
    }
}

void checkMallocChar(char *input)
{
    if (input == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
}

void checkMallocLink(LINK *input)
{
    if (input == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
}

char *readLineCharbyChar(FILE *file)
{
    int c;
    int index = 0;
    char *line = (char *)malloc(2);
    checkMallocChar(line);

    while ((c = getc(file)) != 10)
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
    line[index] = '\0';
    return line;
}

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
            fprintf(output, "LINK: %.30s\n", start->line);
            pBuffer = start->pNext;
            start = pBuffer;
        }
    }
}

LINK *getLastLink(LINK *pStart)
{
    while (pStart != NULL && pStart->pNext != NULL)
    {
        pStart = pStart->pNext;
    }
    return pStart;
}

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

int main(int argc, char const *argv[])
{
    FILE *inputFile;
    FILE *outputFile = stdout;
    LINK *pStart = NULL;

    switch (argc)
    {
    case 1:
        inputFile = stdin;
        pStart = readFile(inputFile, pStart);
        break;
    case 2:
        inputFile = fopen(argv[1], "r");
        checkFile(inputFile, (char *)argv[1]);
        pStart = readFile(inputFile, pStart);
        break;
    case 3:
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
    writeToOutput(outputFile, pStart, 1);
    freeList(pStart);
    fclose(inputFile);
    fclose(outputFile);
    exit(0);
}
