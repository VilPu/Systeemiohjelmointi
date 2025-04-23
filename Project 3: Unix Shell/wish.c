#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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

int ERROR_STATE = 0;

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

int isRedirect(char *tokens[1024], int index, int ntokens)
{
    if (index + 1 > ntokens)
    {
        return 1;
    }
    if (strcmp(tokens[index], ">") != 0)
    {
        return 1;
    }
    if (tokens[index + 1] != NULL)
    {
        return 0;
    }
    return 1;
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
    *row = *row + 1;
}

void saveTokenWithSpecials(char *tokens[1024], char *token, char *specialCharPtr, int *row, char specialChar[2])
{
    if (&token[0] - 1 == specialCharPtr && *row != 0) // token starts with special excl. first token
    {
        saveToken(tokens, specialChar, row);
        saveToken(tokens, token, row);
    }
    else if (&token[0] - 1 != specialCharPtr)
    {
        saveToken(tokens, token, row);
        saveToken(tokens, specialChar, row);
    }
    else
    {
        saveToken(tokens, token, row);
    }
}

void findNextSpecial(char **ptrSpecial, char specialChar)
{
    if (*ptrSpecial != NULL)
    {
        *ptrSpecial = strchr(*ptrSpecial + 1, specialChar);
    }
}

void checkConsecutiveSpecials(char *token)
{
    if (token == NULL)
    {
        return;
    }
    if (strlen(token) <= 1)
    {
        return;
    }
    for (int i = 0; token[i + 1] != '\0'; i++)
    {
        if (token[i] != token[i + 1]) // not the same, continue
        {
            continue;
        }
        if (token[i] == '&' || token[i] == '>') // check for &, >
        {
            ERROR_STATE = 1;
            break;
        }
    }
    return;
}

int parseToken(char *tokens[1024], char *token, int row)
{
    char *ptrAmpersand;
    char *ptrRedirect;
    char *newToken;

    char *tokenCopy = malloc(strlen(token) + 2); // copy to avoid changes to original
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
    // find specials
    ptrAmpersand = strchr(tokenCopy, '&');
    ptrRedirect = strchr(tokenCopy, '>');

    checkConsecutiveSpecials(tokenCopy);
    if (ERROR_STATE == 1)
    {
        printError();
        free(tokenCopy);
        return row;
    }
    if (&tokenCopy[0] == ptrAmpersand && row == 0)
    {
        findNextSpecial(&ptrAmpersand, '&');
    }
    if (&tokenCopy[0] == ptrRedirect && row == 0) // leads to error, no trailing command
    {
        ERROR_STATE = 1;
        printError();
        free(tokenCopy);
        return row;
    }
    if (strcmp(tokenCopy, "&") == 0 || strcmp(tokenCopy, ">") == 0) // if the token is a special char
    {
        saveToken(tokens, tokenCopy, &row);
        free(tokenCopy);
        return row;
    }
    if (ptrAmpersand + 1 == ptrRedirect || ptrRedirect + 1 == ptrAmpersand) // specials are next to each other -> redirect fails
    {
        ERROR_STATE = 1;
        printError();
        free(tokenCopy);
        return row;
    }

    newToken = strtok(tokenCopy, "&>");
    while (newToken != NULL)
    {
        if (ptrAmpersand != NULL && ptrRedirect == NULL) // only ampersand is found
        {
            saveTokenWithSpecials(tokens, newToken, ptrAmpersand, &row, "&");
            findNextSpecial(&ptrAmpersand, '&');
        }
        else if (ptrRedirect != NULL && ptrAmpersand == NULL) // only redirect is found
        {
            saveTokenWithSpecials(tokens, newToken, ptrRedirect, &row, ">");
            findNextSpecial(&ptrRedirect, '>');
        }
        else if (ptrAmpersand != NULL && ptrRedirect != NULL && ptrAmpersand < ptrRedirect) // both found but ampersand is first
        {
            saveTokenWithSpecials(tokens, newToken, ptrAmpersand, &row, "&");
            findNextSpecial(&ptrAmpersand, '&');
        }
        else if (ptrAmpersand != NULL && ptrRedirect != NULL && ptrRedirect < ptrAmpersand) // both found but redirect is first
        {
            saveTokenWithSpecials(tokens, newToken, ptrRedirect, &row, ">");
            findNextSpecial(&ptrRedirect, '>');
        }
        else
        {
            saveToken(tokens, newToken, &row);
        }
        newToken = strtok(NULL, "&>");
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
        if (row == 1024) // avoid index out of bounds error
        {
            break;
        }
        row = parseToken(tokens, token, row);
        if (ERROR_STATE == 1)
        {
            return row;
        }
        token = strtok_r(NULL, delim, &statePtr);
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
    for (int i = 0; i < index - start; i++)
    {
        free(args[i]);
    }
}

void allocateArgs(char *args[], char *tokens[], int index, int start)
{
    for (int i = 0; i < index - start; i++)
    {
        if ((args[i] = malloc(strlen(tokens[start]))) == NULL)
        {
            raiseError();
        }
        strcpy(args[i], tokens[i + start]);
    }
}

void biPath(char *args[], int argc)
{
    int varCount = 0;
    freePathVariables();

    for (int i = 1; i < argc; i++) // starts at 1 so 'path' is not included
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

pid_t runBinary(char *args[], int start, int index, char *outputFile)
{
    int fileRedirect = -1;

    if (outputFile != NULL)
    {
        if ((fileRedirect = open(outputFile, O_WRONLY | O_TRUNC | O_CREAT, S_IWUSR | S_IRUSR)) == -1)
        {
            ERROR_STATE = 1;
            printError();
            return 0;
        }
    }

    for (int i = 0; i < NPATH_VARIABLES; i++)
    {
        char *bin = malloc(strlen(PATH_VARIABLES[i]) + strlen(args[0]) + 2);
        if (bin == NULL)
        {
            break;
        }
        strcat(bin, PATH_VARIABLES[i]);
        strcat(bin, "/");
        strcat(bin, args[0]);
        if (access(bin, X_OK) != -1)
        {
            pid_t id = fork();
            if (id == 0)
            {
                if (outputFile != NULL) // if redirect -> change stdout and stderr of the process to file
                {
                    int out = dup2(fileRedirect, fileno(stdout));
                    int err = dup2(fileRedirect, fileno(stderr));
                    close(fileRedirect);
                    if (out == -1 || err == -1) // redirect failed, exit(1)
                    {
                        exit(1);
                    }
                }
                execv(bin, args);
                raiseError(); // if got here execv encountered error
            }
            close(fileRedirect);
            free(bin); // free bin if X_OK
            return id; // return child process id
        }
        free(bin); // free bin after every non X_OK iteration
    }
    printError();
    return 0; // no processes started -> return 0
}

int runCommand(char *tokens[1024], int *start, int ntokens, int builtIn)
{
    int index = *start;
    char *token = tokens[index];
    pid_t processId = 0;

    if (strcmp(token, "&") == 0 || strcmp(token, ">") == 0)
    {
        *start = index + 1;
        return 0;
    }

    while (strcmp(token, "&") != 0 && strcmp(token, ">") != 0 && index < ntokens)
    {
        index++;
        token = tokens[index];
    }
    int argc = index - *start;
    char *args[argc];
    allocateArgs(args, tokens, index, *start);
    args[argc] = NULL; // NULL in the end for execv

    switch (builtIn)
    {
    case EXIT:
        if ((index - *start) != 1)
        {
            printError();
        }
        else
        {
            clearTokens(tokens, ntokens);
            exit(0);
        }
        break;
    case CD:
        biCd(index - *start, args[1]);
        break;
    case PATH:
        biPath(args, argc);
        break;
    default:
        if (isRedirect(tokens, index, ntokens) == 0)
        {
            processId = runBinary(args, *start, index, tokens[index + 1]);
            freeArgs(args, index, *start);
            *start = index + 2;
            return processId;
        }
        else
        {
            processId = runBinary(args, *start, index, NULL);
            freeArgs(args, index, *start);
            *start = index;
            return processId;
        }
        break;
    }
    freeArgs(args, index, *start);
    *start = index;
    return 0;
}

void executeCommands(char *tokens[1024], int ntokens)
{
    pid_t processes[1024];
    pid_t processId = 0;
    int nprocess = 0;

    for (int i = 0; i < ntokens; i++)
    {
        if (ERROR_STATE == 1)
        {
            break;
        }
        char *token = tokens[i];

        if (strcmp(token, BUILT_INS[EXIT]) == 0)
        {
            runCommand(tokens, &i, ntokens, EXIT);
        }
        else if (strcmp(token, BUILT_INS[CD]) == 0)
        {
            runCommand(tokens, &i, ntokens, CD);
        }
        else if (strcmp(token, BUILT_INS[PATH]) == 0)
        {
            runCommand(tokens, &i, ntokens, PATH);
        }
        else
        {
            processId = runCommand(tokens, &i, ntokens, -1);
            if (processId > 0 && ERROR_STATE == 0)
            {
                processes[nprocess++] = processId;
            }
        }
    }
    for (int i = 0; i < nprocess; i++) // wait for child processes
    {
        waitpid(processes[i], NULL, 0);
    }
}

void checkRedirection(char *tokens[1024], int ntokens)
{
    for (int i = 0; i < ntokens; i++)
    {
        if (strcmp(tokens[i], ">") != 0)
        {
            continue;
        }
        if (i == 0 || i == ntokens || i + 1 > ntokens || strcmp(tokens[i - 1], "&") == 0)
        {
            printError();
            ERROR_STATE = 1;
            break;
        }
        if (i + 1 == ntokens || strcmp(tokens[i + 1], "&") == 0)
        {
            printError();
            ERROR_STATE = 1;
            break;
        }
        if (i + 2 == ntokens || ((i + 2 <= ntokens) && strcmp(tokens[i + 2], "&") == 0))
        {
            continue;
        }
        printError(); // defaults to error
        ERROR_STATE = 1;
        break;
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
        checkRedirection(tokens, ntokens);
        free(line);
        line = NULL;
        if (ERROR_STATE == 0)
        {
            executeCommands(tokens, ntokens);
        }
        clearTokens(tokens, ntokens);
        ntokens = 0;
        ERROR_STATE = 0;
    }
}

int main(int argc, char const *argv[])
{
    FILE *file;

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
        file = fopen(argv[1], "r");
        if (file == NULL)
        {
            raiseError();
        }
        runShell(file);
        fclose(file);
        break;

    default:
        raiseError();
        break;
    }

    exit(0);
}
