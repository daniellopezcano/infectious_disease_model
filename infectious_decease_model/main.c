// ================================================================================================================================ //
// ============================================================ INCLUDES ========================================================== //
// ================================================================================================================================ //
#include "functions.h"
#include "parameters.h"
#include "society_model.h"
#include "iterations.h"

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

#define N_states 5

// ================================================================================================================================ //
// ============================================================== MAIN ============================================================ //
// ================================================================================================================================ //
int main() 
{
// ================================================================================================================================ //
// ===================================================== VARIABLE DECLARATION  ==================================================== //
// ================================================================================================================================ //
// -------------------------------------------------------- PARALELIZATION ASPECTS ------------------------------------------------ //
	int32_t N_proc = 10;	// for all procs: omp_get_num_procs()
	fprintf( stderr, "\nNumber of procs = %i\n", N_proc);
	omp_set_num_threads(N_proc);
// --------------------------------------------------------- MODEL CHARACTERISTICS ------------------------------------------------ //	
	int64_t N = 32500;
	int64_t Ndays = 150;
	double percentage_collapse = 0.05;
	int64_t N_study = 1000;
	int64_t survey_delay = 3;
	int64_t threshold_days_in_alert = 10;
	double percentage_alert = 0.001;
// ------------------------------------------------------- CREATE RESULTS FOLDER --------------------------------------------------- //
	char new_folder_name[50] = "RESULTS_NOALERT_COLLAPSE_005";
	char *path_to_folder = create_folder(new_folder_name);
// ================================================================================================================================ //
// ===================================================== LOAD MODEL PARAMETERS ==================================================== //
// ================================================================================================================================ //
	Prob *P; P = (Prob *) calloc(1, sizeof(Prob));
	get_epidemic_parameters(P);
// -------------------------------------------------------------- RANDOM SEED ----------------------------------------------------- //
	int N_realizations = 10;
	int random_seed;
#pragma omp parallel for private(random_seed, path_to_folder) shared(N, Ndays, percentage_collapse, N_study, survey_delay, threshold_days_in_alert, percentage_alert, new_folder_name, P, N_realizations) schedule(static)
	for (random_seed = 1; random_seed <= N_realizations; random_seed++)
	{
		time_t t_start, t_end;
		t_start = time(NULL);
		//fprintf( stderr, "realization = %"PRIu64" / %"PRIu64"\n", random_seed, N_realizations);
		srand(random_seed);
	 // ================================================================================================================================ //
	 // =========================================================== SOCIAL GRAPH ======================================================= //
	 // ================================================================================================================================ //
	 // --------------------------------------------------------- VARIABLE DECLARATION ------------------------------------------------- //
		individual *ID; ID = (individual *) calloc( N, sizeof(individual) );
		sizes_locations *loc_sizes; loc_sizes = (sizes_locations *) calloc( 1, sizeof(sizes_locations) );
	 // --------------------------------------------------------- build_social_graph ---------------------------------------------------- //
		int64_t *hosp_sizes; hosp_sizes = (int64_t *) calloc( 1, sizeof(int64_t) );
		build_social_graph(N, loc_sizes, ID, hosp_sizes, hosp_sizes);
	 // ---------------------------------------------------------- BUILD LOCATIONS ------------------------------------------------------ //
		int64_t *h; h = (int64_t *) calloc( (loc_sizes -> ID.h + 1) * loc_sizes -> cap.h, sizeof(int64_t) );
		int64_t *w; w = (int64_t *) calloc( (loc_sizes -> ID.w + 1) * loc_sizes -> cap.w, sizeof(int64_t) );
		int64_t *hosp; hosp = (int64_t *) calloc( (loc_sizes -> ID.hosp + 1) * loc_sizes -> cap.hosp, sizeof(int64_t) );

	 // ================================================================================================================================ //
	 // ======================================================== INITIAL CONDITIONS ==================================================== //
	 // ================================================================================================================================ //
		int64_t *M; M = (int64_t *) calloc( Ndays * N, sizeof(int64_t) );
		initial_conditions(N, M);
 	 // ================================================================================================================================ //
     // =========================================================== ITERATIONS ========================================================= //
	 // ================================================================================================================================ //
		int64_t N_of[N_states];
		int64_t days_in_alert = 0;
		int64_t state_of_alert = 0;
		int64_t N_collapsed_hosps = 0;
		int64_t *ALERT; ALERT = (int64_t *) calloc( Ndays, sizeof(int64_t) );
		double *HOSP_COLLAPSE; HOSP_COLLAPSE = (double *) calloc( Ndays, sizeof(double) );
		ALERT[0] = state_of_alert;
		HOSP_COLLAPSE[0] = 0;
		for(int64_t day = 1; day < Ndays; day++)
		{
			// ------------------------------------------------- simulate_testing --------------------------------------------------- //
			//sampling(N, N_study, N_of, M, day, survey_delay);
			// --------------------------------------------------- declare_alert ---------------------------------------------------- //
			//declare_alert(&state_of_alert, N_study, N_of, &days_in_alert, threshold_days_in_alert, percentage_alert);
			declare_alert1(&state_of_alert, day);
			ALERT[day] = state_of_alert;
			// ------------------------------------------------ initilaize_locations ------------------------------------------------- //
			initialize_h(N, loc_sizes, ID, h);
			initialize_w(N, loc_sizes, ID, w);
			initialize_hosp(N, loc_sizes, ID, hosp);
			// ---------------------------------------------------- build_social_graph ---------------------------------------------- //
			int64_t *in_hosp; in_hosp = (int64_t *) calloc( N, sizeof(int64_t) );
			people_going_places(N, day, state_of_alert, M, loc_sizes, ID, P, h, w, hosp, in_hosp);
			// ----------------------------------------------------------- contagion ------------------------------------------------ //
			int64_t *h_infected; h_infected = (int64_t *) calloc( N, sizeof(int64_t) );
			int64_t *w_infected; w_infected = (int64_t *) calloc( N, sizeof(int64_t) );
			int64_t *hosp_infected; hosp_infected = (int64_t *) calloc( N, sizeof(int64_t) );
			int64_t *collapsed_hosps; collapsed_hosps = (int64_t *) calloc( loc_sizes -> ID.w + 1, sizeof(int64_t) );
			contagion(N, day, M, loc_sizes, ID, P, h, w, hosp, h_infected, w_infected, hosp_infected, hosp_sizes, collapsed_hosps, percentage_collapse, &N_collapsed_hosps);
			//fprintf( stderr, "    (double)N_collapsed_hosps = %f\n", (double)N_collapsed_hosps);
			//fprintf( stderr, "    (double)(loc_sizes -> ID.hosp) = %"PRIu64"\n", loc_sizes -> ID.hosp + 1);
			HOSP_COLLAPSE[day] =  (double)N_collapsed_hosps / (double)((loc_sizes -> ID.hosp) + 1);
			// ------------------------------------------------------ illness_progress ---------------------------------------------- //
			int64_t *new_states; new_states = (int64_t *) calloc( N, sizeof(int64_t) );
			illness_progress(N, day, M, ID, P, new_states, in_hosp, collapsed_hosps);
			// ------------------------------------------------------ new_step ------------------------------------------------------ //
			new_step(N, day, M, new_states, h_infected, w_infected, hosp_infected);
			fprintf( stderr, "    day = %"PRIu64". state_of_alert = %"PRIu64". HOSP_COLLAPSE = %f\n", day, state_of_alert, HOSP_COLLAPSE[day]);
		}

	 // ================================================================================================================================ //
	 // ============================================================= SAVE ============================================================= //
	 // ================================================================================================================================ //
		char *path_to_folder = create_folder(new_folder_name);
	 // ----------------------------------------------------- OPEN results.dat file ----------------------------------------------------- //
		char string_random_seed[20];
		sprintf(string_random_seed,"%d",random_seed);
		char filename[ sizeof(string_random_seed) / sizeof(string_random_seed[0]) + 4];
		strcpy( filename, string_random_seed );
		strcat( filename, ".dat" );
		char *path_to_file;
		path_to_file = strcat(path_to_folder, "/");
		path_to_file = strcat(path_to_folder, filename);
		fprintf(stderr, "\nSaving results in %s\n", path_to_file);
		FILE *f = fopen(path_to_file, "w");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			exit(1);
		}
	 // --------------------------------------------------- SAVE TO results.dat file --------------------------------------------------- //
		for(int64_t day = 0; day < Ndays; day++)
		{	
			for(int64_t i = 0; i < N; i++)
			{
				fprintf( f, "%"PRIu64" ", M[i2D(day, i, N)]);
				fflush(f);
			}
			fprintf( f, "%"PRIu64, ALERT[day]);
			fprintf( f, " %f", HOSP_COLLAPSE[day]);
			fprintf( f, "\n");
			fflush(f);
		}
		fclose(f);
	 // ================================================================================================================================ //
	 // ======================================================== Execution Time ======================================================== //
	 // ================================================================================================================================ //
		t_end = time(NULL);
		fprintf( stderr, "\n\nExecution time = %f [sec]\n\n", difftime(t_end, t_start) );
	}
return 0;
}
