/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/enterprise_data_service.c
 *
 * PURPOSE:
 *   Implement aggregate enterprise-view service state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/enterprise_data_service.h"
#include <string.h>
/*
 * Initialise ui ent enterprise data service from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_ent_enterprise_data_service_init(UmiUiEntEnterpriseDataService *s){/* Apply this branch only when its contract condition is satisfied. */ if(s){memset(s,0,sizeof *s);s->revision=1U;}}
/*
 * Provide the ui ent enterprise data service update operation used by this module and its
 * client applications.
 */
void umi_ui_ent_enterprise_data_service_update(UmiUiEntEnterpriseDataService *s,size_t g,size_t t,size_t i,size_t r,size_t c){/* Apply this operation only while the related capability or state is available. */ if(s){s->active_grids=g;s->active_trees=t;s->active_inspectors=i;s->materialised_rows=r;s->cached_cells=c;s->revision++;}}
/*
 * Provide the ui ent enterprise data service ready operation used by this module and its
 * client applications.
 */
int umi_ui_ent_enterprise_data_service_ready(const UmiUiEntEnterpriseDataService *s){return s&&s->revision>0U;}
