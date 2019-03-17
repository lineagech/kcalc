#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

typedef struct fix_p_num{
    int value;
    unsigned short int_len;
} fix_p_num;

#define IWL_LIMIT 31

#define FIXMAX  INT_MAX
#define FIXMIN  INT_MIN

fix_p_num fix_p_num_add(fix_p_num *x, fix_p_num *y);
fix_p_num fix_p_num_sub(fix_p_num *x, fix_p_num *y);
fix_p_num fix_p_num_mul(fix_p_num *x, fix_p_num *y);
fix_p_num fix_p_num_div(fix_p_num *x, fix_p_num *y);
int fix_init_d(fix_p_num *fix, double d);
void fix_p_num_min_int_len(fix_p_num* x);