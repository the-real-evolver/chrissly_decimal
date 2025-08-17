//------------------------------------------------------------------------------
//  main.c
//  (C) 2025 Christian Bleicher
//------------------------------------------------------------------------------
#include <stdio.h>
#define CHRISSLY_DECIMAL_IMPLEMENTATION
#include "chrissly_decimal.h"

//------------------------------------------------------------------------------
/**
*/
int
main(void)
{
    decimal_t a = {3, 2, 12345}; // 123.45
    decimal_t b = {1, 3, 6789};  // 6.789
    decimal_t r = decimal_add(a, b);
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    a = (decimal_t){4, 0, 1001}; // 1001
    b = (decimal_t){3, 0, 501};  // 501
    r = decimal_add(a, b);
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    a = (decimal_t){0, 1, 1};   // 0.1
    b = (decimal_t){3, 0, 501}; // 501
    r = decimal_add(a, b);
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    a = (decimal_t){3, 1, 1001}; // 100.1
    b = (decimal_t){2, 1, -501}; // -50.1
    r = decimal_add(a, b);
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    a = (decimal_t){3, 1, -1001}; // -100.1
    b = (decimal_t){2, 1, 501};   // 50.1
    r = decimal_add(a, b);
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    a = (decimal_t){0, 1, -1};        // -0.1
    b = decimal_from_string("-50.0"); // -50
    r = decimal_add(a, b);
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    a = (decimal_t){0, 0, 0};          // 0
    b = decimal_from_string("-0.000"); // -0.0
    r = decimal_add(a, b);
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    r = decimal_add(decimal_from_string("0.1"), decimal_add(decimal_from_string("0.2"), decimal_from_string("0.3")));
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    return 0;
}