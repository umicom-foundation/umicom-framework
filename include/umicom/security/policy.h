/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/policy.h
 *
 * PURPOSE:
 *   Define a small capability-based policy engine used by applications,
 *   modules, plug-ins, native tools, agents, and repository operations before
 *   privileged actions are executed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named policy effect values accepted by this public contract.
 */
typedef enum UmiPolicyEffect {
    UMI_POLICY_DENY = 0,
    UMI_POLICY_ALLOW = 1
} UmiPolicyEffect;

/**
 * Represent the policy rule data shared with callers of this public contract.
 */
typedef struct UmiPolicyRule {
    const char *principal;
    const char *capability;
    const char *resource;
    UmiPolicyEffect effect;
} UmiPolicyRule;

/**
 * Represent the policy decision data shared with callers of this public contract.
 */
typedef struct UmiPolicyDecision {
    UmiPolicyEffect effect;
    const char *matched_principal;
    const char *matched_capability;
    const char *matched_resource;
} UmiPolicyDecision;

/**
 * Represent the policy engine data shared with callers of this public contract.
 */
typedef struct UmiPolicyEngine UmiPolicyEngine;

/**
 * Initialise policy engine from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_policy_engine_create(UmiPolicyEngine **out_engine);
/**
 * Release or reset state held by policy engine so the same storage can be reused safely.
 */
void umi_policy_engine_destroy(UmiPolicyEngine *engine);
/**
 * Add policy engine only after its inputs and available capacity have been checked.
 */
UmiStatus umi_policy_engine_add(UmiPolicyEngine *engine,
                                const UmiPolicyRule *rule);
/**
 * Release or reset state held by policy engine so the same storage can be reused safely.
 */
UmiStatus umi_policy_engine_clear(UmiPolicyEngine *engine);
/**
 * Provide the policy engine authorize operation used by this module and its client
 * applications.
 */
UmiPolicyDecision umi_policy_engine_authorize(
    const UmiPolicyEngine *engine,
    const char *principal,
    const char *capability,
    const char *resource
);
/**
 * Return the number of records represented by policy engine without changing their state.
 */
size_t umi_policy_engine_count(const UmiPolicyEngine *engine);

#ifdef __cplusplus
}
#endif

#endif
