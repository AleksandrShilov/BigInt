#include "s21_decimal.h"
#include "secondary_functions.h"
// ОПЕРАТОРЫ СРАВНЕНИЯ
// логическое равно ==
int s21_is_equal(s21_decimal decimal1, s21_decimal decimal2) {
    long_decimal long_dec1 = convert_in_long_dec(decimal1);
    long_decimal long_dec2 = convert_in_long_dec(decimal2);
    return equal_long_decimal(long_dec1, long_dec2);
}

// логическое не равно !=
int s21_is_not_equal(s21_decimal decimal1, s21_decimal decimal2) {
    long_decimal long_dec1 = convert_in_long_dec(decimal1);
    long_decimal long_dec2 = convert_in_long_dec(decimal2);
    short result = s21_FALSE;
    normalization(&long_dec1, &long_dec2);
    if (long_dec1.sign == long_dec2.sign) {
        for (short i = 191; i >= 0; i--) {
            if ((get_bit(long_dec1, i)) != (get_bit(long_dec2, i))) {
                result = s21_TRUE;
                break;
            }
        }
    } else {
        if (!(equal_nul(long_dec1, long_dec2))) {
            result = s21_TRUE;
        }
    }
    return result;
}

// логическое больше >
int s21_is_greater(s21_decimal decimal1, s21_decimal decimal2) {
    long_decimal long_dec1 = convert_in_long_dec(decimal1);
    long_decimal long_dec2 = convert_in_long_dec(decimal2);
    int err = 0;
    if (!(equal_nul(long_dec1, long_dec2))) {
        err = greater_for_long_dec(long_dec1, long_dec2);
    }
    return err;
}

// логическое больше или равно >=
int s21_is_greater_or_equal(s21_decimal decimal1, s21_decimal decimal2) {
    long_decimal long_dec1 = convert_in_long_dec(decimal1);
    long_decimal long_dec2 = convert_in_long_dec(decimal2);
    return greater_or_equal_long_decimal(long_dec1, long_dec2);
}

// логическое меньше <
int s21_is_less(s21_decimal decimal1, s21_decimal decimal2) {
    long_decimal long_dec1 = convert_in_long_dec(decimal1);
    long_decimal long_dec2 = convert_in_long_dec(decimal2);
    int err = 0;
    if (!(equal_nul(long_dec1, long_dec2))) {
        err = less_for_long_dec(long_dec1, long_dec2);
    }
    return err;
}

// логическое меньше или равно <=
int s21_is_less_or_equal(s21_decimal decimal1, s21_decimal decimal2) {
    short result = s21_FALSE;
    short greater = s21_is_less(decimal1, decimal2);
    short equal = s21_is_equal(decimal1, decimal2);
    if (greater || equal)
        result = s21_TRUE;
    return result;
}

// АРИФМЕТИКА
// полное вычетание
int s21_sub(s21_decimal decimal1, s21_decimal decimal2, s21_decimal *result) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    long_decimal long_dec1 = convert_in_long_dec(decimal1);
    long_decimal long_dec2 = convert_in_long_dec(decimal2);
    long_decimal long_dec3 = convert_in_long_dec(*result);
    short err = OK;
    clean_struct_long_dec(&long_dec3);
    sub_long_decimal(long_dec1, long_dec2, &long_dec3);
    bank_rounding(long_dec3, &long_dec3);
    err = error(long_dec3);
    if (err) {
        clean_struct_long_dec(&long_dec3);
    }
    *result = convert_from_long_to_decimal(long_dec3);
    return err;
}

// полное сложение
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    short err = OK;
    long_decimal long_dec1 = convert_in_long_dec(value_1);
    long_decimal long_dec2 = convert_in_long_dec(value_2);
    long_decimal long_dec3 = convert_in_long_dec(*result);
    clean_struct_long_dec(&long_dec3);
    long_dec3.scale = normalization(&long_dec1, &long_dec2);
    if (!long_dec1.sign && !long_dec2.sign && !equal_nul(long_dec1, long_dec2)) {
        sum(long_dec1, long_dec2, &long_dec3);
    } else if (!long_dec1.sign && long_dec2.sign && !equal_nul(long_dec1, long_dec2)) {
        choosing_large_divisible(long_dec1, long_dec2, &long_dec3);
    } else if (long_dec1.sign && !long_dec2.sign && !equal_nul(long_dec1, long_dec2)) {
        choosing_large_divisible(long_dec1, long_dec2, &long_dec3);
    } else if (long_dec1.sign && long_dec2.sign && !equal_nul(long_dec1, long_dec2)) {
        sum(long_dec1, long_dec2, &long_dec3);
        long_dec3.sign = 1;
    }
    long_dec3.scale = check_scale(long_dec1, long_dec2);
    bank_rounding(long_dec3, &long_dec3);
    err = error(long_dec3);
    if (err) {
        clean_struct_long_dec(&long_dec3);
    } else if (equal_long_decimal_nul(long_dec3) && !equal_nul(long_dec1, long_dec2)) {
        clean_struct_long_dec(&long_dec3);
    }
    *result = convert_from_long_to_decimal(long_dec3);
    return err;
}

// полное умножение
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    long_decimal long_dec1 = convert_in_long_dec(value_1);
    long_decimal long_dec2 = convert_in_long_dec(value_2);
    long_decimal long_dec3 = convert_in_long_dec(*result);
    clean_struct_long_dec(&long_dec3);
    short err = OK;
    mul_long_decimal(long_dec1, long_dec2, &long_dec3);
    set_sign_result(long_dec1, long_dec2, &long_dec3);
    bank_rounding(long_dec3, &long_dec3);
    err = error(long_dec3);
    if (err) {
        clean_struct_long_dec(&long_dec3);
    }
    *result = convert_from_long_to_decimal(long_dec3);
    return err;
}

// остаток от деления
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    long_decimal long_dec1_integer = convert_in_long_dec(value_1);
    long_decimal long_dec_res = convert_in_long_dec(*result);
    long_decimal long_dec1 = convert_in_long_dec(value_1);
    long_decimal long_dec2 = convert_in_long_dec(value_2);
    long_decimal long_temp = convert_in_long_dec(value_2);
    short sign_tmp_val_1 = 0;
    short err = OK;
    clean_struct_long_dec(&long_temp);
    if (get_sign(value_1)) {
        sign_tmp_val_1 = 1;
        long_dec1.sign = 0;
        clean_sign(&value_1);
    }
    if (get_sign(value_2)) {
        long_dec2.sign = 0;
        clean_sign(&value_2);
    }
    if (!get_scale(value_1) && !get_scale(value_2)) {
        err = calculating_mod_from_int(long_dec1, long_dec2, &long_dec_res, err);
    } else if (value_2.bits[0] == 1) {
        long_dec_res = convert_in_long_dec(*result);
    } else {
        calculat_result_div_int(long_dec1, long_dec2, long_dec1_integer, &long_temp);
        mul_long_decimal(long_temp, long_dec2, &long_temp);
        sub_long_decimal(long_dec1, long_temp, &long_dec_res);
        removing_zeros_from_mod_result(&long_dec_res);
    }
    if (sign_tmp_val_1) {
        long_dec_res.sign = 1;
    }
    *result = convert_from_long_to_decimal(long_dec_res);
    return err;
}

// деление
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    long_decimal long_dec_res = convert_in_long_dec(*result);
    long_decimal long_dec1 = convert_in_long_dec(value_1);
    long_decimal long_dec2 = convert_in_long_dec(value_2);
    s21_decimal nul = {{0, 0, 0, 0}};
    short err = OK;
    clean_struct_long_dec(&long_dec_res);
    if (s21_is_not_equal(value_2, nul)) {
        div_int(long_dec1, long_dec2, &long_dec_res);
        bank_rounding(long_dec_res, &long_dec_res);
        set_sign_result(long_dec1, long_dec2, &long_dec_res);
        err = error(long_dec_res);
        if (err) {
            clean_struct_long_dec(&long_dec_res);
        }
    } else {
        err = s21_NAN;
        if (equal_nul(long_dec1, long_dec2) && long_dec1.scale && long_dec2.scale) {
            long_dec_res.scale = 6;
        }
    }
    *result = convert_from_long_to_decimal(long_dec_res);
    return err;
}

// ДРУГИЕ ФУНКЦИИ
// Округляет указанное Decimal число до ближайшего целого числа в сторону отрицательной бесконечности.
int s21_floor(s21_decimal value, s21_decimal *result) {
    s21_decimal one = {{1, 0, 0, 0}};
    s21_truncate(value, result);
    if (get_scale(value)) {
        if (get_sign(value)) {
            set_sign(result);
            s21_sub(*result, one, result);
        }
    } else {
        *result = value;
    }
    return 0;
}

// Округляет Decimal до ближайшего целого числа
int s21_round(s21_decimal value, s21_decimal *result) {
    long_decimal long_res = convert_in_long_dec(value);
    long_decimal ten = {{10, 0, 0, 0, 0, 0, 0}, 0, 0};
    s21_decimal one = {{1, 0, 0, 0}};
    s21_truncate(value, result);
    if (long_res.scale) {
        while (long_res.scale > 1) {
            calculat_integer(long_res, ten, &long_res, 0);
            long_res.scale--;
        }
    }
    if (get_scale(value)) {
        if (long_decimal_mod_10(long_res) >= 5) {
            if (get_sign(value)) {
                s21_sub(*result, one, result);
            } else {
                s21_add(*result, one, result);
            }
        }
    } else {
        *result = value;
    }
    if (get_sign(value)) {
        set_sign(result);
    }
    return 0;
}

//  Возвращает целые цифры указанного decimal числа
int s21_truncate(s21_decimal value, s21_decimal *result) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    long_decimal long_dec_res = convert_in_long_dec(*result);
    long_decimal long_dec1 = convert_in_long_dec(value);
    clean_struct_long_dec(&long_dec_res);
    long_truncate(long_dec1, &long_dec_res);
    *result = convert_from_long_to_decimal(long_dec_res);
    return 0;
}

//  Возвращает результат умножения указанного Decimal на -1.
int s21_negate(s21_decimal value, s21_decimal *result) {
    short err = 0;
    *result = value;
    if (get_sign(value)) {
        clean_sign(result);
    } else {
        set_sign(result);
    }
    return err;
}

// ПРЕОБРАЗОВАТЕЛИ
/* из float в decimal */
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
    clean_dec(dst);
    int ret = 0;
    if (src != src || src == INFINITY || src == -INFINITY) {
        ret = 1;
    } else {
        float val = src;
        float int_part;
        short count = 0;
        float fraction = modff(val, &int_part);
        if (fraction != 0) {
            for (; ((long int)fraction - fraction) && count < 29; count++) {
                val *= 10;
                fraction *= 10;
            }
        }
        if (count >= 29) {
            ret = 1;
        } else {
            dst->bits[3] = count << 16;
            if (src < 0) set_sign(dst);
            unsigned int float_to_bit = *(unsigned int*)(void*)&val;
            int exp_float = (float_to_bit & 0x7F800000)>>23;
            exp_float -= 127;
            if (exp_float > 95) {
                ret = 1;
            } else {
                int bit = 0;
                int N_decimal = exp_float;
                short tmp = N_decimal % 32;
                set_bit_decimal(&bit, tmp);
                dst->bits[N_decimal / 32] = (dst->bits[N_decimal / 32] | bit);
                N_decimal--;
                int n_float = 22;
                for (int i = N_decimal; i >= 0 && n_float >= 0 ; i--, n_float --) {
                    int float_bit = 1 << n_float;
                    if (!!(float_to_bit & float_bit)) {
                        bit = 1 << (i % 32);
                        dst->bits[i / 32] = (dst->bits[i / 32] | bit);
                    }
                }
                bit = 0;
            }
        }
    }
    return ret;
}

/* из decimal в float */
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    int ret = 0;
    int bit = 0;
    if (dst) {
        double val = 0;
        for (int i = 0; i < 96; i++) {
            short tmp = i % 32;
            set_bit_decimal(&bit, tmp);
            if (src.bits[i / 32] & bit) {
                val += pow(2, i);
            }
            bit = 0;
        }
        int scale = get_scale(src);
        while (scale != 0) {
            val/=10;
            scale--;
        }
        if (get_sign(src)) {
            val *=-1;
        }
        *dst = val;
    } else {
        ret = 1;
    }
    return ret;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
    int err = OK;
    clean_dec(dst);
    if (src >= 0) {
        dst->bits[0] = src;
    } else {
        set_sign(dst);
        if (src != INT_MIN) {
            dst->bits[0] = src * (-1);
        } else {
            dst->bits[0] = (dst->bits[0] | -2147483648);
        }
    }
    return err;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    int err = OK;
    s21_decimal value;
    if (get_scale(src)) {
        s21_truncate(src, &value);
    } else {
        value = src;
    }
    int idx = found_elder_bit(value);
    if (idx < 31) {
        if (get_sign(src)) {
            *dst = (-1)*value.bits[0];
        } else {
            *dst = value.bits[0];
        }
    } else {
        if (idx == 31 && ((unsigned)value.bits[0] == 0x80000000) && get_sign(src)) {
            *dst = INT_MIN;
        } else {
            err = 1;
        }
    }
    return err;
}
