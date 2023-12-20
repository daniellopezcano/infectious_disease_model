#include "initial_conditions.h"
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
// ====================================================== initial_conditions ====================================================== //
// ================================================================================================================================ //
void initial_conditions(int64_t N, int64_t *M)
{
	int64_t day = 0;
	//https://nadaesgratis.es/anxo-sanchez/ni-el-pico-ni-el-final-de-una-epidemia-se-pueden-predecir-con-precision
	//states : susceptible (S-0), infected mild - infected asymptomatic (E-1), infected severe (I-2), recovered (R-3), dead (D-4) --> SEIRD
	int64_t initial_state[5];
	initial_state[1] = 3; // Number initial E
	initial_state[2] = 0; // Number initial I
	initial_state[3] = 0; // Number initial R
	initial_state[4] = 0; // Number initial D
	initial_state[0] = N - initial_state[1]; // Number initial S
	for(int64_t i = 0; i < N; i++)
    {
		if(i < initial_state[1])
		{
			M[i2D(day, i, N)] = 1;
		}
		else
		{
			M[i2D(day, i, N)] = 0;
		}
    }
}
