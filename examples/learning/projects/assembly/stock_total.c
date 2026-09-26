/* Umicom Framework teaching project: assembly / stock_total.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include "stock_total.h"
uint32_t StockTotalC(const uint32_t *values,size_t count)
{
    uint32_t total=0U;
    for (size_t i=0U;i<count;++i) total+=values[i];
    return total;
}
#ifndef UMICOM_LESSON_NATIVE_ASSEMBLY
/* The fallback remains explicitly identified by the build and test output. */
uint32_t StockTotal(const uint32_t *values,size_t count)
{return StockTotalC(values,count);}
#endif
