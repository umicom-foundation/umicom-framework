/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reporting_entity.c
 *
 * PURPOSE:
 *   Implement the test reporting entity behavior for
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

#include "umicom/finance/regulatory/reporting_entity.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiReportingEntity record;
    CHECK(umi_reg_reporting_entity_init(&record, "entity_id-1", "Legal Name", "Jurisdiction", 1) == UMI_STATUS_OK);
    CHECK(record.entity_id[0] != '\0');
    CHECK(record.legal_name[0] != '\0');
    CHECK(record.jurisdiction[0] != '\0');
    return 0;
}
