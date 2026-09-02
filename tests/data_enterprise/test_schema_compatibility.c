/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_compatibility.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema compatibility enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_compatibility.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataSchemaCompatibilityPolicy policy;
    bool allowed = false;
    umi_data_schema_compatibility_default(&policy);
    policy.allow_breaking=false;
    CHECK(umi_data_schema_compatibility_evaluate(&policy, UMI_DATA_BREAKING, &allowed) == UMI_STATUS_OK);
    CHECK(allowed == false);
    return 0;
}
