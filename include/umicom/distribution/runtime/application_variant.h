/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/application_variant.h
 *
 * PURPOSE:
 *   platform-specific application variants without moving reusable logic into products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the dr application variant data shared with callers of this public contract.
 */
typedef struct UmiDrApplicationVariant { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrArchitecture architecture; UmiDrPackageFormat preferred_format; char entrypoint[UMI_DR_PATH_CAPACITY]; } UmiDrApplicationVariant;
/**
 * Initialise dr application variant from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_variant_init(UmiDrApplicationVariant *value);
/**
 * Check that dr application variant satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_variant_valid(const UmiDrApplicationVariant *value);
/**
 * Provide the dr application variant fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_variant_fingerprint(const UmiDrApplicationVariant *value);

#ifdef __cplusplus
}
#endif
#endif
