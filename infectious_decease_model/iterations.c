#include "iterations.h"
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

#define N_states 5

// ================================================================================================================================= //
// =========================================================== sampling ============================================================ //
// ================================================================================================================================= //
void sampling(int64_t N, int64_t N_study, int64_t *N_of, int64_t *M, int64_t day, int64_t survey_delay)
{	
	int64_t *rand_selection; rand_selection = (int64_t *) calloc( N, sizeof(int64_t) );
	for(int64_t i = 0; i < N; i++)
	{	
		rand_selection[i] = i;
	}
	shuffle(rand_selection, N);
	
	int64_t day_survey;
	if ( day <= survey_delay )
	{	
		day_survey = 1;
	}
	else
	{
		day_survey = day - survey_delay;
	}
	
	for(int64_t i = 0; i < N_states; i++)
	{	
		N_of[i] = 0;
	}
	for(int64_t i = 0; i < N_study; i++)
	{	
		N_of[M[i2D(day_survey - 1, rand_selection[i], N)]] += 1;
	}
}
// ================================================================================================================================= //
// =========================================================== declare_alert ======================================================= //
// ================================================================================================================================= //
void declare_alert(int64_t *state_of_alert, int64_t N_study, int64_t *N_of, int64_t *days_in_alert, int64_t threshold_days_in_alert, double percentage_alert)
{	
	if ((double)(N_of[1] + N_of[2]) / (double)N_study > percentage_alert)
	{	
		*days_in_alert += 1;
		*state_of_alert = 1;
	}
	else
	{	
		if ( *state_of_alert == 1 )
		{
			if ( *days_in_alert < threshold_days_in_alert)
			{	
				*days_in_alert += 1;
				*state_of_alert = 1;
			}
			else
			{
				*days_in_alert = 0;
				*state_of_alert = 0;
			}
		}
		else
		{
			*days_in_alert = 0;
			*state_of_alert = 0;
		}
	}
}


// ================================================================================================================================= //
// =========================================================== declare_alert1 ======================================================= //
// ================================================================================================================================= //
void declare_alert1(int64_t *state_of_alert, int64_t day)
{	
	int64_t first_day_alert = -1;
	int64_t last_day_alert = -1;
	
	if (day >= first_day_alert && day <= last_day_alert)
	{
		*state_of_alert = 1;
	}
	else
	{
		*state_of_alert = 0;
	}
}

// ================================================================================================================================= //
// ====================================================== people_going_places ====================================================== //
// ================================================================================================================================= //

void people_going_places(int64_t N, int64_t day, int64_t state_of_alert, int64_t *M, sizes_locations *loc_sizes, individual *ID, Prob *P, int64_t *h, int64_t *w, int64_t *hosp, int64_t *in_hosp)
{
	for(int64_t i = 0; i < N; i++)
    {
	 // -------------------------------------------------------------- households ------------------------------------------------- //
		if (my_random() < P -> go[state_of_alert][M[i2D(day - 1, i, N)]].h)
		{
			int64_t j = 0;
			while (h[i2D(ID[i].h, j, loc_sizes -> cap.h)] != N + 1)
			{
				j += 1;
			}
			h[i2D(ID[i].h, j, loc_sizes -> cap.h)] = i;
		}
	 // ----------------------------------------------------------------- work ----------------------------------------------------- //
		if (my_random() < P -> go[state_of_alert][M[i2D(day - 1, i, N)]].w)
		{
			int64_t j = 0;
			while (w[i2D(ID[i].w, j, loc_sizes -> cap.w)] != N + 1)
			{
				j += 1;
			}
			w[i2D(ID[i].w, j, loc_sizes -> cap.w)] = i;
		}
	 // --------------------------------------------------------------- hospital --------------------------------------------------- //
		if (my_random() < P -> go[state_of_alert][M[i2D(day - 1, i, N)]].hosp)
		{
			in_hosp[i] = 1;
			int64_t j = 0;
			while (hosp[i2D(ID[i].hosp, j, loc_sizes -> cap.hosp)] != N + 1)
			{
				j += 1;
			}
			hosp[i2D(ID[i].hosp, j, loc_sizes -> cap.hosp)] = i;
		}
    }
}

// ================================================================================================================================ //
// ============================================================= contagion ======================================================== //
// ================================================================================================================================ //
void contagion(int64_t N, int64_t day, int64_t *M, sizes_locations *loc_sizes, individual *ID, Prob *P, int64_t *h, int64_t *w, int64_t *hosp, int64_t *h_infected, int64_t *w_infected, int64_t *hosp_infected, int64_t *hosp_sizes, int64_t *collapsed_hosps, double percentage_collapse, int64_t *N_collapsed_hosps)
{
// -------------------------------------------------------------- households ------------------------------------------------------ //
	int64_t N_infected_h;
	for(int64_t i = 0; i <= loc_sizes -> ID.h; i++)
    {
	 //fprintf( stderr, "i_ID_h = %"PRIu64"\n", i );
	 // ---------- count number of infected per household ---------- //
		N_infected_h = 0;
		int64_t j = 0;
		while ( (h[i2D(i, j, loc_sizes -> cap.h)] != N + 1) && (j < loc_sizes -> cap.h) )
		{
			if ( M[i2D(day - 1, h[i2D(i, j, loc_sizes -> cap.h)], N)] == 1 || M[i2D(day - 1, h[i2D(i, j, loc_sizes -> cap.h)], N)] == 2 )
			{
				N_infected_h += 1;
			}
			j += 1;
		}
	 // ---------- contagion of S's within that household ---------- //
		j = 0;
		while ( (h[i2D(i, j, loc_sizes -> cap.h)] != N + 1) && (j < loc_sizes -> cap.h) )
		{
			if ( M[i2D(day - 1, h[i2D(i, j, loc_sizes -> cap.h)], N)] == 0 )
			{	
				for(int64_t k = 0; k < N_infected_h; k++)
				{
					if (my_random() < P -> infect.h)
					{
						h_infected[h[i2D(i, j, loc_sizes -> cap.h)]] = 1;
					}
				}
			}
			j += 1;
		}
    }
// -------------------------------------------------------------- work ------------------------------------------------------ //
	int64_t N_infected_w;
	for(int64_t i = 0; i <= loc_sizes -> ID.w; i++)
    {
	 // ---------- count number of infected per household ---------- //
		N_infected_w = 0;
		int64_t j = 0;
		while ( (w[i2D(i, j, loc_sizes -> cap.w)] != N + 1) && (j < loc_sizes -> cap.w) )
		{	
			if ( M[i2D(day - 1, w[i2D(i, j, loc_sizes -> cap.w)], N)] == 1 || M[i2D(day - 1, w[i2D(i, j, loc_sizes -> cap.w)], N)] == 2 )
			{
				N_infected_w += 1;
			}
			j += 1;
		}
	 // ---------- contagion of S's within that work ---------- //
		j = 0;
		while ( (w[i2D(i, j, loc_sizes -> cap.w)] != N + 1) && (j < loc_sizes -> cap.w) )
		{
			if ( M[i2D(day - 1, w[i2D(i, j, loc_sizes -> cap.w)], N)] == 0 )
			{	
				for(int64_t k = 0; k < N_infected_w; k++)
				{
					if (my_random() < P -> infect.w)
					{
						w_infected[w[i2D(i, j, loc_sizes -> cap.w)]] = 1;
					}
				}
			}
			j += 1;
		}
    }
// -------------------------------------------------------------- hosp ------------------------------------------------------ //
	int64_t N_infected_hosp;
	int64_t *N_in_hosp; N_in_hosp = (int64_t *) calloc( loc_sizes -> ID.w + 1, sizeof(int64_t) );
	*N_collapsed_hosps = 0;
	for(int64_t i = 0; i <= loc_sizes -> ID.hosp; i++)
    {
		N_in_hosp[i] = 0;
	 // ---------- count number of infected per household ---------- //
		N_infected_hosp = 0;
		int64_t j = 0;
		while ( (hosp[i2D(i, j, loc_sizes -> cap.hosp)] != N + 1) && (j < loc_sizes -> cap.hosp) )
		{	
			N_in_hosp[i] += 1;
			if ( M[i2D(day - 1, hosp[i2D(i, j, loc_sizes -> cap.hosp)], N)] == 1 || M[i2D(day - 1, hosp[i2D(i, j, loc_sizes -> cap.hosp)], N)] == 2 )
			{
				N_infected_hosp += 1;
			}
			j += 1;
		}
	 // ---------- contagion of S's within that work ---------- //
		j = 0;
		while ( (hosp[i2D(i, j, loc_sizes -> cap.hosp)] != N + 1) && (j < loc_sizes -> cap.hosp) )
		{
			if ( M[i2D(day - 1, hosp[i2D(i, j, loc_sizes -> cap.hosp)], N)] == 0 )
			{	
				for(int64_t k = 0; k < N_infected_hosp; k++)
				{
					if (my_random() < P -> infect.hosp)
					{
						hosp_infected[hosp[i2D(i, j, loc_sizes -> cap.hosp)]] = 1;
					}
				}
			}
			j += 1;
		}
		//fprintf( stderr, "N_in_hosp[i = %"PRIu64"] = %"PRIu64"\n", i,  N_in_hosp[i]);
		//fprintf( stderr, "hosp_sizes[i = %"PRIu64"] = %"PRIu64"\n", i,  hosp_sizes[i]);
		//fprintf( stderr, "(double)N_in_hosp[i] / (double)hosp_sizes[i] = %f\n", (double)N_in_hosp[i] / (double)hosp_sizes[i]);
		if ( (double)N_in_hosp[i] / (double)hosp_sizes[i] > percentage_collapse )
		{	
			collapsed_hosps[i] = 1;
			*N_collapsed_hosps += 1;
		}
    }
}

// ================================================================================================================================ //
// ======================================================== illness_progress ====================================================== //
// ================================================================================================================================ //
void illness_progress(int64_t N, int64_t day, int64_t *M, individual *ID, Prob *P, int64_t *new_states, int64_t *in_hosp, int64_t *collapsed_hosps)
{
	double mc_progress;
 	for(int64_t i = 0; i < N; i++)
    {	
		int64_t hosp_state; 
		if ( in_hosp[i] == 0 )
		{	
			hosp_state = 0;
		}
		else
		{
			if ( collapsed_hosps[ID[i].hosp] == 0 )
			{	
				hosp_state = 1;
			}
			else
			{
				hosp_state = 2;
			}
		}

		int64_t j = 0;
		mc_progress = my_random();					
		while (mc_progress > P -> evolve_cumulative[hosp_state][M[i2D(day - 1, i, N)]][j])
		{
			j += 1;
		}
		new_states[i] = j;
	}
}

// ================================================================================================================================ //
// ============================================================ new_step ========================================================== //
// ================================================================================================================================ //
void new_step(int64_t N, int64_t day, int64_t *M, int64_t *new_states, int64_t *h_infected, int64_t *w_infected, int64_t *hosp_infected)
{
	for(int64_t i = 0; i < N; i++)
	{
		if ((h_infected[i] + w_infected[i] + hosp_infected[i]) != 0)
		{
			new_states[i] = 1;
		}
		M[i2D(day, i, N)] = new_states[i];
	}
}