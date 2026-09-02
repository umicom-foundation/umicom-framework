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

/*
 * Initialise directory from caller-provided values so later operations receive a known
 * state.
 */
static UmiStatus directory_create(void *instance,
                                  const UmiPackageSpec *spec,
                                  UmiPackageResult *result)
{
    UmiStatus status;
    (void)instance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_package_result_init(result);
    status = umi_delivery_copy_text(result->output_path,
                                    sizeof(result->output_path),
                                    spec->output_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    result->succeeded = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the directory package provider operation used by this module and its client
 * applications.
 */
UmiStatus umi_directory_package_provider(UmiPackageProvider *provider)
{
    return umi_package_provider_init(provider,
                                     "umicom.directory",
                                     UMI_PACKAGE_DIRECTORY,
                                     NULL,
                                     directory_create);
}
