#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/governance_snapshot.h"

int main(void){ UmiGovernanceSnapshot s;CHECK(umi_reg_governance_snapshot_init(&s,2U,5U,10U,0.99,0U)==UMI_STATUS_OK);CHECK(umi_reg_governance_snapshot_ready(&s,0.98)==1);return 0; }
