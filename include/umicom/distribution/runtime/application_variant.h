/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/application_variant.h
 *
 * PURPOSE:
 *   platform-specific application variants without moving reusable logic into products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_VARIANT_H
#define UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_VARIANT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrApplicationVariant { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrArchitecture architecture; UmiDrPackageFormat preferred_format; char entrypoint[UMI_DR_PATH_CAPACITY]; } UmiDrApplicationVariant;
void umi_dr_application_variant_init(UmiDrApplicationVariant *value);
bool umi_dr_application_variant_valid(const UmiDrApplicationVariant *value);
uint64_t umi_dr_application_variant_fingerprint(const UmiDrApplicationVariant *value);

#ifdef __cplusplus
}
#endif
#endif
