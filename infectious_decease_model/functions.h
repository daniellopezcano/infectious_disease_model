#include <inttypes.h>

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
// ================================================================================================================================ //
// ============================================================ STRUCTURES ======================================================== //
// ================================================================================================================================ //

// ------------------------------------------------------------ sizes_locations --------------------------------------------------- //
struct max_size
{
    int64_t h;
    int64_t w;
	int64_t hosp;
};
typedef struct max_size max_size;

struct max_ID
{
    int64_t h;
    int64_t w;
	int64_t hosp;
};
typedef struct max_ID max_ID;

struct sizes_locations
{
    max_ID ID;
	max_size cap;
};
typedef struct sizes_locations sizes_locations;

// ------------------------------------------------------------ individual --------------------------------------------------- //
struct individual
{
    int64_t h;
    int64_t w;
	int64_t hosp;
};
typedef struct individual individual;

// --------------------------------------------------------------- Prob ------------------------------------------------------- //
#define N_states 5

struct P_go
{ 
    double h;
    double w;
	double hosp;
};
typedef struct P_go P_go; 


struct P_infection
{ 
    double h;
    double w;
	double hosp;
};
typedef struct P_infection P_infection; 


struct Prob
{
	P_go go[2][N_states];
	P_infection infect;
	double evolve[3][N_states][N_states];
	double evolve_cumulative[3][N_states][N_states];
};

typedef struct Prob Prob;

// ================================================================================================================================ //
// ======================================================== FUNCTION HEADERS ====================================================== //
// ================================================================================================================================ //

double my_random();

void shuffle(int64_t *list, int64_t N);

int64_t countOccurrences(int64_t *list, int64_t N, int64_t element);

int64_t i2D(int64_t i, int64_t j, int64_t N);

char * create_folder(const char name[]);

#endif /* FUNCTIONS_H_ */