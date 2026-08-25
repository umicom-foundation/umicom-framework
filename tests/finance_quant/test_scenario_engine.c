#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/scenario_engine.h"

int main(void)
{
    double r,s,v; CHECK(umi_quant_scenario_engine_apply(0.03,100.0,0.20,0.01,-0.10,0.05,&r,&s,&v)==UMI_STATUS_OK); CHECK(r>0.039&&r<0.041); CHECK(s>89.9&&s<90.1); CHECK(v>0.249&&v<0.251);
    return 0;
}
