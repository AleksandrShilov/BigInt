#include "secondary_functions.h"
// возвращаем знак
short get_sign(s21_decimal decimal) {
    return (decimal.bits[3] & 2147483648) >> 31;
}

// меняем знак с - на + без потери степени
void clean_sign(s21_decimal *decimal) {
    decimal->bits[3] = decimal->bits[3] & 2147483647;  // 4294967295 для U
}

// меняем знак с + на - без потери степени
void set_sign(s21_decimal *decimal) {
    decimal->bits[3] = decimal->bits[3] | 2147483648;
}

// возвращает степень
short get_scale(s21_decimal decimal) {
    return (decimal.bits[3] & 16711680) >> 16;
}

// устанавливаем i бит для decimal
void set_bit_decimal(int *tmp, short i) {
    int bit = 1 << (i % 32);
    *tmp = (*tmp | bit);
}

// конвертирует s21_deimal в long_decimal
long_decimal convert_in_long_dec(s21_decimal decimal) {
    long_decimal res = {{
        decimal.bits[0],
        decimal.bits[1],
        decimal.bits[2],
        0,
        0,
        0,
        0},
        get_sign(decimal),
        get_scale(decimal)
    };
    return res;
}

// конвертирует long_decimal в s21_deimal
s21_decimal convert_from_long_to_decimal(long_decimal long_dec) {
    int scale = long_dec.scale << 16;
    s21_decimal res = {{
        long_dec.bits[0],
        long_dec.bits[1],
        long_dec.bits[2],
        scale}
    };
    if (long_dec.sign)
        set_sign(&res);
    return res;
}

// возвращаем i бит
short get_bit(long_decimal long_dec, short i) {
    int bit = 1 << (i % 32);
    return !!(long_dec.bits[i / 32] & bit);
}

// устанавливаем i бит для long_decimal
void set_bit(long_decimal *long_dec, short i) {
    int bit = 1 << (i % 32);
    long_dec->bits[i / 32] = (long_dec->bits[i / 32] | bit);
}

// сбрасываем i бит
void clean_bit(long_decimal *long_dec, short i) {
    int bit = 1 << (i % 32);
    long_dec->bits[i / 32] = (long_dec->bits[i / 32] & ~bit);
}

// двигаем long_decimal на 1 бит влево
void move_left(long_decimal *long_dec) {
    // 191-160 159-128 127-96 95-64 63-32 31-0
    long_dec->bits[6] = long_dec->bits[6] << 1;
    for (short i = 191; i > 30; i -= 32) {
        short z = 1 + i;
        if (get_bit(*long_dec, i)) {
            set_bit(long_dec, z);
            long_dec->bits[i / 32] = long_dec->bits[i / 32] << 1;
        } else {
            long_dec->bits[i / 32] = long_dec->bits[i / 32] << 1;
        }
    }
}

// сложение двух long_decimal без учета знака и степени
void sum(long_decimal long_dec1, long_decimal long_dec2, long_decimal *result) {
    short tmp = 0;
    for (short i = 0; i < 224; i++) {
        short d1 = get_bit(long_dec1, i);
        short d2 = get_bit(long_dec2, i);
        if (!d1 && !d2) {
            if (tmp) {
                set_bit(result, i);
                tmp = 0;
            } else {
                clean_bit(result, i);
            }
        } else if (!d1 && d2) {
            if (tmp) {
                clean_bit(result, i);
            } else {
                set_bit(result, i);
            }
        } else if (d1 && !d2) {
            if (tmp) {
                clean_bit(result, i);
            } else {
                set_bit(result, i);
            }
        } else if (d1 && d2) {
            if (tmp) {
                set_bit(result, i);
            } else {
                clean_bit(result, i);
                tmp = 1;
            }
        }
    }
}

// увеличиваем степень на 1
void scale_up(long_decimal *long_dec) {
    // 0-31 32-63 64-95 96-127 128-159 160-191
    long_decimal temp1 = {{
        long_dec->bits[0],
        long_dec->bits[1],
        long_dec->bits[2],
        long_dec->bits[3],
        long_dec->bits[4],
        long_dec->bits[5],
        long_dec->bits[6]},
        long_dec->sign,
        long_dec->scale
    };
        long_decimal temp2 = {{
        long_dec->bits[0],
        long_dec->bits[1],
        long_dec->bits[2],
        long_dec->bits[3],
        long_dec->bits[4],
        long_dec->bits[5],
        long_dec->bits[6]},
        long_dec->sign,
        long_dec->scale
    };
    for (int i = 0; i < 3; i++) {
        move_left(&temp1);
    }
    move_left(&temp2);
    sum(temp1, temp2, long_dec);
    long_dec->scale++;
}

// умножаем число на 10
void ten_up(long_decimal *long_dec) {
    long_decimal temp1 = {{
        long_dec->bits[0],
        long_dec->bits[1],
        long_dec->bits[2],
        long_dec->bits[3],
        long_dec->bits[4],
        long_dec->bits[5],
        long_dec->bits[6]},
        long_dec->sign,
        long_dec->scale
    };
        long_decimal temp2 = {{
        long_dec->bits[0],
        long_dec->bits[1],
        long_dec->bits[2],
        long_dec->bits[3],
        long_dec->bits[4],
        long_dec->bits[5],
        long_dec->bits[6]},
        long_dec->sign,
        long_dec->scale
    };
    for (int i = 0; i < 3; i++) {
        move_left(&temp1);
    }
    move_left(&temp2);
    sum(temp1, temp2, long_dec);
}

// приводит 2 long_decimal к одной степени
short normalization(long_decimal *long_dec1, long_decimal *long_dec2) {
    short res;
    short scale = long_dec1->scale;
    if (long_dec1->scale > long_dec2->scale) {
        res = long_dec1->scale - long_dec2->scale;
        for (; res; res--)
            scale_up(long_dec2);
        scale = long_dec1->scale;
    } else if (long_dec1->scale < long_dec2->scale) {
        res = long_dec2->scale - long_dec1->scale;
        for (; res; res--)
            scale_up(long_dec1);
        scale = long_dec2->scale;
    }
    return scale;
}

// зануление long_decimal
void clean_struct_long_dec(long_decimal *long_dec) {
    for (short i = 0; i < 7; i++) {
        long_dec->bits[i] = 0;
    }
    long_dec->sign = 0;
    long_dec->scale = 0;
}

// равенство 0 и -0
int equal_nul(long_decimal long_dec1, long_decimal long_dec2) {
    int err = 1;
    for (short i = 0; i < 7; i++) {
        if ((long_dec1.bits[i] != 0) || (long_dec2.bits[i] != 0)) {
            err = 0;
        }
    }
    return err;
}

// логическое равно == для long_decimal
int equal_long_decimal(long_decimal long_dec1, long_decimal long_dec2) {
    short result = s21_TRUE;
    normalization(&long_dec1, &long_dec2);
    if (long_dec1.sign == long_dec2.sign) {
        for (short i = 191; i >= 0; i--) {
            if ((get_bit(long_dec1, i)) != (get_bit(long_dec2, i))) {
                result = s21_FALSE;
                break;
            }
        }
    } else {
        if (!(equal_nul(long_dec1, long_dec2))) {
            result = s21_FALSE;
        }
    }
    return result;
}

// логическое больше > для long_dec
short greater_for_long_dec(long_decimal long_dec1, long_decimal long_dec2) {
    short result = s21_FALSE;
    normalization(&long_dec1, &long_dec2);
    if (long_dec1.sign && long_dec2.sign) {
        for (short i = 191; i >= 0; i--) {
            short bit1 = get_bit(long_dec1, i);
            short bit2 = get_bit(long_dec2, i);
            if (bit1 > bit2) {
                break;
            } else if (bit1 < bit2) {
                result = s21_TRUE;
                break;
            }
        }
    } else if (!long_dec1.sign && !long_dec2.sign) {
        for (short i = 191; i >= 0; i--) {
            short bit1 = get_bit(long_dec1, i);
            short bit2 = get_bit(long_dec2, i);
            if (bit1 > bit2) {
                result = s21_TRUE;
                break;
            } else if (bit1 < bit2) {
                break;
            }
        }
    } else if (long_dec1.sign < long_dec2.sign) {
        result = s21_TRUE;
    }
    return result;
}

// логическое больше или равно >=
int greater_or_equal_long_decimal(long_decimal long_dec1, long_decimal long_dec2) {
    short result = s21_FALSE;
    short greater = greater_for_long_dec(long_dec1, long_dec2);
    short equal = equal_long_decimal(long_dec1, long_dec2);
    if (greater || equal)
        result = s21_TRUE;
    return result;
}

// логическое меньше < для long_decimal
short less_for_long_dec(long_decimal long_dec1, long_decimal long_dec2) {
    short result = s21_FALSE;
    normalization(&long_dec1, &long_dec2);
    if (long_dec1.sign && long_dec2.sign) {
        for (short i = 191; i >= 0; i--) {
            short bit1 = get_bit(long_dec1, i);
            short bit2 = get_bit(long_dec2, i);
            if (bit1 > bit2) {
                result = s21_TRUE;
                break;
            } else if (bit1 < bit2) {
                break;
            }
        }
    } else if (!long_dec1.sign && !long_dec2.sign) {
        for (short i = 191; i >= 0; i--) {
            short bit1 = get_bit(long_dec1, i);
            short bit2 = get_bit(long_dec2, i);
            if (bit1 > bit2) {
                break;
            } else if (bit1 < bit2) {
                result = s21_TRUE;
                break;
            }
        }
    } else if (long_dec1.sign > long_dec2.sign && !equal_nul(long_dec1, long_dec2)) {
        result = s21_TRUE;
    }
    return result;
}

// сравнивает значение двух long_decimal
int compare_digit_long_decimal(long_decimal long_dec1, long_decimal long_dec2) {
    short result = s21_FALSE;
    for (short i = 223; i >= 0; i--) {
        short bit1 = get_bit(long_dec1, i);
        short bit2 = get_bit(long_dec2, i);
        if (bit1 > bit2) {
            result = s21_TRUE;
            break;
        } else if (bit1 < bit2) {
            break;
        }
    }
    return result;
}

// сравнивает значение двух long_decimal
int compare_digit_long_decimal_great_or_equal(long_decimal long_dec1, long_decimal long_dec2) {
    short result = s21_TRUE;
    for (short i = 223; i >= 0; i--) {
        short bit1 = get_bit(long_dec1, i);
        short bit2 = get_bit(long_dec2, i);
        if (bit1 > bit2) {
            result = s21_TRUE;
            break;
        } else if (bit1 < bit2) {
            result = s21_FALSE;
            break;
        }
    }
    return result;
}

// проверка на ноль long_decimal
short equal_long_decimal_nul(long_decimal long_dec1) {
    short nul = 1;
    for (short i = 0; i < 7; i++) {
        if (long_dec1.bits[i] != 0) {
            nul = 0;
        }
    }
    return nul;
}

// обработка ошибки
short error(long_decimal long_dec) {
    long_decimal min_decimal_for_n_inf = {{-1, -1, -1, 0, 0, 0, 0}, 1, 0};
    long_decimal max_decimal_for_inf = {{-1, -1, -1, 0, 0, 0, 0}, 0, 0};
    long_decimal n_nan = {{1, 0, 0, 0, 0, 0, 0}, 1, 28};
    long_decimal nan = {{1, 0, 0, 0, 0, 0, 0}, 0, 28};
    long_decimal nul = {{0, 0, 0, 0, 0, 0, 0}, 0, 0};
    short err = OK;
    if (greater_for_long_dec(long_dec, max_decimal_for_inf)) {
        err = s21_INF;
    } else if (less_for_long_dec(long_dec, min_decimal_for_n_inf)) {
        err = s21_N_INF;
    } else if ((less_for_long_dec(long_dec, nan)) && greater_for_long_dec(long_dec, nul)) {
        err = s21_NAN;
    } else if ((greater_for_long_dec(long_dec, n_nan)) && (less_for_long_dec(long_dec, nul))) {
        err = s21_NAN;
    } else if (long_dec.scale > 28) {
        err = s21_NAN;
    }
    return err;
}

// инверсия
void inversion(long_decimal *long_dec) {
    for (short i = 0; i < 224; i++) {
        short bit = get_bit(*long_dec, i);
        if (bit) {
            clean_bit(long_dec, i);
        } else {
            set_bit(long_dec, i);
        }
    }
}

//  вычитание из большего без учета знака и степени
void sub_simple(long_decimal long_dec1, long_decimal long_dec2, long_decimal *long_dec_res) {
    short bit_tmp = 0;
    for (short i = 0; i < 224; i++) {
        short bit1 = get_bit(long_dec1, i);
        short bit2 = get_bit(long_dec2, i);
        if (!bit1 && !bit2) {
            if (bit_tmp) {
                set_bit(long_dec_res, i);
            } else {
                clean_bit(long_dec_res, i);
            }
        } else if (bit1 && !bit2) {
            if (bit_tmp) {
                clean_bit(long_dec_res, i);
                bit_tmp = 0;
            } else {
                set_bit(long_dec_res, i);
            }
        } else if (bit1 && bit2) {
            if (bit_tmp) {
                set_bit(long_dec_res, i);
            } else {
                clean_bit(long_dec_res, i);
            }
        } else if (!bit1 && bit2) {
            if (bit_tmp) {
                clean_bit(long_dec_res, i);
            } else {
                set_bit(long_dec_res, i);
                bit_tmp = 1;
            }
        }
    }
}

// вычитание для long_decimal
void sub_long_decimal(long_decimal long_dec1, long_decimal long_dec2, long_decimal *long_dec3) {
    clean_struct_long_dec(long_dec3);
    s21_decimal decimal1 = convert_from_long_to_decimal(long_dec1);
    s21_decimal decimal2 = convert_from_long_to_decimal(long_dec2);
    long_dec3->scale = normalization(&long_dec1, &long_dec2);
    if (s21_is_less(decimal1, decimal2)) {
        if (!long_dec1.sign && !long_dec2.sign) {
            sub_simple(long_dec1, long_dec2, long_dec3);
            s21_decimal one = {{1, 0, 0, 0}};
            long_decimal long_dec4 = convert_in_long_dec(one);
            sub_simple(*long_dec3, long_dec4, long_dec3);
            inversion(long_dec3);
            long_dec3->sign = 1;
        } else if (long_dec1.sign && long_dec2.sign) {
            sub_simple(long_dec1, long_dec2, long_dec3);
            long_dec3->sign = 1;
        } else if (long_dec1.sign && !long_dec2.sign) {
            sum(long_dec1, long_dec2, long_dec3);
            long_dec3->sign = 1;
        }
    } else {
        if (!long_dec1.sign && !long_dec2.sign) {
            sub_simple(long_dec1, long_dec2, long_dec3);
        } else if (long_dec1.sign && long_dec2.sign) {
            if (compare_digit_long_decimal(long_dec1, long_dec2)) {
                sub_simple(long_dec1, long_dec2, long_dec3);
                long_dec3->sign = long_dec1.sign;
            } else {
                sub_simple(long_dec2, long_dec1, long_dec3);
                long_dec3->sign = 0;
            }
        } else if (!long_dec1.sign && long_dec2.sign) {
            sum(long_dec1, long_dec2, long_dec3);
        }
    }
    long_dec3->scale = check_scale(long_dec1, long_dec2);
}

// АРИФМЕТИКА
void set_sign_result(long_decimal long_dec1, long_decimal long_dec2, long_decimal *result) {
    if (long_dec1.sign && long_dec2.sign) {
        result->sign = 0;
    } else if ((long_dec1.sign || long_dec2.sign) && !(equal_nul(long_dec1, long_dec2))) {
        result->sign = 1;
    }
}

// проверяет и устанавливает степень
int check_scale(long_decimal long_dec1, long_decimal long_dec2) {
    int scale_res = 0;
    if (equal_nul(long_dec1, long_dec2) && !long_dec1.scale && !long_dec2.scale) {
        scale_res = 0;
    } else if (equal_nul(long_dec1, long_dec2)) {
        scale_res = 6;
    } else {
        if (long_dec1.scale > long_dec2.scale) {
            scale_res = long_dec1.scale;
        } else if (long_dec1.scale < long_dec2.scale) {
            scale_res = long_dec2.scale;
        } else {
            scale_res = long_dec2.scale;
        }
    }
    return scale_res;
}

// выбирает большее число для вычитания
void choosing_large_divisible(long_decimal long_dec1, long_decimal long_dec2, long_decimal *long_dec3) {
    if (compare_digit_long_decimal(long_dec1, long_dec2)) {
        sub_simple(long_dec1, long_dec2, long_dec3);
        long_dec3->sign = long_dec1.sign;
    } else {
        sub_simple(long_dec2, long_dec1, long_dec3);
        long_dec3->sign = long_dec2.sign;
    }
}

//  вычисление целой части при делении
void calculat_integer(long_decimal long_dec1, long_decimal long_dec2, long_decimal *result, short flag_mod) {
    long_decimal one_long = {{1, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal long_nul = {{0, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal long_dec_temp = long_dec2;
    long_decimal long_dec_up_temp = long_dec2;
    long_decimal long_dec_result = one_long;
    long_decimal long_dec_last_res = *result;
    clean_struct_long_dec(&long_dec_result);
    clean_struct_long_dec(&long_dec_last_res);
    long_dec_temp.sign = 0;
    long_dec_up_temp.sign = 0;
    while (compare_digit_long_decimal(long_dec1, long_nul)) {
        long_dec_temp = long_dec2;
        long_dec_up_temp = long_dec2;
        long_dec_result = one_long;
        long_dec_temp.sign = 0;
        long_dec_up_temp.sign = 0;
        while (!compare_digit_long_decimal(long_dec_temp, long_dec1)) {
            ten_up(&long_dec_temp);
            if (!compare_digit_long_decimal(long_dec_temp, long_dec1)) {
                long_dec_up_temp = long_dec_temp;
                ten_up(&long_dec_result);
            }
        }
        while (compare_digit_long_decimal_great_or_equal(long_dec1, long_dec_up_temp)) {
            sub_simple(long_dec1, long_dec_up_temp, &long_dec1);
            sum(long_dec_last_res, long_dec_result, &long_dec_last_res);
        }
        if (!compare_digit_long_decimal(long_dec1, long_dec2)) {
            break;
        }
    }
    if (flag_mod) {
        *result = long_dec1;
    } else {
        *result = long_dec_last_res;
    }
}

// Возвращает остаток от деления на 10
int long_decimal_mod_10(long_decimal src) {
    int dst = 0;
    for (int i = 191; i >= 0; i--) {
        dst = dst << 1;
        dst |= get_bit(src, i);
        if (dst >= 10) dst -= 10;
    }
    return dst;
}

// остаток от деления
short calculating_mod_from_int(long_decimal long_dec1, long_decimal long_dec2,
long_decimal *result, short err) {
    long_decimal long_nul = {{0, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal long_res = {{0, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal long_dec1_copy = long_dec1;
    s21_decimal nul = {{0, 0, 0, 0}};
    clean_struct_long_dec(result);
    if (compare_digit_long_decimal(long_dec1, long_dec2) && !equal_long_decimal(long_dec2, long_nul)) {
        calculat_integer(long_dec1, long_dec2, result, 1);
    } else if (!greater_or_equal_long_decimal(long_dec1, long_dec2) &&
                !equal_long_decimal(long_dec2, long_nul)) {
        *result = long_dec1_copy;
    } else if (!equal_long_decimal(long_dec1, long_dec2)) {
        long_dec1_copy = convert_in_long_dec(nul);
        *result = long_dec1_copy;
    } else {
        *result = long_res;
        err = s21_NAN;
    }
    set_sign_result(long_dec1, long_dec2, result);
    return err;
}

// умножение для long_decimal
void mul_long_decimal(long_decimal long_dec1, long_decimal long_dec2, long_decimal *long_dec3) {
    clean_struct_long_dec(long_dec3);
    short start = 0;
    short shift_count = 0;
    long_dec3->scale = long_dec1.scale + long_dec2.scale;
    for (short i = 0; i < 224; i++) {
        long_decimal long_dec1_copy = long_dec1;
        short bit = get_bit(long_dec2, i);
        if (bit && !start) {
            sum(*long_dec3, long_dec1, long_dec3);
            start = 1;
            shift_count++;
        } else if (!bit && !start) {
            start = 1;
            shift_count++;
        } else if (bit) {
            for (int j = 0; j < shift_count; j++) {
                move_left(&long_dec1_copy);
            }
            shift_count++;
            sum(*long_dec3, long_dec1_copy, long_dec3);
        } else {
            shift_count++;
        }
    }
    set_sign_result(long_dec1, long_dec2, long_dec3);
}

// вычисляем результат деления на целое число
void calculat_result_div_int(long_decimal long_dec1, long_decimal long_dec2,
long_decimal long_dec1_integer, long_decimal *long_temp) {
    long_truncate(long_dec1, &long_dec1_integer);
    short scale_value_2 = long_dec2.scale;
    while (scale_value_2) {
        ten_up(&long_dec1_integer);
        scale_value_2--;
    }
    calculat_integer(long_dec1_integer, long_dec2, long_temp, 0);
}

// удаляем нули из результа в мод
void removing_zeros_from_mod_result(long_decimal *long_dec_res) {
    int scale_tmp = long_dec_res->scale;
    int sign_tmp_res = long_dec_res->sign;
    while (scale_tmp && !long_decimal_mod_10(*long_dec_res)) {
        long_decimal long_ten = {{10, 0, 0, 0, 0, 0, 0}, 0, 0};
        calculat_integer(*long_dec_res, long_ten, long_dec_res, 0);
        scale_tmp--;
    }
    long_dec_res->scale = scale_tmp;
    long_dec_res->sign = sign_tmp_res;
}

// вычисление дробной части
void calculat_fraction(long_decimal *long_dec1, long_decimal long_dec2, long_decimal *long_dec_last_res) {
    long_decimal one_long = {{1, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal nul = {{0, 0, 0, 0, 0, 0, 0}, 0, 0};
    int j = 40;
    while (compare_digit_long_decimal(*long_dec1, nul) && j) {
        ten_up(long_dec1);
        long_dec_last_res->scale += 1;
        if (compare_digit_long_decimal_great_or_equal(*long_dec1, long_dec2)) {
            ten_up(long_dec_last_res);
        }
        while (compare_digit_long_decimal_great_or_equal(*long_dec1, long_dec2)) {
            sub_simple(*long_dec1, long_dec2, long_dec1);
            sum(*long_dec_last_res, one_long, long_dec_last_res);
        }
        j--;
    }
}

// целочислено деление
short div_int(long_decimal long_dec1, long_decimal long_dec2, long_decimal *result) {
    short err = OK;
    long_decimal one_long = {{1, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal nul = {{0, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal long_dec_temp = long_dec2;
    long_decimal long_dec_up_temp = long_dec2;
    long_decimal long_dec_result = one_long;
    long_decimal long_dec_last_res = *result;
    clean_struct_long_dec(&long_dec_result);
    clean_struct_long_dec(&long_dec_last_res);
    long_dec_last_res.scale = long_dec1.scale - long_dec2.scale;
    long_dec_temp.sign = 0;
    long_dec_up_temp.sign = 0;
    long_dec1.sign = 0;
    while (compare_digit_long_decimal(long_dec1, nul)) {
        long_dec_temp = long_dec2;
        long_dec_temp.sign = 0;
        long_dec_up_temp = long_dec2;
        long_dec_up_temp.sign = 0;
        long_dec_result = one_long;
        do {
            ten_up(&long_dec_temp);
            if (!compare_digit_long_decimal(long_dec_temp, long_dec1)) {
                long_dec_up_temp = long_dec_temp;
                ten_up(&long_dec_result);
            }
        } while (!compare_digit_long_decimal(long_dec_temp, long_dec1));
        while (compare_digit_long_decimal_great_or_equal(long_dec1, long_dec_up_temp)) {
            sub_simple(long_dec1, long_dec_up_temp, &long_dec1);
            sum(long_dec_last_res, long_dec_result, &long_dec_last_res);
        }
        // вычисляется дробная часть
        if (!compare_digit_long_decimal(long_dec1, long_dec2)) {
            calculat_fraction(&long_dec1, long_dec2, &long_dec_last_res);
            break;
        }
    }
    *result = long_dec_last_res;
    return err;
}

// проверка long_decimal на вместимость в decimal
short check_long_decimal(long_decimal long_dec) {
    long_decimal max_decimal_for_inf = {{-1, -1, -1, 0, 0, 0, 0}, 0, 0};
    short err;
    if (compare_digit_long_decimal(long_dec, max_decimal_for_inf)) {
        err = 0;
    } else {
        if (long_dec.scale > 28) {
            err = 0;
        } else {
            err = 1;
        }
    }
    return err;
}

// банковское округление
void bank_rounding(long_decimal value, long_decimal *result) {
    long_decimal long_ten = {{10, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal long_one = {{1, 0, 0, 0, 0, 0, 0}, 0, 0};
    short tmp_sign = value.sign;
    long_decimal long_value_tmp = value;
    int mod_10_value_last = long_decimal_mod_10(long_value_tmp);
    int mod_10_value = long_decimal_mod_10(long_value_tmp);
    int scale_score = long_value_tmp.scale;
    int check_cycle = 0;
    while (!check_long_decimal(long_value_tmp) && scale_score) {
        calculat_integer(long_value_tmp, long_ten, &long_value_tmp, 0);
        mod_10_value_last = mod_10_value;
        mod_10_value = long_decimal_mod_10(long_value_tmp);
        scale_score--;
        check_cycle = 1;
    }
    long_value_tmp.scale = scale_score;
    if (check_long_decimal(long_value_tmp) && check_cycle) {
        if (mod_10_value_last == 5) {
            if (!(mod_10_value % 2)) {
                *result = long_value_tmp;
            } else {
                *result = long_value_tmp;
                sum(*result, long_one, result);
            }
        } else {
            if (mod_10_value_last >= 0 && mod_10_value_last <= 4) {
                *result = long_value_tmp;
            } else if (mod_10_value_last >= 5 && mod_10_value_last <= 9) {
                *result = long_value_tmp;
                sum(*result, long_one, result);
            }
        }
    } else if (equal_long_decimal_nul(long_value_tmp)) {
        *result = long_value_tmp;
        result->scale = 0;
    }

    result->sign = tmp_sign;
    if (equal_long_decimal_nul(*result))
        result->scale = 0;
}

// возвращает целые цифры указанного long_decimal числа
void long_truncate(long_decimal value, long_decimal *result) {
    long_decimal ten = {{10, 0, 0, 0, 0, 0, 0}, 0, 0};
    long_decimal temp = value;
    short score_scale = value.scale;
    while (score_scale) {
        calculat_integer(temp, ten, &temp, 0);
        score_scale--;
    }
    *result = temp;
    result->sign = value.sign;
}

void clean_dec(s21_decimal *dec) {
    for (int i = 0; i < 4; i++) {
        dec->bits[i] = 0;
    }
}

int found_elder_bit(s21_decimal dec) {
    int bit = 0;
    long_decimal tmp = convert_in_long_dec(dec);
    for (int i = 95; i >= 0; i--) {
        if (get_bit(tmp, i)) {
            bit = i;
            break;
        }
    }
    return bit;
}
