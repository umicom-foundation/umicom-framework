/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/launcher_manifest.h
 *
 * PURPOSE:
 *   launcher executable, arguments and working-directory contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_LAUNCHER_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_LAUNCHER_MANIFEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrLauncherManifest { char id[UMI_DR_ID_CAPACITY]; char executable[UMI_DR_PATH_CAPACITY]; char working_directory[UMI_DR_PATH_CAPACITY]; char arguments[UMI_DR_TEXT_CAPACITY]; bool single_instance; } UmiDrLauncherManifest;
void umi_dr_launcher_manifest_init(UmiDrLauncherManifest *value);
bool umi_dr_launcher_manifest_valid(const UmiDrLauncherManifest *value);
uint64_t umi_dr_launcher_manifest_fingerprint(const UmiDrLauncherManifest *value);

#ifdef __cplusplus
}
#endif
#endif
