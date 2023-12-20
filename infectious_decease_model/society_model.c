#include "society_model.h"
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
// ====================================================== build_social_graph ====================================================== //
// ================================================================================================================================ //
void build_social_graph(int64_t N, sizes_locations *loc_sizes, individual *ID, int64_t *hosp_sizes)
{
	int64_t *list_h; list_h = (int64_t *) calloc( N, sizeof(int64_t) );
	int64_t *list_w; list_w = (int64_t *) calloc( N, sizeof(int64_t) );
	int64_t *list_hosp; list_hosp = (int64_t *) calloc( N, sizeof(int64_t) );
// -------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------- HOUSEHOLDS ------------------------------------------------------ //
// -------------------------------------------------------------------------------------------------------------------------------- //
	// https://www.ine.es/en/prensa/ech_2017_en.pdf
	int64_t N_sizes_h = 6;
	double prob_h_size[N_sizes_h];
	prob_h_size[0] = 0.254;
	prob_h_size[1] = 0.304;
	prob_h_size[2] = 0.209;
	prob_h_size[3] = 0.176;
	prob_h_size[4] = 0.0285;
	prob_h_size[5] = 0.0285;

	double mc_h_size[N_sizes_h];
	double dummy_h = 0;
	for(int64_t i = 0; i < N_sizes_h; i++)
    { 
		mc_h_size[i] = 1 - ( dummy_h + prob_h_size[i]);
		dummy_h += prob_h_size[i];
	}

	int64_t ID_h = 0;
	int64_t counter_h;

	list_h[0] = ID_h;
	counter_h = 0;
	loc_sizes -> cap.h = 1;
	for(int64_t i = 1; i < N; i++)
    {	
		if (my_random() < mc_h_size[counter_h])
		{
			counter_h += 1;
			list_h[i] = ID_h;
			if (counter_h + 1 > loc_sizes -> cap.h)
			{
				loc_sizes -> cap.h = counter_h + 1;
			}
		}
		else
		{	
			counter_h = 0;
			ID_h += 1;
			list_h[i] = ID_h;
		}
    }
	loc_sizes -> ID.h = ID_h;
	shuffle(list_h, N);
// -------------------------------------------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------ WORK -------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------- //

	//NEED DATA
	int64_t N_sizes_w = 6;
	double prob_w_size[N_sizes_w];
	prob_w_size[0] = 0.254;
	prob_w_size[1] = 0.304;
	prob_w_size[2] = 0.209;
	prob_w_size[3] = 0.176;
	prob_w_size[4] = 0.0285;
	prob_w_size[5] = 0.0285;

	double mc_w_size[N_sizes_w];
	double dummy_w = 0;
	for(int64_t i = 0; i < N_sizes_w; i++)
    {
		mc_w_size[i] = 1 - ( dummy_w + prob_w_size[i]);
		dummy_w += prob_w_size[i];
	}

	int64_t ID_w = 0;
	int64_t counter_w;

	list_w[0] = ID_w;
	counter_w = 0;
	loc_sizes -> cap.w = 1;
	for(int64_t i = 1; i < N; i++)
    {
		if (my_random() < mc_w_size[counter_w])
		{
			counter_w += 1;
			list_w[i] = ID_w;
			if (counter_w + 1 > loc_sizes -> cap.w)
			{
				loc_sizes -> cap.w = counter_w + 1;
			}
		}
		else
		{	
			counter_w = 0;
			ID_w += 1;
			list_w[i] = ID_w;
		}	
    }
	loc_sizes -> ID.w = ID_w;
	shuffle(list_w, N);

// -------------------------------------------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------ HOSP -------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------- //

	//https://www.mscbs.gob.es/ciudadanos/prestaciones/centrosServiciosSNS/hospitales/home.htm
	int64_t capacity_hosp = 100;//not saturated (characteristic capacity hospitals)
	int64_t N_people_per_bed = 325;
	int64_t N_people_per_hosp = capacity_hosp * N_people_per_bed;

	int64_t ID_hosp = 0;
	int64_t counter_hosp;

	list_hosp[0] = ID_hosp;
	counter_hosp = 0;
	loc_sizes -> cap.hosp = 1;
	for(int64_t i = 1; i < N; i++)
    {
		if (counter_hosp < N_people_per_hosp)
		{
			counter_hosp += 1;
			list_hosp[i] = ID_hosp;
			if (counter_hosp + 1 > loc_sizes -> cap.hosp)
			{
				loc_sizes -> cap.hosp = counter_hosp + 1;
			}
		}
		else
		{	
			hosp_sizes[ID_hosp] = counter_hosp;
			hosp_sizes = realloc(hosp_sizes, (ID_hosp + 1) * sizeof(int64_t));
			//fprintf( stderr, "hosp_sizes[ ID_hosp = %"PRIu64"] = %"PRIu64"\n",ID_hosp, hosp_sizes[1]);
			counter_hosp = 0;
			ID_hosp += 1;
			list_hosp[i] = ID_hosp;
		}
    }
	hosp_sizes[ID_hosp] = counter_hosp;
	loc_sizes -> ID.hosp = ID_hosp;
	shuffle(list_hosp, N);
	
	for(int64_t i = 0; i < N; i++)
    {
		ID[i].h = list_h[i];
		ID[i].w = list_w[i];
		ID[i].hosp = list_hosp[i];
    }
	
}

// ================================================================================================================================ //
// ========================================================= initilaize_h ========================================================= //
// ================================================================================================================================ //
void initialize_h(int64_t N, sizes_locations *loc_sizes, individual *ID, int64_t *h)
{
	for(int64_t i = 0; i <= loc_sizes -> ID.h; i++)
	{	
		for(int64_t j = 0; j < loc_sizes -> cap.h; j++)
		{
			h[i2D(i, j, loc_sizes -> cap.h)] = N + 1;
		}
	}
}

// ================================================================================================================================ //
// ========================================================= initilaize_w ========================================================= //
// ================================================================================================================================ //
void initialize_w(int64_t N, sizes_locations *loc_sizes, individual *ID, int64_t *w)
{
	for(int64_t i = 0; i <= loc_sizes -> ID.w; i++)
	{	
		for(int64_t j = 0; j < loc_sizes -> cap.w; j++)
		{
			w[i2D(i, j, loc_sizes -> cap.w)] = N + 1;
		}
	}
}

// ================================================================================================================================ //
// ========================================================= initilaize_hosp ========================================================= //
// ================================================================================================================================ //
void initialize_hosp(int64_t N, sizes_locations *loc_sizes, individual *ID, int64_t *hosp)
{
	for(int64_t i = 0; i <= loc_sizes -> ID.hosp; i++)
	{	
		for(int64_t j = 0; j < loc_sizes -> cap.hosp; j++)
		{
			hosp[i2D(i, j, loc_sizes -> cap.hosp)] = N + 1;
		}
	}
}
