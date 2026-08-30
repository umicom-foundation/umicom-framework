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
void umi_treasury_treasury_service_init(UmiTreasuryTreasuryService *s){if(s)memset(s,0,sizeof *s);}
UmiStatus umi_treasury_treasury_service_start(UmiTreasuryTreasuryService *s,const UmiTreasuryTreasurySnapshot *snap){if(!s||!umi_treasury_treasury_snapshot_valid(snap))return UMI_STATUS_INVALID_ARGUMENT;s->snapshot=*snap;s->running=true;return UMI_STATUS_OK;}
void umi_treasury_treasury_service_set_blockers(UmiTreasuryTreasuryService *s,uint32_t blockers){if(s)s->blockers=blockers;}
bool umi_treasury_treasury_service_ready(const UmiTreasuryTreasuryService *s){return s!=NULL && s->running && !s->maintenance && s->blockers==0U;}
