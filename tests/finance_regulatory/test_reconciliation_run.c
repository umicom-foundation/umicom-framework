#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reconciliation_run.h"

int main(void){ UmiReconciliationRun r; CHECK(umi_reg_reconciliation_run_init(&r,"run")==UMI_STATUS_OK); CHECK(umi_reg_reconciliation_run_record(&r,10.0,10.01,0.02)==UMI_STATUS_OK); CHECK(r.matched==1U); return 0; }
