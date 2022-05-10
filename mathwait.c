#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#pragma GCC diagnostic ignored "-Wmultichar"

#define FOO 4096



// print array with pre-text to file and only for child. 
void printArray (int *arr, int size, FILE *outputFilePointer, char *executionContext)
{
    fprintf(outputFilePointer, "%s: %d: ", executionContext, getpid());

    int i;
    for (i = 0; i < size; i++)
    {
        fprintf(outputFilePointer, "%d ", arr[i]);
    }
    fprintf(outputFilePointer, "\n");
}

// Check if two values equal 19 to decide exit success or exit failure
bool checkForNineteen(int *A, int size, FILE *outputFilePointer, char *executionContext)
{
    int boole = 0;
    int i = 0;
    int j = 0;
    for (i = 0; i < (size - 1); i++)
    {
        for (j = (i + 1); j < size; j++)
        {
            if (A[i] + A[j] == 19)
            {
                boole = 1;
                fprintf(outputFilePointer, "%s: %d: Pair: %d %d \n", executionContext, getpid(), A[i], A[j]);
            }
        }
    }
    if (boole == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
static int *glob_var;

int main(int argc, char **argv)
{
    char *filename = "logfile.txt";
    char *executionContext = "Parent";


    // Get Array from user input into dynamic array
    int i, NoOfElements;
    int *arr; // array pointer
    char option;

    // case for tutorial -h
    if (option == '-h')
    {
        printf("Please enter mathwait.c followed by a output filename and numbers.");
        printf("Termination.");

        return EXIT_SUCCESS;
    }

    if (argv[1] != NULL)
    {
        filename = argv[1];
    }
    if (argc > 2)
    {
        // allocate memory dynamically with of
        // total size = NoOfElements* size of 1 integer i.e. sizeof(int)
        // malloc returns void pointer,means points to nothing
        // typecast it to int* as we want to store int values

        NoOfElements=argc-2;
        
        arr = (int *)malloc(NoOfElements * sizeof(int));

        // Check memory is allocated successfully
        // if fails return from the program.
        if (arr == NULL)
        {
            printf("No memory allocated.\n");
            return 1; // memory allocation fails - return from here
        }

        // fill the array with elements by reading from console screen
        for (i = 0; i < NoOfElements; ++i)
        {
            arr[i] = atoi(argv[i+2]);
        }
    }

    // Open and write to file
    FILE *outputFilePointer;
    outputFilePointer = fopen(filename, "a");
    // child process
    int successStatus = 0;
    pid_t childPid = fork(); // This is where the child process splits from the parent
    if (childPid == 0)
    {
        executionContext = "Child";
        printArray(arr, NoOfElements, outputFilePointer, executionContext);
        if (checkForNineteen(arr, NoOfElements, outputFilePointer, executionContext) == 1)
        {
            successStatus = 1;
	    
        }
        else
        {
            printf("bad match\n");
            successStatus = 3;
        }

    }
    else
    {
        int stat = 0;
        // Exit status for parent have it return from child
        wait(&stat);
        if (WIFEXITED(stat))
        {
            if (WEXITSTATUS(stat) == 0)
            {
                fprintf(outputFilePointer, "%s: %d: EXIT_SUCCESS\n", executionContext, getpid());
		 
            }
            else
            {
                fprintf(outputFilePointer, "%s: %d: EXIT_FAILURE\n", executionContext, getpid());
            }
            printf("Exit status: %d\n ", WEXITSTATUS(stat));
        }
        else if (WIFSIGNALED(stat))
        {
            psignal(WTERMSIG(stat), "Exit signal");
            printf("Child has ended, now returning to Parent. \n");
        }
    }

    if (outputFilePointer == NULL)
    {
        printf("Bad file! %s cannot be opened", filename);

        return EXIT_FAILURE;
    }
    if (successStatus == 3)
    {
        printf("No matches to 19 founds\n");

        return EXIT_FAILURE;
    }
    fclose(outputFilePointer);

    return EXIT_SUCCESS;


// shared memory location

	glob_var = mmap(NULL, sizeof *glob_var, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -2, -2);
	*glob_var = 1;

	if (fork() == 0)
	{
	 *glob_var = 5;
	 exit(EXIT_SUCCESS);
	
	}
	else
	{
	  wait(NULL);
	 printf("%d\n", *glob_var);
	 munmap(glob_var, sizeof *glob_var);
	
	}



}

