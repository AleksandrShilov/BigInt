#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#define s21_TRUE 1
#define s21_FALSE 0

#include <math.h>

typedef struct {
    int bits[4];
} s21_decimal;

// АРИФМЕТИКА. Возвращают код ошибки:
// 0 - OK
// 1 - число слишком велико или равно бесконечности
// 2 - число слишком мало или равно отрицательной бесконечности
// 3 - деление на 0

// Сложение
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// Вычитание
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// Умножение
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// Деление
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// Отстаток от деления
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);


// ОПЕРАТОРЫ СРАВНЕНИЯ - возвращают значения: 0 - FALSE, 1 - TRUE
// Меньше
int s21_is_less(s21_decimal decimal1, s21_decimal decimal2);

// Меньше или равно
int s21_is_less_or_equal(s21_decimal decimal1, s21_decimal decimal2);

// Больше
int s21_is_greater(s21_decimal decimal1, s21_decimal decimal2);

// Больше или равно
int s21_is_greater_or_equal(s21_decimal decimal1, s21_decimal decimal2);

// Равно
int s21_is_equal(s21_decimal decimal1, s21_decimal decimal2);

// Не равно
int s21_is_not_equal(s21_decimal decimal1, s21_decimal decimal2);


// ПРЕОРАЗОВАТЕЛИ. Возвращаемое значение - код ошибки:
// 0 - OK
// 1 - ошибка конвертации

// из int в decimal
int s21_from_int_to_decimal(int src, s21_decimal *dst);

// из float в decimal
int s21_from_float_to_decimal(float src, s21_decimal *dst);

// из decimal в int
int s21_from_decimal_to_int(s21_decimal src, int *dst);

// из decimal в float
int s21_from_decimal_to_float(s21_decimal src, float *dst);


// ДРУГИЕ ФУНКЦИИ. Возвращаемое значение - код ошибки:
// 0 - OK
// 1 - ошибка вычисления


// Округляет указанное Decimal число до ближайшего целого числа в сторону отрицательной бесконечности.
int s21_floor(s21_decimal value, s21_decimal *result);

// Округляет Decimal до ближайшего целого числа.
int s21_round(s21_decimal value, s21_decimal *result);

// Возвращает целые цифры указанного Decimal числа; любые дробные цифры отбрасываются, включая конечные нули.
int s21_truncate(s21_decimal value, s21_decimal *result);

// Возвращает результат умножения указанного Decimal на -1.
int s21_negate(s21_decimal value, s21_decimal *result);


#endif  // SRC_S21_DECIMAL_H_
