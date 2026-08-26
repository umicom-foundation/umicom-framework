/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/prudential_service.c
 *
 * PURPOSE:
 *   Aggregate Framework prudential service readiness and control state.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/prudential_service.h"

UmiStatus umi_pru_prudential_service_evaluate(UmiPrudentialService *service,int capital_ready,int liquidity_ready,int stress_ready,int governance_ready,size_t blocked_controls) { if(service==NULL)return UMI_STATUS_INVALID_ARGUMENT; service->capital_ready=capital_ready?1:0; service->liquidity_ready=liquidity_ready?1:0; service->stress_ready=stress_ready?1:0; service->governance_ready=governance_ready?1:0; service->blocked_controls=blocked_controls; return UMI_STATUS_OK; }
int umi_pru_prudential_service_ready(const UmiPrudentialService *service) { return service!=NULL&&service->capital_ready&&service->liquidity_ready&&service->stress_ready&&service->governance_ready&&service->blocked_controls==0U?1:0; }
