/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_integrity_scan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the integrity scan enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/integrity_scan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataIntegrityScan s; CHECK(umi_data_integrity_scan_begin(&s,"scan1",10U)==UMI_STATUS_OK); CHECK(umi_data_integrity_scan_record(&s,true,false)==UMI_STATUS_OK); CHECK(umi_data_integrity_scan_finish(&s,20U)==UMI_STATUS_OK); CHECK(s.state==UMI_DATA_ENTERPRISE_DEGRADED);
    return 0;
}
