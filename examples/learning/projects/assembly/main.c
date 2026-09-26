/* Umicom Framework teaching project: assembly / main.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include "stock_total.h"
#include <inttypes.h>
#include <stdio.h>
int main(void)
{
    const uint32_t values[]={12U,8U,4U};
    printf("Workshop quantities: %" PRIu32 "\n",StockTotal(values,3U));
    return StockTotal(values,3U)==24U?0:1;
}
