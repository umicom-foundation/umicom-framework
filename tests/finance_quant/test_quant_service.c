#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/quant_service.h"

int main(void)
{
    UmiQuantService service; umi_quant_service_init(&service); CHECK(!umi_quant_service_ready(&service)); CHECK(umi_quant_service_configure(&service,2U,1U,3U,5U,1,1)==UMI_STATUS_OK); CHECK(umi_quant_service_ready(&service));
    return 0;
}
