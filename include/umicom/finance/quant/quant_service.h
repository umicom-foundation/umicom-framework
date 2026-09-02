/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/quant_service.h
 *
 * PURPOSE:
 *   Aggregate quantitative artefacts and enforce readiness gates.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_QUANT_SERVICE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_QUANT_SERVICE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant service data shared with callers of this public contract.
 */
typedef struct UmiQuantService { uint32_t curve_count; uint32_t surface_count; uint32_t model_count; uint32_t scenario_count; int market_data_ready; int calibration_ready; } UmiQuantService;
/* Initialise an empty quantitative service state. */
void umi_quant_service_init(UmiQuantService *service);
/* Update governed artefact counts and readiness evidence. */
UmiStatus umi_quant_service_configure(UmiQuantService *service, uint32_t curve_count, uint32_t surface_count, uint32_t model_count, uint32_t scenario_count, int market_data_ready, int calibration_ready);
/* Return one when the service has market data and at least one enabled model. */
int umi_quant_service_ready(const UmiQuantService *service);

#ifdef __cplusplus
}
#endif

#endif
