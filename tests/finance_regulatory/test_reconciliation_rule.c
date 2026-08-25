#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reconciliation_rule.h"

int main(void)
{
    UmiReconciliationRule record;
    CHECK(umi_reg_reconciliation_rule_init(&record, "rule_id-1", "left_source-1", "right_source-1", 1.25) == UMI_STATUS_OK);
    CHECK(record.rule_id[0] != '\0');
    CHECK(record.left_source[0] != '\0');
    CHECK(record.right_source[0] != '\0');
    return 0;
}
