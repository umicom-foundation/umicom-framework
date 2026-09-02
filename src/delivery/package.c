/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/package.c
 *
 * PURPOSE:
 *   Implement package specifications and results without binding products to one archive or installer implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The same Studio release can be packaged as a directory, ZIP, Windows installer or another format through one contract.
 */

#include "umicom/delivery/package.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise package spec from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_package_spec_init(UmiPackageSpec *spec,
                                const char *package_id,
                                UmiPackageFormat format,
                                const char *staging_root,
                                const char *output_path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL || package_id == NULL || staging_root == NULL ||
        output_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(spec, 0, sizeof(*spec));
    status = umi_delivery_copy_text(spec->package_id, sizeof(spec->package_id), package_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(spec->staging_root, sizeof(spec->staging_root), staging_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(spec->output_path, sizeof(spec->output_path), output_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    spec->format = format;
    return UMI_STATUS_OK;
}

/* Check that package spec satisfies its contract before another service relies on it. */
UmiStatus umi_package_spec_validate(const UmiPackageSpec *spec)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (spec->package_id[0] == '\0' || spec->staging_root[0] == '\0' ||
        spec->output_path[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Initialise package result from caller-provided values so later operations receive a
 * known state.
 */
void umi_package_result_init(UmiPackageResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result != NULL) (void)memset(result, 0, sizeof(*result));
}
