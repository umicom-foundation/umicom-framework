/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/application_manifest.h
 *
 * PURPOSE:
 *   cross-platform application identity and runtime requirement manifest.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_MANIFEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrApplicationManifest { char id[UMI_DR_ID_CAPACITY]; char name[UMI_DR_TEXT_CAPACITY]; UmiDrVersion version; uint64_t required_capabilities; bool gui; bool networked; } UmiDrApplicationManifest;
void umi_dr_application_manifest_init(UmiDrApplicationManifest *value);
bool umi_dr_application_manifest_valid(const UmiDrApplicationManifest *value);
uint64_t umi_dr_application_manifest_fingerprint(const UmiDrApplicationManifest *value);

#ifdef __cplusplus
}
#endif
#endif
