#include "functions.h"

#include <inttypes.h>

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void sampling(int64_t N, int64_t N_study, int64_t *N_of, int64_t *M, int64_t day, int64_t survey_delay);

void declare_alert(int64_t state_of_alert, int64_t N_study, int64_t *N_of, int64_t days_in_alert, int64_t threshold_days_in_alert, double percentage_alert);

void declare_alert1(int64_t *state_of_alert, int64_t day);

void people_going_places(int64_t N, int64_t day, int64_t *M, sizes_locations *loc_sizes, individual *ID, Prob *P, int64_t *h, int64_t *w, int64_t *hosp, int64_t *in_hosp);

void contagion(int64_t N, int64_t day, int64_t *M, sizes_locations *loc_sizes, individual *ID, Prob *P, int64_t *h, int64_t *w, int64_t *hosp, int64_t *h_infected, int64_t *w_infected, int64_t *hosp_infected, int64_t *hosp_sizes, int64_t *collapsed_hosps, double percentage_collapse, int64_t *N_collapsed_hosps);

void illness_progress(int64_t N, int64_t day, int64_t *M, individual *ID, Prob *P, int64_t *new_states, int64_t *in_hosp, int64_t *collapsed_hosps);

void new_step(int64_t N, int64_t day, int64_t state_of_alert, int64_t *M, int64_t *new_states, int64_t *h_infected, int64_t *w_infected, int64_t *hosp_infected);

#endif /* FUNCTIONS_H_ */