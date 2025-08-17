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

        decimal_t num = {integer places, decimal places, digits}

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

typedef struct decimal_t
{
    unsigned char integer_places;
    unsigned char decimal_places;
    int value;
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
// convert a decimal number to string
void decimal_to_string(decimal_t number, char** string_out, size_t string_out_length);

#endif

//------------------------------------------------------------------------------
//
// Implementation
//
//------------------------------------------------------------------------------
#ifdef CHRISSLY_DECIMAL_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_add(decimal_t a, decimal_t b)
{
    decimal_t r;

    int i, d, scale = 1;
    if (a.decimal_places > b.decimal_places)
    {
        r.decimal_places = a.decimal_places;
        d = a.decimal_places - b.decimal_places;
        for (i = 0; i < d; ++i) scale *= 10;
        r.value = b.value * scale + a.value;
    }
    else
    {
        r.decimal_places = b.decimal_places;
        d = b.decimal_places - a.decimal_places;
        for (i = 0; i < d; ++i) scale *= 10;
        r.value = a.value * scale + b.value;
    }

    int x = r.value, precision = 1;
    while (x /= 10) ++precision;
    r.integer_places = precision > r.decimal_places ? precision - r.decimal_places : 0;

    return r;
}

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_subtract(decimal_t a, decimal_t b)
{
    decimal_t s = b;
    s.value *= -1;
    return decimal_add(a, s);
}

#define MAX_NUM_DIGITS 10

//------------------------------------------------------------------------------
/**
*/
decimal_t
decimal_from_string(char const* number)
{
    decimal_t r = {0};

    char buffer[MAX_NUM_DIGITS] = {'\0'};
    char c = '\0';
    int count = 0, i = 0, sign = 1;
    bool period_found = false;
    while ((c = *number++) && i < MAX_NUM_DIGITS)
    {
        if (c == '-') sign = -1;
        if (c == '.')
        {
            r.integer_places = count;
            count = 0;
            period_found = true;
        }
        if (c >= '0' && c <= '9')
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
        r.decimal_places = 0;
        r.integer_places = count;
    }
    r.value = atoi(buffer) * sign;

    return r;
}

#endif