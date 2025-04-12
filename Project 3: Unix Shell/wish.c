#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum BuiltIn
{
    EXIT = 0,
    CD = 1,
    PATH = 2
};

const char *BUILT_INS[] = {"exit", "cd", "path"};

const char ERROR_MESSAGE[30] = "An error has occurred\n";

char *PATH_VARIABLES[1024];
int NPATH_VARIABLES = 0;

void raiseError()
{
    fwrite(&ERROR_MESSAGE, strlen(ERROR_MESSAGE), 1, stderr);
    exit(1);
}

void printError()
{
    fwrite(&ERROR_MESSAGE, strlen(ERROR_MESSAGE), 1, stderr);
}

void checkFile(FILE *file)
{
    if (file == NULL)
    {
        fwrite(&ERROR_MESSAGE, strlen(ERROR_MESSAGE), 1, stderr);
        exit(1);
    }
    return;
}

void biExit()
{
    exit(0);
}

void biCd(int argc, char *path)
{
    if (argc != 2)
    {
        printError();
        return;
    }
    if (chdir(path) == -1)
    {
        printError();
    }
}

void checkPath()
{
}

void clearTokens(char *tokens[1024], int range)
{
    int row = 0;
    while (row < range)
    {
        free(tokens[row]);
        row++;
    }
}

void saveToken(char *tokens[1024], char *token, int *row)
{
    tokens[*row] = malloc(strlen(token));
    if (tokens[*row] == NULL)
    {
        raiseError();
    }
    strcpy(tokens[*row], token);
    printf("saveToken: %d %s\n", *row, tokens[*row]);
    *row = *row + 1;
}

int parseToken(char *tokens[1024], char *token, int row)
{
    char *ptrAmpersand;
    char *ptrRedirect;
    char *newToken;
    char *tokenCopy = malloc(strlen(token) + 2);
    if (tokenCopy == NULL)
    {
        raiseError();
    }
    strcpy(tokenCopy, token);
    
    if (row == 1024)
    {
        free(tokenCopy);
        return row;
    }
    
    ptrAmpersand = strchr(tokenCopy, '&');
    ptrRedirect = strchr(tokenCopy, '>');

    if (ptrAmpersand == NULL && ptrRedirect == NULL)
    {
        saveToken(tokens, tokenCopy, &row);
        free(tokenCopy);
        return row;
    }
    if (strcmp(tokenCopy, "&") == 0 || strcmp(tokenCopy, ">") == 0)
    {
        saveToken(tokens, tokenCopy, &row);
        free(tokenCopy);
        return row;
    }
    
    newToken = strtok(tokenCopy, "&>");
    while (newToken != NULL)
    {
        printf("newToken: %s\n", newToken);
        if (ptrAmpersand != NULL && ptrRedirect == NULL) // only ampersand is found
        {
            if (&newToken[0] - 1 == ptrAmpersand) // compare memory address
            {
                saveToken(tokens, "&", &row);
                saveToken(tokens, newToken, &row);
            } else
            {
                saveToken(tokens, newToken, &row);                
                saveToken(tokens, "&", &row);
                
                printf("token: %s\n", token);
            }
        } else
        {
            //printf("else\n");
            saveToken(tokens, newToken, &row);
        }

        newToken = strtok(NULL, "&>");
        if (newToken != NULL)
        {
            ptrAmpersand = strchr(newToken, '&');
            ptrRedirect = strchr(newToken, '>');
            printf("newToken: %s, ptrAmp: %p, ptrRed: %p\n", newToken, ptrAmpersand, ptrRedirect);
        }
    }

    free(tokenCopy);
    return row;
}

int tokenize(char *line, char *tokens[1024])
{
    const char delim[5] = " \t\n"; // tabs, whitespaces and newlines
    char *token;
    char *statePtr;
    int row = 0;

    token = strtok_r(line, delim, &statePtr);

    while (token != NULL)
    {
        printf("Parsing token: %s\n", token);
        if (row == 1024) // avoid index out of bounds error
        {
            break;
        }
        row = parseToken(tokens, token, row);
        printf("\nToken after: %s\n\n", token);

        token = strtok_r(NULL, delim, &statePtr);
        printf("\nSTRTOK: %s\n\n", token);

    }
    return row;
}

void freePathVariables()
{
    for (int i = 0; i < NPATH_VARIABLES; i++)
    {
        free(PATH_VARIABLES[i]);
    }
}

void freeArgs(char *args[], int index, int start)
{
    // printf("argc: %d | index: %d | start: %d\n", index-start, index, start);
    for (int i = 0; i < index - start; i++) // free args TODO change i to 0
    {
        // printf("Freeing arg[%d]: %s\n", i, args[i]);
        free(args[i]);
    }
    // printf("should be null: %s\n", args[index-start]);
}

void allocateArgs(char *args[], char *tokens[], int index, int start)
{
    for (int i = 0; i < index - start; i++)
    {
        if ((args[i] = malloc(strlen(tokens[start + i]))) == NULL)
        {
            raiseError();
        }
        strcpy(args[i], tokens[i]);
    }
}

void biPath(char *args[], int argc)
{
    int varCount = 0;
    freePathVariables();

    for (int i = 1; i < argc; i++)
    {
        int varIdx = i - 1;
        if ((PATH_VARIABLES[varIdx] = malloc(strlen(args[i]))) == NULL)
        {
            NPATH_VARIABLES = varCount;
            freePathVariables();
            varCount = 0;
            printError();
            break;
        }
        strcpy(PATH_VARIABLES[varIdx], args[i]);
        varCount++;
    }
    NPATH_VARIABLES = varCount;
}

void runBinary(char *args[], int start, int index)
{
    for (int i = 0; i < NPATH_VARIABLES; i++)
    {
        char *bin = malloc(strlen(PATH_VARIABLES[i]) + strlen(args[0]) + 1);
        if (bin == NULL)
        {
            break;
        }
        strcat(bin, PATH_VARIABLES[i]);
        strcat(bin, "/");
        strcat(bin, args[0]);
        // printf("Running bin: %s and args[0]: %s\n", bin, args[0]);
        if (access(bin, X_OK) != -1)
        {
            pid_t id = fork();
            if (id == 0)
            {
                execv(bin, args);
                raiseError(); // if got here execv encountered error
            }
            else
            {
                wait(0);
            }
            free(bin); // free bin if accessible
            return;
        }
        free(bin); // free bin after every iteration
    }
    printError();
    return;
}

int runCommand(char *tokens[1024], int start, int end, int builtIn)
{
    int index = start;
    char *token = tokens[index];
    while (strcmp(token, "&") != 0 && strcmp(token, ">") != 0 && index < end)
    {
        index++;
        token = tokens[index];
    }
    int argc = index - start;
    char *args[argc];
    allocateArgs(args, tokens, index, start);
    args[argc] = NULL; // NULL in the end for execv

    switch (builtIn)
    {
    case CD:
        biCd(index - start, args[1]);
        break;
    case PATH:
        biPath(args, argc);
        break;
    default:
        runBinary(args, start, index);
        break;
    }
    freeArgs(args, index, start);
    return index;
}

void executeCommands(char *tokens[1024], int ntokens)
{
    for (size_t i = 0; i < ntokens; i++)
    {
        char *token = tokens[i];

        if (strcmp(token, BUILT_INS[EXIT]) == 0)
        {
            clearTokens(tokens, ntokens);
            biExit();
        }
        else if (strcmp(token, BUILT_INS[CD]) == 0)
        {
            i = runCommand(tokens, i, ntokens, CD);
        }
        else if (strcmp(token, BUILT_INS[PATH]) == 0)
        {
            i = runCommand(tokens, i, ntokens, PATH);
        }
        else
        {
            i = runCommand(tokens, i, ntokens, -1);
        }
    }
}

void runShell(FILE *input)
{
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;

    char *tokens[1024];
    int ntokens = 0;

    while (1 == 1)
    {
        if (input == stdin)
        {
            printf("wish> ");
        }

        nread = getline(&line, &size, input);
        if (nread == -1)
        {
            free(line);
            clearTokens(tokens, ntokens);
            break;
        }
        ntokens = tokenize(line, tokens);
        free(line);
        line = NULL;
        executeCommands(tokens, ntokens);
        clearTokens(tokens, ntokens);
        ntokens = 0;
    }
}

int main(int argc, char const *argv[])
{

    if ((PATH_VARIABLES[0] = malloc(sizeof(char) * 6)) == NULL)
    {
        raiseError();
    }
    strcpy(PATH_VARIABLES[0], "/bin"); // start with /bin path
    NPATH_VARIABLES = 1;

    switch (argc)
    {
    case 1:
        runShell(stdin);
        break;
    case 2:

        break;

    default:
        raiseError();
        break;
    }

    exit(0);
}
