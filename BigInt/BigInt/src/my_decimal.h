#ifndef SRC_MY_DECIMAL_H_
#define SRC_MY_DECIMAL_H_

#define my_TRUE 1
#define my_FALSE 0

#include <math.h>

typedef struct {
    int bits[4];
} my_decimal;

// АРИФМЕТИКА. Возвращают код ошибки:
// 0 - OK
// 1 - число слишком велико или равно бесконечности
// 2 - число слишком мало или равно отрицательной бесконечности
// 3 - деление на 0

// Сложение
int my_add(my_decimal value_1, my_decimal value_2, my_decimal *result);

// Вычитание
int my_sub(my_decimal value_1, my_decimal value_2, my_decimal *result);

// Умножение
int my_mul(my_decimal value_1, my_decimal value_2, my_decimal *result);

// Деление
int my_div(my_decimal value_1, my_decimal value_2, my_decimal *result);

// Отстаток от деления
int my_mod(my_decimal value_1, my_decimal value_2, my_decimal *result);


// ОПЕРАТОРЫ СРАВНЕНИЯ - возвращают значения: 0 - FALSE, 1 - TRUE
// Меньше
int my_is_less(my_decimal decimal1, my_decimal decimal2);

// Меньше или равно
int my_is_less_or_equal(my_decimal decimal1, my_decimal decimal2);

// Больше
int my_is_greater(my_decimal decimal1, my_decimal decimal2);

// Больше или равно
int my_is_greater_or_equal(my_decimal decimal1, my_decimal decimal2);

// Равно
int my_is_equal(my_decimal decimal1, my_decimal decimal2);

// Не равно
int my_is_not_equal(my_decimal decimal1, my_decimal decimal2);


// ПРЕОРАЗОВАТЕЛИ. Возвращаемое значение - код ошибки:
// 0 - OK
// 1 - ошибка конвертации

// из int в decimal
int my_from_int_to_decimal(int src, my_decimal *dst);

// из float в decimal
int my_from_float_to_decimal(float src, my_decimal *dst);

// из decimal в int
int my_from_decimal_to_int(my_decimal src, int *dst);

// из decimal в float
int my_from_decimal_to_float(my_decimal src, float *dst);


// ДРУГИЕ ФУНКЦИИ. Возвращаемое значение - код ошибки:
// 0 - OK
// 1 - ошибка вычисления


// Округляет указанное Decimal число до ближайшего целого числа в сторону отрицательной бесконечности.
int my_floor(my_decimal value, my_decimal *result);

// Округляет Decimal до ближайшего целого числа.
int my_round(my_decimal value, my_decimal *result);

// Возвращает целые цифры указанного Decimal числа; любые дробные цифры отбрасываются, включая конечные нули.
int my_truncate(my_decimal value, my_decimal *result);

// Возвращает результат умножения указанного Decimal на -1.
int my_negate(my_decimal value, my_decimal *result);


#endif  // SRC_MY_DECIMAL_H_
