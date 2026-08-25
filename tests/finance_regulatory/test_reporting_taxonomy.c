#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reporting_taxonomy.h"

int main(void)
{
    UmiReportingTaxonomy record;
    CHECK(umi_reg_reporting_taxonomy_init(&record, "taxonomy_id-1", "authority_id-1", "Version", 1) == UMI_STATUS_OK);
    CHECK(record.taxonomy_id[0] != '\0');
    CHECK(record.authority_id[0] != '\0');
    CHECK(record.version[0] != '\0');
    return 0;
}
