#include "type.h"

//small datasize
#ifdef SMALL 
#define NA        7000
#define NONZER    8
#define SHIFT     12
#define NITER     15
#define RCOND     1.0e-1
#define VALID_RESULT 10.362595087124
#endif

//midiumn datasize
#ifdef MEDIUMN
#define NA        14000
#define NONZER    11
#define SHIFT     20
#define NITER     15
#define RCOND     1.0e-1
#define VALID_RESULT 17.130235054029
#endif

//large datasize
#ifdef LARGE
#define NA        75000
#define NONZER    13
#define SHIFT     60
#define NITER     75
#define RCOND     1.0e-1
#define VALID_RESULT 22.712745482631
#endif

#define NZ    (NA*(NONZER+1)*(NONZER+1))
#define NAZ   (NA*(NONZER+1))

#define T_init        0
#define T_bench       1
#define T_conj_grad   2

#define T_150 3
#define T_162 4
#define T_169 5
#define T_200 6
#define T_214 7
#define T_226 8
#define T_262 9
#define T_281 10
#define T_345 11
#define T_360 12
#define T_385 13
#define T_399 14
#define T_420 15
#define T_433 16
#define T_448 17
#define T_462 18
#define T_475 19


#define T_last 20

