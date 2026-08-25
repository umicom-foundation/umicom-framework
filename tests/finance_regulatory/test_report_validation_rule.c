#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/report_validation_rule.h"

int main(void)
{
    UmiReportValidationRule record;
    CHECK(umi_reg_report_validation_rule_init(&record, "rule_id-1", "field_id-1", 1.25, 1.25) == UMI_STATUS_OK);
    CHECK(record.rule_id[0] != '\0');
    CHECK(record.field_id[0] != '\0');
    return 0;
}
