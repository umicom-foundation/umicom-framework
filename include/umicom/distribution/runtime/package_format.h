/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/package_format.h
 *
 * PURPOSE:
 *   package format descriptors and platform suitability checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_FORMAT_H
#define UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_FORMAT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the dr package format supported operation used by this module and its client
 * applications.
 */
bool umi_dr_package_format_supported(UmiDrPackageFormat format,UmiDrPlatform platform); bool umi_dr_package_format_installer(UmiDrPackageFormat format);

#ifdef __cplusplus
}
#endif
#endif
