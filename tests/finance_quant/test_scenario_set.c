#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/scenario_set.h"

int main(void)
{
    UmiQuantScenarioSet set; umi_quant_scenario_set_init(&set); CHECK(umi_quant_scenario_set_add(&set,0.01,-0.05,0.02,0.4)==UMI_STATUS_OK); CHECK(umi_quant_scenario_set_add(&set,-0.01,0.05,-0.02,0.6)==UMI_STATUS_OK); CHECK(umi_quant_scenario_set_total_weight(&set)>0.99);
    return 0;
}
