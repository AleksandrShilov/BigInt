#ifndef SRC_SECONDARY_FUNCTIONS_H_
#define SRC_SECONDARY_FUNCTIONS_H_

#define s21_TRUE 1
#define s21_FALSE 0

#include <limits.h>
#include "s21_decimal.h"

// мини функции для работы с s21_decimal
// возвращаем знак
short get_sign(s21_decimal decimal);
// меняем знак с - на + без потери степени
void clean_sign(s21_decimal *decimal);
// меняем знак с + на - без потери степе
void set_sign(s21_decimal *decimal);
// Возвращает степень
short get_scale(s21_decimal decimal);
// устанавливаем i бит для decimal
void set_bit_decimal(int *tmp, short i);

// это можно переделать в дефайн, хотя так вроде тоже работает
typedef enum { OK, s21_INF, s21_N_INF, s21_NAN
} value_type_t;

// структура еще большего размера чем s21_decimal,
// с отдельными полями для знака и степени для удобства работы
typedef struct {
    int bits[7];
    int sign;
    int scale;
} long_decimal;

// конвертирует s21_deimal в long_decimal
long_decimal convert_in_long_dec(s21_decimal decimal);

// конвертирует long_decimal в s21_deimal
s21_decimal convert_from_long_to_decimal(long_decimal long_dec);

// возвращаем i бит
short get_bit(long_decimal long_dec, short i);

// устанавливаем i бит
void set_bit(long_decimal *long_dec, short i);

// сбрасываем i бит
void clean_bit(long_decimal *long_dec, short i);

// двигаем long_decimal на 1 бит влево
void move_left(long_decimal *long_dec);

// сложение двух long_decimal без учета знака и степени
void sum(long_decimal long_dec1, long_decimal long_dec2, long_decimal *result);

// увеличиваем степень на 1
void scale_up(long_decimal *long_dec);

// умножаем число на 10
void ten_up(long_decimal *long_dec);

// приводит 2 long_decimal к одной степени
short normalization(long_decimal *long_dec1, long_decimal *long_dec2);

// зануление long_decimal
void clean_struct_long_dec(long_decimal *long_dec);

// равенство 0 и -0
int equal_nul(long_decimal long_dec1, long_decimal long_dec2);

// логическое равно == для long_decimal
int equal_long_decimal(long_decimal long_dec1, long_decimal long_dec2);

// логическое больше > для long_dec
short greater_for_long_dec(long_decimal long_dec1, long_decimal long_dec2);

// логическое больше или равно >=
int greater_or_equal_long_decimal(long_decimal long_dec1, long_decimal long_dec2);

// логическое меньше < для long_decimal
short less_for_long_dec(long_decimal long_dec1, long_decimal long_dec2);

// сравнивает значение двух long_decimal
int compare_digit_long_decimal(long_decimal long_dec1, long_decimal long_dec2);

// сравнивает значение двух long_decimal
int compare_digit_long_decimal_great_or_equal(long_decimal long_dec1, long_decimal long_dec2);

// проверка на ноль long_decimal
short equal_long_decimal_nul(long_decimal long_dec1);

// обработка ошибки
short error(long_decimal long_dec);

// инверсия
void inversion(long_decimal *long_dec);

// вычитание из большего без учета знака и степени
void sub_simple(long_decimal long_dec1, long_decimal long_dec2, long_decimal *long_dec_res);

// вычитание для long_decimal
void sub_long_decimal(long_decimal long_dec1, long_decimal long_dec2, long_decimal *long_dec3);

// АРИФМЕТИКА
// устанавливает знак в результат
void set_sign_result(long_decimal long_dec1, long_decimal long_dec2, long_decimal *result);

// проверяет и устанавливает степень
int check_scale(long_decimal long_dec1, long_decimal long_dec2);

// выбирает большее число для вычитания
void choosing_large_divisible(long_decimal long_dec1, long_decimal long_dec2, long_decimal *long_dec3);

// вычисление целой части при делении
void calculat_integer(long_decimal long_dec1, long_decimal long_dec2, long_decimal *result, short flag_mod);

// возвращает остаток от деления на 10
int long_decimal_mod_10(long_decimal src);

// остаток от деления целых чисел
short calculating_mod_from_int(long_decimal long_dec1, long_decimal long_dec2,
long_decimal *result, short err);

// умножение для long_decimal
void mul_long_decimal(long_decimal long_dec1, long_decimal long_dec2, long_decimal *long_dec3);

// вычисляем результат деления на целое число
void calculat_result_div_int(long_decimal long_dec1, long_decimal long_dec2,
long_decimal long_dec1_integer, long_decimal *long_temp);

// удаляем нули из результа в мод
void removing_zeros_from_mod_result(long_decimal *long_dec_res);

// вычисление дробной части
void calculat_fraction(long_decimal *long_dec1, long_decimal long_dec2, long_decimal *long_dec_last_res);

// целочислено деление
short div_int(long_decimal long_dec1, long_decimal long_dec2, long_decimal *result);

// проверка long_decimal на вместимость в decimal
short check_long_decimal(long_decimal long_dec);

// банковское округление
void bank_rounding(long_decimal value, long_decimal *result);

// возвращает целые цифры указанного long_decimal числа
void long_truncate(long_decimal value, long_decimal *result);

void clean_dec(s21_decimal *dec);

int found_elder_bit(s21_decimal dec);

#endif  // SRC_SECONDARY_FUNCTIONS_H_
