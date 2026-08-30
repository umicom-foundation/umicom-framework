/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/quant_snapshot.h
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_QUANT_SNAPSHOT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_QUANT_SNAPSHOT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantQuantSnapshot {
    uint32_t curve_count;
    uint32_t surface_count;
    uint32_t model_count;
    uint32_t scenario_count;
} UmiQuantQuantSnapshot;

/* Initialise and validate the quant snapshot contract. */
UmiStatus umi_quant_quant_snapshot_init(UmiQuantQuantSnapshot *record, uint32_t curve_count, uint32_t surface_count, uint32_t model_count, uint32_t scenario_count);

/* Return total governed quantitative artefact count. */
double umi_quant_quant_snapshot_asset_count(const UmiQuantQuantSnapshot *record);

#ifdef __cplusplus
}
#endif

#endif
