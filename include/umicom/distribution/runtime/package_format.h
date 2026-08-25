/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/package_format.h
 *
 * PURPOSE:
 *   package format descriptors and platform suitability checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

bool umi_dr_package_format_supported(UmiDrPackageFormat format,UmiDrPlatform platform); bool umi_dr_package_format_installer(UmiDrPackageFormat format);

#ifdef __cplusplus
}
#endif
#endif
