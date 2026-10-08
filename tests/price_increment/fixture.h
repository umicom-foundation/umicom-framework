/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/price_increment/fixture.h
 * PURPOSE: Keep price-rule checks independent of provider connections and floating-point conversions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PRICE_INCREMENT_TEST_FIXTURE_H
#define UMICOM_PRICE_INCREMENT_TEST_FIXTURE_H
#include "umicom/trading/price_increment.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static inline UmiDecimal Amount(int64_t coefficient, uint8_t scale)
{
    UmiDecimal value = {0};
    value.coefficient = coefficient;
    value.scale = scale;
    return value;
}
#endif
