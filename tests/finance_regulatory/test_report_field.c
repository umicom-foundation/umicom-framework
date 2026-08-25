#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/report_field.h"

int main(void)
{
    UmiReportField record;
    CHECK(umi_reg_report_field_init(&record, "field_id-1", "section_id-1", "concept_id-1", 1) == UMI_STATUS_OK);
    CHECK(record.field_id[0] != '\0');
    CHECK(record.section_id[0] != '\0');
    CHECK(record.concept_id[0] != '\0');
    return 0;
}
