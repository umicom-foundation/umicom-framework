#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/model_registry.h"

int main(void)
{
    UmiQuantModelRegistry r; const UmiQuantModelDescriptor *m=NULL; umi_quant_model_registry_init(&r); CHECK(umi_quant_model_registry_add(&r,"bs-fast",UMI_QUANT_MODEL_BLACK_SCHOLES,5,1)==UMI_STATUS_OK); CHECK(umi_quant_model_registry_add(&r,"bs-reference",UMI_QUANT_MODEL_BLACK_SCHOLES,10,1)==UMI_STATUS_OK); CHECK(umi_quant_model_registry_select(&r,UMI_QUANT_MODEL_BLACK_SCHOLES,&m)==UMI_STATUS_OK); CHECK(m->priority==10);
    return 0;
}
