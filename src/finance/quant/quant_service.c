/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/quant_service.c
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

#include "umicom/finance/quant/quant_service.h"

#include <math.h>
#include <string.h>

/*
 * Initialise quant service from caller-provided values so later operations receive a known
 * state.
 */
void umi_quant_service_init(UmiQuantService *service){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(service!=NULL)memset(service,0,sizeof *service);}
/*
 * Provide the quant service configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_service_configure(UmiQuantService *service,uint32_t curve_count,uint32_t surface_count,uint32_t model_count,uint32_t scenario_count,int market_data_ready,int calibration_ready){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(service==NULL)return UMI_STATUS_INVALID_ARGUMENT;service->curve_count=curve_count;service->surface_count=surface_count;service->model_count=model_count;service->scenario_count=scenario_count;service->market_data_ready=market_data_ready?1:0;service->calibration_ready=calibration_ready?1:0;return UMI_STATUS_OK;}
/*
 * Provide the quant service ready operation used by this module and its client
 * applications.
 */
int umi_quant_service_ready(const UmiQuantService *service){return service!=NULL&&service->market_data_ready&&service->model_count>0U?1:0;}
