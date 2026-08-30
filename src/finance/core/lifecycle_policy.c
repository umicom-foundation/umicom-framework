/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/lifecycle_policy.c
 *
 * PURPOSE:
 *   Implement reusable lifecycle transition policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/lifecycle_policy.h"

/* Initialize conservative defaults. */ void umi_lifecycle_policy_default(UmiLifecyclePolicy *p){if(p!=NULL){p->flags=0xFFFFFFFFU;p->limit=100U;p->strict=true;}}
/* Evaluate a bounded numeric request. */ bool umi_lifecycle_policy_allows(const UmiLifecyclePolicy *p,uint32_t value,uint32_t required_flags){return p!=NULL&&value<=p->limit&&(p->flags&required_flags)==required_flags;}
