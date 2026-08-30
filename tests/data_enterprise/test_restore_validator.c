/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_restore_validator.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the restore validator enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/restore_validator.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataRestorePlan p; UmiDataBackupManifest m; UmiDataRestoreValidation v; CHECK(umi_data_restore_plan_init(&p,"r1","b1",11U,true)==UMI_STATUS_OK); CHECK(umi_data_backup_manifest_init(&m,"b1",1U,11U,22U,100U)==UMI_STATUS_OK); CHECK(umi_data_restore_validator_check(&p,&m,&v)==UMI_STATUS_OK); CHECK(v.valid);
    return 0;
}
