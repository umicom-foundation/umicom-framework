/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/directory_package.h
 *
 * PURPOSE:
 *   Provide a deterministic built-in directory-package provider for tests and portable staging workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This provider represents a package as a prepared directory and is useful before invoking ZIP, MSIX or installer-specific tools.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_DIRECTORY_PACKAGE_H
#define INCLUDE_UMICOM_DELIVERY_DIRECTORY_PACKAGE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/package_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_directory_package_provider(UmiPackageProvider *provider);

#ifdef __cplusplus
}
#endif

#endif
