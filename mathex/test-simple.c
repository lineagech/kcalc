#include "expression.h"

#include <stdio.h>
#include <string.h>

#if !FIXPOP
/* Custom function that returns the sum of its two arguments */
static float add(struct expr_func *f, vec_expr_t args, void *c)
{
    float a = expr_eval(&vec_nth(&args, 0));
    float b = expr_eval(&vec_nth(&args, 1));
    return a + b;
}

static struct expr_func user_funcs[] = {
    {"add", add, 0},
    {NULL, NULL, 0},
};

int main()
{
    const char *s = "x = 40, add(2, x)";
    struct expr_var_list vars = {0};
    struct expr *e = expr_create(s, strlen(s), &vars, user_funcs);
    if (!e) {
        printf("Syntax error");
        return 1;
    }

    float result = expr_eval(e);
    printf("result: %f\n", result);

    expr_destroy(e, &vars);
    return 0;
}

#else 

static struct expr_func user_funcs[] = {
    {NULL, NULL, 0},
};

int main()
{
    const char *s = "x = 40.123 * 20.456 + 123.12 / 12";
    struct expr_var_list vars = {0};
    struct expr *e = expr_create(s, strlen(s), &vars, user_funcs);
    if (!e) {
        printf("Syntax error");
        return 1;
    }

    fix_p_num result = expr_eval(e);
    printf("result: %f\n", fix_p_num_to_float(&result));

    expr_destroy(e, &vars);
    return 0;
}

#endif
