#include <stdio.h>
#include <math.h>
#include "my_decimal.h"

int main(void) {
    my_decimal temp1 = {{10256, 0, 0, -2147483648}};
    my_decimal temp2 = {{11, 0, 0, 0}};
    my_decimal temp3 = {{0, 0, 0, 0}};
    short err = my_mod(temp1, temp2, &temp3);
    printf("Код ошибки = %d\n", err);
    return 0;
}
