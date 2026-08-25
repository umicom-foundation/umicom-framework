/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/application_bundle.h
 *
 * PURPOSE:
 *   application bundle metadata, selected variant and immutable content fingerprint.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_BUNDLE_H
#define UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_BUNDLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrApplicationBundle { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; char variant_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; uint64_t content_fingerprint; size_t file_count; } UmiDrApplicationBundle;
void umi_dr_application_bundle_init(UmiDrApplicationBundle *value);
bool umi_dr_application_bundle_valid(const UmiDrApplicationBundle *value);
uint64_t umi_dr_application_bundle_fingerprint(const UmiDrApplicationBundle *value);

#ifdef __cplusplus
}
#endif
#endif
