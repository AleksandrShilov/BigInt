#include <stdio.h>
#include <math.h>
#include "s21_decimal.h"

int main(void) {
    s21_decimal temp1 = {{10256, 0, 0, -2147483648}};
    s21_decimal temp2 = {{11, 0, 0, 0}};
    s21_decimal temp3 = {{0, 0, 0, 0}};
    short err = s21_mod(temp1, temp2, &temp3);
    printf("Код ошибки = %d\n", err);
    return 0;
}
