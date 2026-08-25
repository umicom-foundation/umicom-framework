#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/survival_probability.h"

int main(void)
{
    UmiQuantSurvivalProbability value;
    CHECK(umi_quant_survival_probability_init(&value, 0.02, 5.0) == UMI_STATUS_OK);
    CHECK(umi_quant_survival_probability_probability(&value) > 0.90 && umi_quant_survival_probability_probability(&value) < 0.91);
    return 0;
}
