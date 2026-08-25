#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/regulatory_limit_result.h"

int main(void){ UmiRegulatoryLimitResult r; CHECK(umi_reg_regulatory_limit_result_evaluate(&r,"LCR",0.95,1.0,1)==UMI_STATUS_OK); CHECK(r.breached==1); return 0; }
