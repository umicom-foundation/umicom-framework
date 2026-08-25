#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/bond_pricer.h"

int main(void)
{
    double price=0.0; CHECK(umi_quant_bond_pricer_price(1000.0,0.05,2,10,0.05,&price)==UMI_STATUS_OK); CHECK(price>999.0&&price<1001.0);
    return 0;
}
