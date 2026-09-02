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
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricAuditRecord r; CHECK(umi_fabric_audit_record_init(&r,"a1","trade.route","system",UMI_STATUS_OK,10U)==UMI_STATUS_OK); CHECK(r.fingerprint!=0U);
    return 0;
}
