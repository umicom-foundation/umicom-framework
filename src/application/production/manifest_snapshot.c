/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/manifest_snapshot.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/manifest_snapshot.h"

#include <string.h>

UmiStatus umi_application_production_manifest_snapshot_capture(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionManifestSnapshot *out_snapshot)
{
    UmiStatus status;
    if (out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_application_production_manifest_contract_build(
        binding, &out_snapshot->observed);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->manifest_available =
        binding->adoption->manifest_available != 0;
    out_snapshot->composition_available =
        binding->adoption->composition_available != 0;
    out_snapshot->executable_available =
        binding->adoption->executable_available != 0;
    out_snapshot->tests_available =
        binding->adoption->tests_available != 0;
    return UMI_STATUS_OK;
}

