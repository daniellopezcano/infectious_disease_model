#include "functions.h"

#include <inttypes.h>

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void build_social_graph(int64_t N, sizes_locations *loc_sizes, individual *ID, int64_t *hosp_sizes);

void initialize_h(int64_t N, sizes_locations *loc_sizes, individual *ID, int64_t *h);

void initialize_w(int64_t N, sizes_locations *loc_sizes, individual *ID, int64_t *w);

void initialize_hosp(int64_t N, sizes_locations *loc_sizes, individual *ID, int64_t *hosp);

#endif /* FUNCTIONS_H_ */