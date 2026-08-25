#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/valuation_lineage.h"

int main(void){UmiEnterpriseValuationLineage l;CHECK(umi_enterprise_valuation_lineage_init(&l,"j",1U,"black76","abc")==UMI_STATUS_OK);CHECK(l.lineage_fingerprint!=0U);return 0;}
