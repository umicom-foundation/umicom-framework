#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/credit_curve.h"

int main(void)
{
    UmiQuantCreditCurve c; double h=0.0; umi_quant_credit_curve_init(&c); CHECK(umi_quant_credit_curve_add(&c,365,0.02)==UMI_STATUS_OK); CHECK(umi_quant_credit_curve_add(&c,730,0.03)==UMI_STATUS_OK); CHECK(umi_quant_credit_curve_hazard(&c,500,&h)==UMI_STATUS_OK); CHECK(h>0.029&&h<0.031);
    return 0;
}
