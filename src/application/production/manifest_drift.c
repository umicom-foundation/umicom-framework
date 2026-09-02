/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/manifest_drift.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/manifest_drift.h"

#include <string.h>

/* Provide the add drift operation used by this module and its client applications. */
static void add_drift(UmiApplicationProductionManifestDrift *drift,
                      uint32_t flag)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if ((drift->flags & flag) == 0U) {
        drift->flags |= flag;
        drift->difference_count += 1U;
    }
}

/*
 * Provide the application production manifest drift compare operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_manifest_drift_compare(
    const UmiApplicationProductionManifestContract *expected,
    const UmiApplicationProductionManifestSnapshot *actual,
    UmiApplicationProductionManifestDrift *out_drift)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (actual == NULL || out_drift == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_manifest_contract_validate(expected);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_drift, 0, sizeof(*out_drift));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(expected->application_id,
               actual->observed.application_id) != 0 ||
        strcmp(expected->display_name,
               actual->observed.display_name) != 0)
        add_drift(out_drift, UMI_APPLICATION_PRODUCTION_DRIFT_IDENTITY);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(expected->executable_id,
               actual->observed.executable_id) != 0)
        add_drift(out_drift, UMI_APPLICATION_PRODUCTION_DRIFT_EXECUTABLE);
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected->frontend_flags != actual->observed.frontend_flags)
        add_drift(out_drift, UMI_APPLICATION_PRODUCTION_DRIFT_FRONTEND);
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected->panel_count != actual->observed.panel_count)
        add_drift(out_drift, UMI_APPLICATION_PRODUCTION_DRIFT_PANELS);
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected->layout_count != actual->observed.layout_count ||
        strcmp(expected->default_layout_id,
               actual->observed.default_layout_id) != 0)
        add_drift(out_drift, UMI_APPLICATION_PRODUCTION_DRIFT_LAYOUTS);
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected->feature_count != actual->observed.feature_count)
        add_drift(out_drift, UMI_APPLICATION_PRODUCTION_DRIFT_FEATURES);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!actual->manifest_available || !actual->composition_available ||
        !actual->executable_available || !actual->tests_available)
        add_drift(out_drift, UMI_APPLICATION_PRODUCTION_DRIFT_AVAILABILITY);
    out_drift->compatible = out_drift->flags == 0U;
    return UMI_STATUS_OK;
}

