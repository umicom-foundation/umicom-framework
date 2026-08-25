/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_backup_manifest.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the backup manifest enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/backup_manifest.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataBackupManifest item;
    CHECK(umi_data_backup_manifest_init(&item,"b1",10U,11U,22U,4096U) == UMI_STATUS_OK);
    CHECK(item.complete);
    return 0;
}
