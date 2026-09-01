/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/directory_package.c
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

#include "umicom/delivery/directory_package.h"
#include "delivery_internal.h"

static UmiStatus directory_create(void *instance,
                                  const UmiPackageSpec *spec,
                                  UmiPackageResult *result)
{
    UmiStatus status;
    (void)instance;
    if (spec == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_package_result_init(result);
    status = umi_delivery_copy_text(result->output_path,
                                    sizeof(result->output_path),
                                    spec->output_path);
    if (status != UMI_STATUS_OK) return status;
    result->succeeded = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_directory_package_provider(UmiPackageProvider *provider)
{
    return umi_package_provider_init(provider,
                                     "umicom.directory",
                                     UMI_PACKAGE_DIRECTORY,
                                     NULL,
                                     directory_create);
}
