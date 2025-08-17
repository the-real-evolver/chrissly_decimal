# Chrissly Decimal (WIP)
   A single-header file library that implements decimal floating point (DFP) arithmetic. It can be used when values are expected to be base-10 and have to be exact. For instance where it would be expected that an expression like 0.1 * 10 would equal 1.0 without rounding errors.
## Motivation
## Usage
```c
#include <stdio.h>
#define CHRISSLY_DECIMAL_IMPLEMENTATION
#include "chrissly_decimal.h"

int
main(void)
{
    decimal_t a = {3, 2, 12345}; // 123.45
    decimal_t b = decimal_from_string("-6.789");
    decimal_t r = decimal_add(a, b);
    printf("format: %d.%d value: %d\n", r.integer_places, r.decimal_places, r.value);

    return 0;
}
```
## Status
- addition and subtraction and creation from string works