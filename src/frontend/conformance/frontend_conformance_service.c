/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/frontend_conformance_service.c
 *
 * PURPOSE:
 *   top-level Framework-owned service for renderer registration and conformance evaluation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/frontend_conformance_service.h"

void umi_fc_frontend_conformance_service_init(UmiFcFrontendConformanceService *service){if(service!=NULL){*service=(UmiFcFrontendConformanceService){0};umi_fc_renderer_registry_init(&service->renderers);umi_fc_parity_policy_default(&service->policy);}}
UmiStatus umi_fc_frontend_conformance_service_register(UmiFcFrontendConformanceService *service,const UmiFcRendererProfile *profile){UmiStatus st;if(service==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_fc_renderer_registry_upsert(&service->renderers,profile);if(st==UMI_STATUS_OK)service->revision++;return st;}
UmiFcOutcome umi_fc_frontend_conformance_service_evaluate(const UmiFcFrontendConformanceService *service,const char *renderer_id,uint64_t required,double score,double accessibility,double layout,size_t degraded,UmiFcFrontendHealth *out_health){UmiFcRendererProfile p;uint64_t missing;size_t blockers;if(service==NULL||renderer_id==NULL||out_health==NULL)return UMI_FC_FAIL;if(umi_fc_renderer_registry_find(&service->renderers,renderer_id,&p)!=UMI_STATUS_OK)return UMI_FC_FAIL;missing=required&~p.capabilities;blockers=(size_t)__builtin_popcountll(missing);*out_health=(UmiFcFrontendHealth){0U,degraded,0U,blockers,score};if(!umi_fc_parity_policy_accepts(&service->policy,score,accessibility,layout,degraded,blockers)){out_health->failed=1U;return UMI_FC_FAIL;}out_health->passed=1U;return degraded>0U?UMI_FC_DEGRADED:UMI_FC_PASS;}
