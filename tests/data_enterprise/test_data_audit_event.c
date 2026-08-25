/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_audit_event.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data audit event enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_audit_event.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataAuditEvent item;
    CHECK(umi_data_data_audit_event_init(&item,"e1","op1","user","query",10U,UMI_STATUS_OK) == UMI_STATUS_OK);
    CHECK(item.outcome==UMI_STATUS_OK);
    return 0;
}
