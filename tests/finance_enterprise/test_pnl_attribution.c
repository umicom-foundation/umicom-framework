#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/pnl_attribution.h"

int main(void){UmiEnterprisePnlAttribution a;CHECK(umi_enterprise_pnl_attribution_calculate(&a,10.0,5.0,2.0,1.0)==UMI_STATUS_OK);CHECK(a.residual==2.0);return 0;}
