/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/policy.h
 *
 * PURPOSE:
 *   Define a small capability-based policy engine used by applications,
 *   modules, plug-ins, native tools, agents, and repository operations before
 *   privileged actions are executed.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_POLICY_H
#define UMICOM_SECURITY_POLICY_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_POLICY_TEXT_CAPACITY 128U
#define UMI_POLICY_MAX_RULES 256U

typedef enum UmiPolicyEffect {
    UMI_POLICY_DENY = 0,
    UMI_POLICY_ALLOW = 1
} UmiPolicyEffect;

typedef struct UmiPolicyRule {
    const char *principal;
    const char *capability;
    const char *resource;
    UmiPolicyEffect effect;
} UmiPolicyRule;

typedef struct UmiPolicyDecision {
    UmiPolicyEffect effect;
    const char *matched_principal;
    const char *matched_capability;
    const char *matched_resource;
} UmiPolicyDecision;

typedef struct UmiPolicyEngine UmiPolicyEngine;

UmiStatus umi_policy_engine_create(UmiPolicyEngine **out_engine);
void umi_policy_engine_destroy(UmiPolicyEngine *engine);
UmiStatus umi_policy_engine_add(UmiPolicyEngine *engine,
                                const UmiPolicyRule *rule);
UmiStatus umi_policy_engine_clear(UmiPolicyEngine *engine);
UmiPolicyDecision umi_policy_engine_authorize(
    const UmiPolicyEngine *engine,
    const char *principal,
    const char *capability,
    const char *resource
);
size_t umi_policy_engine_count(const UmiPolicyEngine *engine);

#ifdef __cplusplus
}
#endif

#endif
