#include "fix_p_num.h"

const fix_p_num fix_max = {
    FIXMAX,
    IWL_LIMIT
};

const fix_p_num fix_min = {
    FIXMIN,
    IWL_LIMIT
};

const fix_p_num one = {
    0x40000000,
    1
};

const fix_p_num zero = {
    0,
    1
};

void fix_p_num_min_int_len(fix_p_num* x) {
    unsigned int mask = 0x40000000;
    if (x == NULL) return;
    if (x->int_len < 2) return;
    if (x->value < 0) { // negative
        x->value = ~x->value;
        while (!(x->value & mask) && x->int_len > 1) {
            x->value <<= 1;
            x->int_len--;
        }
        x->value = ~x->value;
    }
    else {
        while (!(x->value & mask) && x->int_len > 1) {
            x->value <<= 1;
            x->int_len--;
        }
    }
}

static int fix_p_num_sign(fix_p_num* x) {
    return ((x->value >> 31) & 0x1);
}

static fix_p_num fix_p_num_abs(fix_p_num* fix)
{   
    struct fix_p_num result;
    result.int_len = fix->int_len;
    if(fix->value < 0) {
        result.value = -fix->value;
        return result;
    }
    result.value = fix->value;
    return result;
} 
int fix_p_num_toi(fix_p_num* fix)
{
    return((int)(fix->value >> (IWL_LIMIT - fix->int_len)));

}
static __inline__ fix_p_num fix_p_num_neg(fix_p_num* arg)
{
    fix_p_num result;
    result.value = -arg->value;
    result.int_len = arg->int_len;
    return result;
} 

fix_p_num fix_p_num_add(fix_p_num *x, fix_p_num *y) {
    struct fix_p_num result;
    unsigned short iwl = (x->int_len > y->int_len) ? x->int_len + 1 : y->int_len + 1;

    if (iwl > IWL_LIMIT) {
        return fix_max;
    }

    if (x->int_len > y->int_len) {
        result.value = (x->value >> 1) + (y->value >> (x->int_len - y->int_len + 1));
    }
    else if (x->int_len < y->int_len) {
        result.value = (y->value >> 1) + (x->value >> (y->int_len - x->int_len + 1));
    }
    else { //equal
        result.value = (x->value >> 1) + (y->value >> 1);
    }
    result.int_len = iwl;

    /* Set int len as small as possible */
    fix_p_num_min_int_len(&result);
    return result;
}

fix_p_num fix_p_num_sub(fix_p_num *x, fix_p_num *y) {
    fix_p_num result;
    unsigned short iwl = (x->int_len > y->int_len) ? x->int_len + 1 : y->int_len + 1;

    if (iwl > IWL_LIMIT) {
        return fix_max;
    }

    if (x->int_len > y->int_len) {
        result.value = (x->value >> 1) - (y->value >> (x->int_len - y->int_len + 1));
    }
    else if (x->int_len < y->int_len) {
        result.value = (x->value >> (y->int_len - x->int_len + 1)) - (y->value >> 1);
    }
    else { //equal
        result.value = (x->value >> 1) - (y->value >> 1);
    }
    result.int_len = iwl;

    /* Set int len as small as possible */
    fix_p_num_min_int_len(&result);
    return result;
}

fix_p_num fix_p_num_mul(fix_p_num *x, fix_p_num *y) {
    fix_p_num result;
    fix_p_num tmp_x = {x->value, x->int_len};
    fix_p_num tmp_y = {y->value, y->int_len};
    unsigned short iwl = x->int_len + y->int_len;
    if (iwl > IWL_LIMIT) {
        if (fix_p_num_sign(x) ^ fix_p_num_sign(y)) {
            return fix_min;
        }
        else {
            return fix_max;
        }
    }
    if (iwl > x->int_len) {
        tmp_x.value >>= y->int_len;
    }
    if (iwl > y->int_len) {
        tmp_y.value >>= x->int_len;
    }
    result.int_len = iwl;
    result.value = tmp_x.value;
    __asm__ (
        "imull %%ebx\n\t" /* multiply x*y, results are put in edx and ebx*/
        "shrdl %%cl, %%edx, %%eax\n\t" /* shift frac part to maintain same frac part bit num */
        :"=a"(result.value)
        :"a"(result.value), "b"(tmp_y.value), "c"(IWL_LIMIT-iwl)
        :"edx"
    );
    fix_p_num_min_int_len(&result);
    return result;
}

fix_p_num fix_p_num_div(fix_p_num* x, fix_p_num *y) {
    fix_p_num result;
    fix_p_num tmp_x = fix_p_num_abs(x);
    fix_p_num tmp_y = fix_p_num_abs(y);
    unsigned short iwl = x->int_len;
    int sign;
    
    sign = fix_p_num_sign(x) ^ fix_p_num_sign(y);

    printf("%x, %d\n", tmp_y.value, tmp_y.int_len);
    if(y->value == 0) { /* divided by zero*/
        if(sign) {
            return fix_min;
        } else {
            return fix_max;
        }
    }

    if (x->int_len > y->int_len) {
        tmp_y.value >>= (x->int_len-y->int_len);
        tmp_y.int_len += (x->int_len-y->int_len);
        iwl = x->int_len;
        if (tmp_y.value == 0) {
            if(sign) {
                return fix_min;
            } else {
                return fix_max;
            }
        }
    }
    else if (y->int_len > x->int_len) {
        tmp_x.value >>= (y->int_len-x->int_len);
        tmp_x.int_len += (y->int_len-x->int_len);
        iwl = y->int_len;
        if (tmp_x.value == 0) {
            result.value = 0;
            result.int_len = 1;
            return result;
        }
    }

    if(fix_p_num_toi(&tmp_y) == 0 || fix_p_num_toi(&tmp_y) == -1) {
        int tmp = tmp_y.value;
        /*
         * |y| < 1
         */
        tmp <<= tmp_y.int_len;
        while(!(tmp & (1 << IWL_LIMIT))) {
            tmp <<= 1;
            iwl++;
        }
        //iwl = iwl + x->int_len;
        if(iwl > IWL_LIMIT) {
            if(sign) {
                return fix_min;
            } else {
                return fix_max;
            }
        }
    }

    result.int_len = iwl;
    result.value = tmp_x.value;

    printf("%x, %d\n", result.value, result.int_len);

    __asm__ (
        "xorl %%edx, %%edx\n\t"        /* Clear edx                           */
        "shldl %%cl, %%eax, %%edx\n\t" /* Shift edx left and fill in from eax */
        "sall  %%cl, %%eax\n\t"        /* Shift eax upward                    */
        "idivl %%ebx\n\t"              /* Divide (edx:eax) / ebx              */
        :"=a"(result.value)
        :"a"(result.value), "b"(tmp_y.value), "c"(IWL_LIMIT-iwl)
        :"edx"
    );
    fix_p_num_min_int_len(&result);
    if(sign) {
        return fix_p_num_neg(&result);
    } else {
        return result;
    }
}

int fix_init_d(fix_p_num *fix, double d)
{
    unsigned int iwl = 1;
    int p = 1;

    if(!fix) {
        return 1;
    }
    /*
     * find out how many bits are needed to represent d.
     */
    for(iwl = 1; iwl <= IWL_LIMIT; iwl++) {
        if(fabs(d) <= p) break;
        p <<= 1;
    }
    if(iwl > IWL_LIMIT) {
        if(d >= 0.) {
            *fix = fix_max;
        } else {
            *fix = fix_min;
        }
    } else {
        fix->int_len = iwl;
        fix->value = (int)(d * (1 << (IWL_LIMIT - fix->int_len)));
    }
    fix_p_num_min_int_len(fix);
    return 0;

} 

int fix_p_num_gt(fix_p_num *x, fix_p_num *y) {
    int sign = fix_p_num_sign(x) ^ fix_p_num_sign(y);
    if (sign) {
        if (fix_p_num_sign(x) == 0) return 1;
        else return 0;
    }
    if (x->int_len > y->int_len || x->int_len == y->int_len && x->value > y->value) 
        return 1;
    else 
        return 0;
}
int fix_p_num_eq(fix_p_num *x, fix_p_num *y) {
    if (x->value == y->value && x->int_len == y->int_len) 
        return 1;
    else 
        return 0;
}