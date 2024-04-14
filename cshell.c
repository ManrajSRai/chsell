#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#define DEFAULT "\x1b[0m"
#define RED "\x1b[31m"
#define BLUE "\x1b[34m"
#define GREEN "\x1b[32m"
#define SIZE_OF_TEMP 4096
#define MAX_VARIABLE_SIZE 32
#define ARRAY_SIZE 16
#define LOG_SIZE 32
#define MAX_NUM_CHAR 100

//CUSTOM system call number
#define UPPERCASE 450


typedef struct
{
    char **tokens;//array of strings
    int numberOftokens;
} Token; //used to split apart inputs

typedef struct
{
    char name[MAX_NUM_CHAR];
    char value[MAX_NUM_CHAR];
} Variable; //struct for user created variables

typedef struct
{
    time_t timestamp;
    char entry[SIZE_OF_TEMP];
    int returnValue;
} Log; //used for storing entries

Variable envVar[ARRAY_SIZE]; // array for environmental variables
int variableTotal = 0;

Log logEntries[LOG_SIZE]; // array for log entries
int logCounter = 0;

Token split_tokens(char *input)
{
    Token completed_tokens;
    completed_tokens.tokens = malloc(SIZE_OF_TEMP * sizeof(char*)); //allocate memory for the array of strings

    char *token = strtok(input, " "); // select a line
    int i = 0;
    while (token != NULL && i < SIZE_OF_TEMP)
    {
        completed_tokens.tokens[i] = strdup(token);
        i++;
        token = strtok(NULL, " ");
    }

    completed_tokens.numberOftokens = i;

    //if no entries free allocated memory
    if (completed_tokens.numberOftokens == 0)
    {
        free(completed_tokens.tokens);
        completed_tokens.tokens=NULL;
    }

    return completed_tokens;//returns token struct 
}

void exitProgram(Token SplitTokens, char textColour[])//prints out bye and ends while loop in main
{
    printf("%s%s%s", textColour, "Bye!\n", DEFAULT);
}

int searchFunction(const char *entry, const Variable *env,char textColour[])//search for matching variable and return its location
{
    int matchLocation = -1;
    char nameSearch[MAX_NUM_CHAR];
    strcpy(nameSearch,entry);
    nameSearch[strlen(nameSearch)-1]='\0';//remove any trailing whitespace

    for (int i = 0; i < variableTotal; i++)
    {
        if (strcmp(env[i].name, nameSearch) == 0)
        {
            matchLocation = i;
            return matchLocation;
        }
    }
    return matchLocation;
}

void createVariable(const char *inputedInfo,char textColour[])
{
    char name[MAX_NUM_CHAR];
    char value[MAX_NUM_CHAR];
    char noWant[10]={'-','+',',','.','_','{','/','}','&','%'};
    // sees if there is the matched input with a = limit in middle to split input
    int numOfArguements = sscanf(inputedInfo, "%[^=]=%s", name, value);
    int location = -1;

    for(int i=0;name[i]!='\0';i++)
    {
        for(int j=0;j<10;j++)
        {
            if(name[i]==noWant[j])
            {
                printf("%s%s%s", textColour, "Error: inappropriate character found\n", DEFAULT);
                return;
            }
        }
    }

    if (numOfArguements != 2)
    {
        printf("%s%s%s", textColour, "Error: Not enough info for assignment\n", DEFAULT);
        return;
    }
    for (int i = 0; i < variableTotal; i++)
    {
        if (strcmp(envVar[i].name, name) == 0)
        {
            location = i;//return matching name-location
            break;
        }
    }

    if (location != -1)
    {
        //update existing info
        strncpy(envVar[location].value, value, MAX_NUM_CHAR - 1);
    }
    else
    {
        //add a new varaible into the environmental array
        strncpy(envVar[variableTotal].name, name, MAX_NUM_CHAR - 1);
        strncpy(envVar[variableTotal].value, value, MAX_NUM_CHAR - 1);
        variableTotal++;
    }
}

void printFunction(Token output, char textColour[])
{
    if (output.numberOftokens < 2 || strcmp(output.tokens[1], "\n") == 0 || strcmp(output.tokens[1], "\t") == 0)
    {
        printf("%s%s%s", textColour, "Error:Not enough values\n", DEFAULT);
    }
    else
    {
        for (int i = 1; i < output.numberOftokens; i++)
        {
            if(output.tokens[1][0]=='$'&& variableTotal)
            {
                if (searchFunction(output.tokens[1], envVar,textColour) != -1)//if we are looking for an env Variable
                {
                    printf("%s%s%s\n", textColour, envVar[searchFunction(output.tokens[1], envVar,textColour)].value, DEFAULT);
                    return;
                }
                else
                {
                    printf("%s%s%s", textColour, "Error: Variable does not exist\n", DEFAULT);
                    return;
                }
            }
            if (i == output.numberOftokens - 1)//no space added for last entry
            {
                printf("%s%s%s", textColour, output.tokens[i], DEFAULT);
            }
            else//space out each entry
    
            {
                printf("%s%s %s", textColour, output.tokens[i], DEFAULT);
            }
        }
    }
}

// This function is used to change the value of: textColour[]
void themeFunction(Token SplitTokens, char textColour[])
{
    //Input validation ensuring the correct number of arguments
    if (1 == SplitTokens.numberOftokens)
    {
        printf("%s%s%s", textColour, "Invalid Input: Not enough arguments\n", DEFAULT);
    }
    else if (2 < SplitTokens.numberOftokens)
    {
        printf("%s%s%s", textColour, "Invalid Input: Too many arguments\n", DEFAULT);
    }

    //The respective ANSI colour values are copied into the textColour variable
    else if (strcmp(SplitTokens.tokens[1], "red\n") == 0 || strcmp(SplitTokens.tokens[1], "red") == 0)
    {
        strcpy(textColour, RED);
    }

    else if (strcmp(SplitTokens.tokens[1], "blue\n") == 0||strcmp(SplitTokens.tokens[1], "blue") == 0)
    {
        strcpy(textColour, BLUE);
    }

    else if (strcmp(SplitTokens.tokens[1], "green\n") == 0||strcmp(SplitTokens.tokens[1], "green") == 0)
    {
        strcpy(textColour, GREEN);
    }

    else if (strcmp(SplitTokens.tokens[1], "default\n") == 0||strcmp(SplitTokens.tokens[1], "default") == 0)
    {
        strcpy(textColour, DEFAULT);
    }

    else
    {
        printf("%s%s%s", textColour, "unsupported theme\n", DEFAULT);
    }
}

//Records all entries that are 
void addLogEntry(const char *entry, char textColour[], char returnValue)
{
    // Add the log entry
    if (logCounter < LOG_SIZE)
    {
        Log logEntry;
        logEntry.timestamp = time(NULL);
        strncpy(logEntry.entry, entry, SIZE_OF_TEMP - 1);
        logEntry.entry[SIZE_OF_TEMP - 1] = '\0';
        logEntry.returnValue = returnValue;
        logEntries[logCounter] = logEntry;
        logCounter++;
    }
    else
    {
        // Handle the case when the log array is full
        // by shifting existing entries or discarding the earliest entry
        for (int i = 0; i < LOG_SIZE - 1; i++)
        {
            logEntries[i] = logEntries[i + 1];
        }
        Log logEntry;
        logEntry.timestamp = time(NULL);
        strncpy(logEntry.entry, entry, SIZE_OF_TEMP - 1);
        logEntry.entry[SIZE_OF_TEMP - 1] = '\0';
        logEntry.returnValue = returnValue;
        logEntries[LOG_SIZE - 1] = logEntry;
    }
}



void displayLogEntries(char textColour[])
{
    for (int i = 0; i < logCounter; i++)
    {
        struct tm *timeInfo;
        timeInfo = localtime(&logEntries[i].timestamp);
        char timestamp[30];
        strftime(timestamp, sizeof(timestamp), "%a %b %d %H:%M:%S %Y", timeInfo);

        if (logEntries[i].returnValue > 0) {
            printf("%s[%s] %s %d%s\n", textColour, timestamp, logEntries[i].entry, -1, DEFAULT);
        } else {
            printf("%s[%s] %s %d%s\n", textColour, timestamp, logEntries[i].entry, logEntries[i].returnValue, DEFAULT);
        }
    }
}

void uppercaseBonus(Token tokens, char textColour[])
{
    char input[128];

    // Copy the second token to the input
    strcpy(input, tokens.tokens[1]);

    // Call the UPPERCASE system call
    long outcome = syscall(UPPERCASE, input);

    // Check the outcome of the system call
    if (outcome == 0)
    {
        // Print the modified input string 
        printf("%s%s%s", textColour, input, DEFAULT);
    }
    else
    {
        // Print "failed" 
        printf("%s%s%s", textColour, "failed\n", DEFAULT);
    }
    return;
}



void nonBuiltFunctions(const char* command, char textColour[])
{
    char syscall[ARRAY_SIZE];
    strcpy(syscall, command);
    char filepath[MAX_VARIABLE_SIZE];

    sprintf(filepath, "/bin/%s", syscall); //creates the filepath to look for the command

    int fd[2];
    if (pipe(fd) == -1)
    {
        printf("%s%s%s", textColour, "Pipe creation failed\n", DEFAULT);
        addLogEntry(command, textColour, -1);
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        printf("%s%s%s", textColour, "Missing keyword or command, or permission problem.\n", DEFAULT);
        addLogEntry(command, textColour, -1);
        exit(1);
    }
    else if (pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execlp(filepath, filepath, (char*)NULL);
        printf("%s%s%s", textColour, "Missing keyword or command, or permission problem.\n", DEFAULT);
        addLogEntry(command, textColour, -1);
        exit(1);
    }
    else
    {
        close(fd[1]);
        char output[SIZE_OF_TEMP];
        ssize_t i;
        int returnValue = 0;

        while ((i = read(fd[0], output, sizeof(output))) > 0)
        {
            output[i] = '\0'; // Null-terminate the string
            printf("%s%s%s", textColour, output, DEFAULT);
        }

        int status;
        wait(&status); // Wait for the child process to finish

        if (WIFEXITED(status)) {
            returnValue = WEXITSTATUS(status);
        }

        addLogEntry(command, textColour, returnValue);
    }
}





int main(int argc, char *argv[])
{

    //Initialization of all variables
    char input[SIZE_OF_TEMP];
    Token SplitTokens;
    char textColour[7] = DEFAULT;
    FILE *script = NULL;
    int done = 0;
    int scriptSize = 0;
    int currentLine = 0;
    char firstLetter;

    //If there are 2 arguments then it is assumed the user is attempted to use the scripts
    if (argc == 2)
    {

        //Opens myscript.txt
        if (strcmp(argv[1], "myscript.txt") == 0)
        {

            script = fopen(argv[1], "r");

            //iterates through to find how many lines long the script is
            while (fgets(input, sizeof(input), script))
            {
                scriptSize++;
            }
            //Goes back to the first line of the file
            fseek(script, 0, SEEK_SET);
        }


        //if the user tries to read a different file
        else
        {
            printf("%s%s%s%s\n", textColour, "Unable to read script file: ", argv[1], DEFAULT);
            return 0;
        }
    }
    if (argc > 2)
    {
        printf("%s%s%s", textColour, "Too many arguments\n", DEFAULT);
    }

    //This is the main loop that procresses all the inputs
    while (done == 0)
    {

        //Interactive mode (Direct user inputs)
        if (argc == 1)
        {
            printf("%s%s%s", textColour, "cshell$ ", DEFAULT);
            fgets(input, SIZE_OF_TEMP, stdin);
        }

        //Script mode (Inputs done by )
        if (argc == 2)
        {

            //automatically exits if program reaches bottom of the script
            if (currentLine == scriptSize)
            {
                fclose(script);
                exitProgram(SplitTokens, textColour);
                done = 1;
                break;
            }
            else
            {
                fgets(input, sizeof(input), script);
                if(strcmp(input, "exit") == 0)//if there is a exit in the script file, ends the file.
                {
                    fclose(script);
                    exitProgram(split_tokens(input), textColour);
                    done = 1;;
                    break;
                }
                currentLine++;
            }
        }

        //Tokens are created
        SplitTokens = split_tokens(input);
        firstLetter = SplitTokens.tokens[0][0];

        //print command
        if (strcmp(SplitTokens.tokens[0], "print") == 0)
        {
            printFunction(SplitTokens, textColour);
            //log is updated
            addLogEntry(input, textColour, 0);
        }
        // if user tries to print without an input
        else if (strcmp(SplitTokens.tokens[0], "print\n") == 0)
        {
            printf("%s%s%s", textColour, "Error:Not enough values\n", DEFAULT);
            addLogEntry(input, textColour, -1);

        }

        //creates variable
        else if (firstLetter == '$')
        {
            input[strcspn(input,"\n")] = '\0';//removes the new line before input
            addLogEntry(input, textColour, 0);
            createVariable(SplitTokens.tokens[0],textColour);

        }

        //intiates exit
        else if (strcmp(SplitTokens.tokens[0], "exit\n") == 0)
        {
            exitProgram(SplitTokens, textColour);
            addLogEntry(input, textColour, 0);

            //if a script was opened, this closes it
            if (argc == 2)
            {
                fclose(script);
            }
            done = 1;
        }

        //theme command is entered
        else if (strcmp(SplitTokens.tokens[0], "theme") == 0)
        {
            themeFunction(SplitTokens, textColour);
            addLogEntry(input, textColour, 0);

        }

        //log function
        else if (strcmp(SplitTokens.tokens[0], "log\n") == 0 || (strcmp(SplitTokens.tokens[0], "log") == 0 && SplitTokens.numberOftokens<2))
        {
            input[strcspn(input, "\n")] = '\0'; //removes newline
            addLogEntry(input, textColour, 0);
            displayLogEntries(textColour);

        }

        /*----------------------------------------------------BONUS CODE--------------------------------------------------------------------*/
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       
        else if (strcmp(SplitTokens.tokens[0], "uppercase") == 0)
        {
            uppercaseBonus(SplitTokens, textColour);
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /*----------------------------------------------------BONUS CODE--------------------------------------------------------------------*/


        else
        {
            input[strcspn(input,"\n")] = '\0';//removes the new line before input
            nonBuiltFunctions(input,textColour);
        }

        // free each entry
        for (int j = 0; j < SplitTokens.numberOftokens; j++)
        {
            free(SplitTokens.tokens[j]); 

        }
        free(SplitTokens.tokens);//free allocated memory for the array
    }
    
    return 0;
}

