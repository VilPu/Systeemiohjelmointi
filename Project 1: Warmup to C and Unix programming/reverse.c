#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct link
{
    char * line;
    struct link * pNext;
    struct link * pNext;

} LINK;

void checkInputFileNames(const char *firstName, const char *secondName)
{
    if (strcmp(firstName, secondName) == 0)
    {
        fprintf(stderr, "Input and output file must differ\n");
        exit(1);
    }
    return;
}

void checkFile(FILE * file, char * name) 
{
    if (file == NULL)
    {
        fprintf(stderr, "error: cannot open file '%s'\n", name);
        exit(1);
    }
        
}

void checkMalloc(char * input) 
{
    if (input == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    
}

char * readLineCharbyChar(FILE * file) 
{
    int c;
    int index = 0;
    char * line = (char *) malloc(2);
    checkMalloc(line);

    while ((c = getc(file)) != 10)
    {
        if (c == EOF)
        {
            printf("%s\n", line);
            return line;
        }
        
        line = realloc(line, (strlen(line) + 1) * sizeof(line));
        line[index++] = (char) c;
    }
    line[index] = '\0';
    printf("\n%s\n", line);
    return line;
}

// TODO ADD NAME PARAMETER
void readFile() 
{
    FILE * file;
    file = fopen("test.txt", "r");
    checkFile(file, "test.txt");

    while (feof(file) == 0)
    {
        readLineCharbyChar(file);
    }
}

int main(int argc, char const *argv[])
{
    if (argc == 3)
    {
    }
    if (argc > 3)
    {
        fprintf(stderr, "usage: <input> <output>\n");
        exit(1);
    }

    // TODO MAKE CASES FOR ARGC 1, 2, 3

    switch (argc)
    {
    case 1:
        /* code */
        break;
    case 2:
        /* code */
        break;
    case 3:
        checkInputFileNames(argv[1], argv[2]);
        break;
    default:
        fprintf(stderr, "usage: <input> <output>\n");
        exit(1);
        break;
    }
    readFile();

    return 0;
}
