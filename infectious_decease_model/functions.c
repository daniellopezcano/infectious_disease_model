#include "functions.h"

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// ================================================================================================================================ //
// ================================================================ random ======================================================== //
// ================================================================================================================================ //
double my_random()
{
    return (double)rand() / (double)RAND_MAX ;
}

// ================================================================================================================================ //
// ================================================================ shuffle ======================================================= //
// ================================================================================================================================ //
void shuffle(int64_t *list, int64_t N)
{
	for (int64_t i = 0; i < N; i++)// shuffle array
	{
		int64_t temp = list[i];
		int64_t randomIndex = rand() % N;

		list[i]           = list[randomIndex];
		list[randomIndex] = temp;
	}
}

// ================================================================================================================================ //
// =========================================================== countOccurrences =================================================== //
// ================================================================================================================================ //
int64_t countOccurrences(int64_t *list, int64_t N, int64_t element) 
{
    int64_t res = 0; 
    for (int64_t i=0; i<N; i++) 
        if (element == list[i]) 
          res++; 
    return res; 
} 

// ================================================================================================================================ //
// ================================================================== i2D ========================================================= //
// ================================================================================================================================ //
int64_t i2D(int64_t i, int64_t j, int64_t N)
{
  return (i)*(N) + (j);
}

// ================================================================================================================================ //
// ===================================================== create_folder ============================================================ //
// ================================================================================================================================ //
char * create_folder(const char name[])
{
    char *path_to_folder, *dummy_path;
    char cwd[PATH_MAX];

    getcwd(cwd, sizeof(cwd));
	
	dummy_path = strcat(cwd, "/");
    path_to_folder = strcat(dummy_path, name);

    mkdir(path_to_folder, 0777);
	
	return path_to_folder;
}