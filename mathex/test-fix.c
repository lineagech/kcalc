#include <stdio.h>
#include <stdlib.h>
#include "fix_p_num.h"

static double fix_p_num_to_float(fix_p_num* fix) {
    int i;
    int sign = 0;
    double result = 0;
    double b = 1;

    if (((fix->value >> 31) & 0x1)) {
        fix->value = -fix->value;
        sign = 1;
    }

    for (i=0; i<fix->int_len; i++) { // integer part
        if ((fix->value >> (IWL_LIMIT-fix->int_len+i)) & 0x1) {
            result += b;
        }
        b *= 2;
    }
    b = 0.5;
    for (i=0; i<IWL_LIMIT-fix->int_len; i++) { // frac part
        if ((fix->value >> (IWL_LIMIT-fix->int_len-1-i)) & 0x1) {
            result += b;
        }
        b /= 2;
    }
    if (sign) {
        fix->value = -fix->value;
        return -result;
    }

    return result;
}

int main() {
    fix_p_num fix1; //1.75
    fix_p_num fix2; //3.875
    fix_p_num fix3; // 0.75
    fix_p_num result;

    fix_init_d(&fix1, 1.75); fix_p_num_min_int_len(&fix1);
    fix_init_d(&fix2, 3.875); fix_p_num_min_int_len(&fix2); 
    fix_init_d(&fix3, 0.75); fix_p_num_min_int_len(&fix3); 

    printf("%x, fix1 value is %f with int len %d\n", fix1.value, fix_p_num_to_float(&fix1), fix1.int_len);
    printf("%x, fix2 value is %f with int len %d\n", fix2.value, fix_p_num_to_float(&fix2), fix2.int_len);

    result = fix_p_num_add(&fix1, &fix2);
    printf("fix2+fix1 value is %f\n", fix_p_num_to_float(&result));

    result = fix_p_num_sub(&fix2, &fix1);
    printf("fix2-fix1 value is %f\n", fix_p_num_to_float(&result));

    result = fix_p_num_mul(&fix1, &fix2);
    printf("fix1*fix2 value is %f\n", fix_p_num_to_float(&result));

    result = fix_p_num_div(&fix2, &fix1);
    printf("fix2/fix1 value is %f\n", fix_p_num_to_float(&result));

    result = fix_p_num_div(&fix2, &fix3);
    printf("fix2/fix3 value is %f\n", fix_p_num_to_float(&result));

    return 0;
}