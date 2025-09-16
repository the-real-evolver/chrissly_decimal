//------------------------------------------------------------------------------
/**
    chrissly_decimal.h

    A single-header file library that implements decimal floating point (DFP)
    arithmetic. It can be used when values are expected to be base-10 and have
    to be exact. For instance where it would be expected that an expression like
    0.1 * 10 would equal 1.0 without rounding errors.

    Add this line:
        #define CHRISSLY_DECIMAL_IMPLEMENTATION
    before you include this file in *one* C or C++ file to create the implementation.

    Usage
    =====

    Numbers are represented as follows:

        decimal_t num = {integer places, decimal places, significand}

    For instance:

        decimal_t a = {3, 2, 12345} -> 123.45

    Numbers with a magnitude < 1 have zero integer places:

        decimal_t a = {0, 3, 123} -> 0.123

    Integer numbers have zero decimal places:

        decimal_t a = {5, 0, -12345} -> -12345

    Create a number from a string:

        decimal_t a = decimal_from_string("123.45");

    Limits
    ======

    Since the underlying data type is a signed 32bit integer the numbers range
    from  -2147483648.0 to 2147483647.

    (C) 2025 Christian Bleicher
*/
//------------------------------------------------------------------------------
#ifndef INCLUDE_CHRISSLY_DECIMAL_H
#define INCLUDE_CHRISSLY_DECIMAL_H
#include <stdint.h>

typedef struct decimal_t
{
    unsigned char integer_places;
    unsigned char decimal_places;
    int32_t significand;
} decimal_t;

// adds two decimal numbers
decimal_t decimal_add(decimal_t a, decimal_t b);
// subtracts two decimal numbers
decimal_t decimal_subtract(decimal_t a, decimal_t b);
// multiplies two decimal numbers
decimal_t decimal_multiply(decimal_t a, decimal_t b);
// divides a decimal number by another
decimal_t decimal_divide(decimal_t dividend, decimal_t divisor);

// create a decimal number from string (format like "-123.54")
decimal_t decimal_from_string(char const* number);
// convert a decimal number to string (if the size of string_out is not sufficient, it remains unmodified)
void decimal_to_string(decimal_t number, char string_out[], size_t string_out_length);

// identifies wether the given number has an undefined or non-representable value
int decimal_isnan(decimal_t num);
// truncates a decimal number at the specified number of decimal places
decimal_t decimal_truncate(decimal_t num, unsigned char decimals);

#endif

//------------------------------------------------------------------------------
//
// Implementation
//
//------------------------------------------------------------------------------
#ifdef CHRISSLY_DECIMAL_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NUM_DIGITS_BASE10 10U
#define MAX_STR_LENGTH_BASE10 12U

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_add(decimal_t a, decimal_t b)
{
    decimal_t r = {0};

    unsigned int i, d; int32_t scale = 1;
    if (a.decimal_places > b.decimal_places)
    {
        r.decimal_places = a.decimal_places;
        d = a.decimal_places - b.decimal_places;
        for (i = 0U; i < d; ++i) scale *= 10;
        r.significand = b.significand * scale + a.significand;
    }
    else
    {
        r.decimal_places = b.decimal_places;
        d = b.decimal_places - a.decimal_places;
        for (i = 0U; i < d; ++i) scale *= 10;
        r.significand = a.significand * scale + b.significand;
    }

    int32_t x = r.significand;
    unsigned char precision = 1U;
    while (x /= 10) ++precision;
    r.integer_places = precision > r.decimal_places ? precision - r.decimal_places : 0U;

    return r;
}

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_subtract(decimal_t a, decimal_t b)
{
    decimal_t s = b;
    s.significand *= -1;
    return decimal_add(a, s);
}

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_multiply(decimal_t a, decimal_t b)
{
    decimal_t r = {0};

    if ((a.significand == 0) || (b.significand == 0)) return r;

    int64_t significand = (int64_t)a.significand * (int64_t)b.significand;
    r.decimal_places = a.decimal_places + b.decimal_places;

    int64_t x = significand;
    unsigned char precision = 1U;
    while (x /= 10) ++precision;
    r.integer_places = precision > r.decimal_places ? precision - r.decimal_places : 0U;

    if (precision >= 9U)
    {
        unsigned char decimal_places = 9U - r.integer_places;
        int64_t f = 1;
        unsigned int i;
        for (i = 0U; i < (unsigned char)(r.decimal_places - decimal_places); ++i) f *= 10;
        r.significand = (int32_t)(significand / f);
        r.decimal_places = decimal_places;
    }
    else
    {
        r.significand = (int32_t)significand;
    }

    return r;
}

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_divide(decimal_t dividend, decimal_t divisor)
{
    if (dividend.significand == 0 || divisor.significand == 0) { decimal_t r = {0}; return r; }

    decimal_t N = {dividend.integer_places, dividend.decimal_places, dividend.significand < 0 ? -dividend.significand : dividend.significand};
    decimal_t D = {divisor.integer_places, divisor.decimal_places, divisor.significand < 0 ? -divisor.significand : divisor.significand};

    decimal_t F = {0, divisor.integer_places, 1};

    decimal_t two = {1U, 0U, 2};

    unsigned int i;
    for (i = 0U; i < 8U; ++i)
    {
        N = decimal_multiply(F, N);
        D = decimal_multiply(F, D);
        F = decimal_subtract(two, D);
    }

    if ((dividend.significand < 0 && divisor.significand > 0) || ((dividend.significand >= 0 && divisor.significand < 0))) N.significand *= -1;

    return N;
}

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_from_string(char const* number)
{
    decimal_t r = {0};

    char buffer[MAX_NUM_DIGITS_BASE10] = {'\0'};
    char c = '\0';
    unsigned char count = 0U;
    char i = 0, sign = 1;
    bool period_found = false;
    while ((c = *number++) && i < MAX_NUM_DIGITS_BASE10)
    {
        if (c == '-') sign = -1;
        else if (c == '.')
        {
            r.integer_places = count;
            count = 0U;
            period_found = true;
        }
        else if (c >= '0' && c <= '9')
        {
            buffer[i++] = c;
            ++count;
        }
    }
    if (period_found)
    {
        r.decimal_places = count;
    }
    else
    {
        r.decimal_places = 0U;
        r.integer_places = count;
    }
    r.significand = atoi(buffer) * sign;

    return r;
}

//------------------------------------------------------------------------------
/**
*/
void
decimal_to_string(decimal_t number, char string_out[], size_t string_out_length)
{
    char buffer[MAX_STR_LENGTH_BASE10] = {'\0'};
    sprintf_s(buffer, sizeof(buffer), "%d", number.significand);
    unsigned char sign = number.significand < 0 ? 1U : 0U;
    unsigned char length_number_string = number.integer_places + number.decimal_places + sign + 1U;

    if (length_number_string >= string_out_length) return;

    memset(string_out, 0, string_out_length);

    if (number.significand == 0) {string_out[0U] = '0'; return;}

    unsigned char i;
    for (i = 0U; i < number.integer_places + sign; ++i) string_out[i] = buffer[i];
    if (number.decimal_places == 0U) return;

    string_out[i++] = '.';

    size_t num_digits_significant = strlen(buffer) - sign;
    if (num_digits_significant < number.decimal_places)
    {
        unsigned char c;
        for (c = 0U; c < number.decimal_places - num_digits_significant; ++c) string_out[i++] = '0';
        for (c = 0U; c < num_digits_significant; ++c) string_out[i++] = buffer[c + sign];
    }
    else
    {
        for (; i < length_number_string; ++i) string_out[i] = buffer[i - 1U];
    }
}

//------------------------------------------------------------------------------
/**
*/
int
decimal_isnan(decimal_t num)
{
    int32_t x = num.significand;
    unsigned char precision = 1U;
    while (x /= 10) ++precision;

    if ((num.integer_places > 0U) && (precision != num.decimal_places + num.integer_places)) return 1;

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_truncate(decimal_t num, unsigned char decimals)
{
    if (num.decimal_places <= decimals) return num;
    decimal_t r = {0};

    int32_t f = 1;
    unsigned int i;
    for (i = 0U; i < (unsigned char)(num.decimal_places - decimals); ++i) f *= 10;
    r.significand = num.significand / f;

    if (r.significand == 0) return r;
    r.integer_places = num.integer_places;
    r.decimal_places = decimals;

    return r;
}

#endif