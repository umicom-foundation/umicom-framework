#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reconciliation_workflow.h"

int main(void)
{
    UmiReconciliationWorkflow record;
    CHECK(umi_reg_reconciliation_workflow_init(&record, "workflow_id-1", "break_id-1", "owner_id-1", 1) == UMI_STATUS_OK);
    CHECK(record.workflow_id[0] != '\0');
    CHECK(record.break_id[0] != '\0');
    CHECK(record.owner_id[0] != '\0');
    return 0;
}
