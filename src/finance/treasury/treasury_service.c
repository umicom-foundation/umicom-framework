/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_service.c
 *
 * PURPOSE:
 *   Implement aggregate treasury service readiness and maintenance governance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_service.h"
#include <string.h>
/*
 * Initialise treasury treasury service from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_treasury_service_init(UmiTreasuryTreasuryService *s){/* Apply this branch only when its contract condition is satisfied. */ if(s)memset(s,0,sizeof *s);}
/*
 * Provide the treasury treasury service start operation used by this module and its client
 * applications.
 */
UmiStatus umi_treasury_treasury_service_start(UmiTreasuryTreasuryService *s,const UmiTreasuryTreasurySnapshot *snap){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!umi_treasury_treasury_snapshot_valid(snap))return UMI_STATUS_INVALID_ARGUMENT;s->snapshot=*snap;s->running=true;return UMI_STATUS_OK;}
/*
 * Provide the treasury treasury service set blockers operation used by this module and its
 * client applications.
 */
void umi_treasury_treasury_service_set_blockers(UmiTreasuryTreasuryService *s,uint32_t blockers){/* Apply this branch only when its contract condition is satisfied. */ if(s)s->blockers=blockers;}
/*
 * Provide the treasury treasury service ready operation used by this module and its client
 * applications.
 */
bool umi_treasury_treasury_service_ready(const UmiTreasuryTreasuryService *s){return s!=NULL && s->running && !s->maintenance && s->blockers==0U;}
