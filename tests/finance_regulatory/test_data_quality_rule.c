/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_data_quality_rule.c
 *
 * PURPOSE:
 *   Implement the test data quality rule behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/data_quality_rule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataQualityRule record;
    CHECK(umi_reg_data_quality_rule_init(&record, "rule_id-1", "domain_id-1", "Dimension", 0.95) == UMI_STATUS_OK);
    CHECK(record.rule_id[0] != '\0');
    CHECK(record.domain_id[0] != '\0');
    CHECK(record.dimension[0] != '\0');
    return 0;
}
