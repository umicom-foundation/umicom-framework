#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/fx_forward_pricer.h"

int main(void)
{
    double pv=0.0; CHECK(umi_quant_fx_forward_pricer_value(1000000.0,1.10,1.11,0.99,&pv)==UMI_STATUS_OK); CHECK(pv>9899.0&&pv<9901.0);
    return 0;
}
