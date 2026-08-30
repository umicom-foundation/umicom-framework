/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_correlation_matrix.c
 *
 * PURPOSE:
 *   Implement the test correlation matrix behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/correlation_matrix.h"

int main(void)
{
    UmiQuantCorrelationMatrix matrix; double value=0.0;
    CHECK(umi_quant_correlation_matrix_init(&matrix,3U)==UMI_STATUS_OK);
    CHECK(umi_quant_correlation_matrix_set(&matrix,0U,1U,0.5)==UMI_STATUS_OK);
    CHECK(umi_quant_correlation_matrix_get(&matrix,1U,0U,&value)==UMI_STATUS_OK);
    CHECK(value>0.49&&value<0.51);
    return 0;
}
