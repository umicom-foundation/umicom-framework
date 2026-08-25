#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reconciliation_break.h"

int main(void)
{
    UmiReconciliationBreak record;
    CHECK(umi_reg_reconciliation_break_init(&record, "break_id-1", "item_id-1", 1.25, 0) == UMI_STATUS_OK);
    CHECK(record.break_id[0] != '\0');
    CHECK(record.item_id[0] != '\0');
    return 0;
}
