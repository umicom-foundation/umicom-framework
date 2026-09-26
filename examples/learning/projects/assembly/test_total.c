/* Umicom Framework teaching project: assembly / test_total.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include "stock_total.h"
#include <stdio.h>
#define CHECK(condition) do {if (!(condition)) {fprintf(stderr,"Line %d: %s\n",__LINE__,#condition);return 1;}} while (0)
int main(void)
{
    const uint32_t known[]={12U,8U,4U},wrap[]={UINT32_MAX,1U};
    CHECK(StockTotal(NULL,0U)==0U);
    CHECK(StockTotal(known,3U)==24U);
    CHECK(StockTotal(wrap,2U)==0U);
    uint32_t values[64],seed=17U;
    for (size_t trial=0U;trial<128U;++trial) {
        for (size_t i=0U;i<64U;++i) {seed=seed*1664525U+1013904223U;values[i]=seed;}
        for (size_t n=0U;n<=64U;++n) CHECK(StockTotal(values,n)==StockTotalC(values,n));
    }
#ifdef UMICOM_LESSON_NATIVE_ASSEMBLY
    puts("Native x86-64 Assembly compared with C and independent known totals.");
#else
    puts("C reference selected. This run does not validate Assembly.");
#endif
    return 0;
}
