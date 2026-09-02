/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/lifecycle_policy.h
 *
 * PURPOSE:
 *   Define reusable lifecycle transition policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_LIFECYCLE_POLICY_H
#define UMICOM_FINANCE_CORE_LIFECYCLE_POLICY_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the lifecycle policy data shared with callers of this public contract.
 */
typedef struct UmiLifecyclePolicy { uint32_t flags; uint32_t limit; bool strict; } UmiLifecyclePolicy;
/* Initialize conservative defaults. */ void umi_lifecycle_policy_default(UmiLifecyclePolicy *p);
/* Evaluate a bounded numeric request. */ bool umi_lifecycle_policy_allows(const UmiLifecyclePolicy *p,uint32_t value,uint32_t required_flags);
#ifdef __cplusplus
}
#endif

#endif
