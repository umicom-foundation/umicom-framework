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

UmiStatus umi_package_spec_init(UmiPackageSpec *spec,
                                const char *package_id,
                                UmiPackageFormat format,
                                const char *staging_root,
                                const char *output_path)
{
    UmiStatus status;
    if (spec == NULL || package_id == NULL || staging_root == NULL ||
        output_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(spec, 0, sizeof(*spec));
    status = umi_delivery_copy_text(spec->package_id, sizeof(spec->package_id), package_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(spec->staging_root, sizeof(spec->staging_root), staging_root);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(spec->output_path, sizeof(spec->output_path), output_path);
    if (status != UMI_STATUS_OK) return status;
    spec->format = format;
    return UMI_STATUS_OK;
}

UmiStatus umi_package_spec_validate(const UmiPackageSpec *spec)
{
    if (spec == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (spec->package_id[0] == '\0' || spec->staging_root[0] == '\0' ||
        spec->output_path[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

void umi_package_result_init(UmiPackageResult *result)
{
    if (result != NULL) (void)memset(result, 0, sizeof(*result));
}
