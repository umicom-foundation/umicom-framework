/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/frontend_conformance_service.h
 *
 * PURPOSE:
 *   top-level Framework-owned service for renderer registration and conformance evaluation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_FRONTEND_CONFORMANCE_SERVICE_H
#define UMICOM_FRONTEND_CONFORMANCE_FRONTEND_CONFORMANCE_SERVICE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/renderer_registry.h"
#include "umicom/frontend/conformance/parity_policy.h"
#include "umicom/frontend/conformance/frontend_health.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcFrontendConformanceService { UmiFcRendererRegistry renderers; UmiFcParityPolicy policy; uint64_t revision; } UmiFcFrontendConformanceService;
void umi_fc_frontend_conformance_service_init(UmiFcFrontendConformanceService *service);
UmiStatus umi_fc_frontend_conformance_service_register(UmiFcFrontendConformanceService *service,const UmiFcRendererProfile *profile);
UmiFcOutcome umi_fc_frontend_conformance_service_evaluate(const UmiFcFrontendConformanceService *service,const char *renderer_id,uint64_t required,double score,double accessibility,double layout,size_t degraded,UmiFcFrontendHealth *out_health);

#ifdef __cplusplus
}
#endif
#endif
