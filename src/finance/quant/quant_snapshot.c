/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/quant_snapshot.c
 *
 * PURPOSE:
 *   Summarise quantitative engine readiness and workload.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/quant_snapshot.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_quant_snapshot_init(UmiQuantQuantSnapshot *record, uint32_t curve_count, uint32_t surface_count, uint32_t model_count, uint32_t scenario_count)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(curve_count <= 100000U && surface_count <= 100000U && model_count <= 100000U && scenario_count <= 100000U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->curve_count = curve_count;
    record->surface_count = surface_count;
    record->model_count = model_count;
    record->scenario_count = scenario_count;
    return UMI_STATUS_OK;
}

/* Return total governed quantitative artefact count. */
double umi_quant_quant_snapshot_asset_count(const UmiQuantQuantSnapshot *record)
{
    if (record == NULL) return 0.0;
    return (double)record->curve_count + (double)record->surface_count + (double)record->model_count + (double)record->scenario_count;
}
