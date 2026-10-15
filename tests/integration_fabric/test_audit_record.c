/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_audit_record.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the audit record Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/audit_record.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted audit record value. */
static int UmiFabricAuditRecordConstructionCases(void)
{
    UmiFabricAuditRecord *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_audit_record_init(value, "a1", "trade.route", "system", UMI_STATUS_OK, 10U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->record_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_audit_record_init(value, oversized, "trade.route", "system", UMI_STATUS_OK, 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_audit_record_init(value, value->record_id, "trade.route", "system", UMI_STATUS_OK, 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->record_id, "a1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->operation_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_audit_record_init(value, "a1", oversized, "system", UMI_STATUS_OK, 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_audit_record_init(value, "a1", value->operation_id, "system", UMI_STATUS_OK, 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->operation_id, "trade.route") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->actor_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_audit_record_init(value, "a1", "trade.route", oversized, UMI_STATUS_OK, 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_audit_record_init(value, "a1", "trade.route", value->actor_id, UMI_STATUS_OK, 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->actor_id, "system") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricAuditRecordConstructionCases() != 0) return 1;

    UmiFabricAuditRecord r; CHECK(umi_fabric_audit_record_init(&r,"a1","trade.route","system",UMI_STATUS_OK,10U)==UMI_STATUS_OK); CHECK(r.fingerprint!=0U);
    return 0;
}
