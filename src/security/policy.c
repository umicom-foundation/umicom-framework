/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/policy.c
 *
 * PURPOSE:
 *   Implement deterministic capability-based allow and deny rules with a
 *   deny-by-default policy suitable for local tools, applications, plug-ins,
 *   agents, and future out-of-process workers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/policy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

typedef struct UmiStoredPolicyRule {
    char principal[UMI_POLICY_TEXT_CAPACITY];
    char capability[UMI_POLICY_TEXT_CAPACITY];
    char resource[UMI_PATH_CAPACITY];
    UmiPolicyEffect effect;
} UmiStoredPolicyRule;

struct UmiPolicyEngine {
    UmiStoredPolicyRule rules[UMI_POLICY_MAX_RULES];
    size_t count;
};

static int umi_policy_match(const char *pattern, const char *value)
{
    size_t pattern_length;
    if (pattern == NULL || value == NULL) {
        return 0;
    }
    if (strcmp(pattern, "*") == 0) {
        return 1;
    }
    pattern_length = strlen(pattern);
    if (pattern_length > 0U && pattern[pattern_length - 1U] == '*') {
        return strncmp(pattern, value, pattern_length - 1U) == 0;
    }
    return strcmp(pattern, value) == 0;
}

UmiStatus umi_policy_engine_create(UmiPolicyEngine **out_engine)
{
    if (out_engine == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_engine = (UmiPolicyEngine *)calloc(1U, sizeof(UmiPolicyEngine));
    return *out_engine != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_policy_engine_destroy(UmiPolicyEngine *engine)
{
    free(engine);
}

UmiStatus umi_policy_engine_add(UmiPolicyEngine *engine,
                                const UmiPolicyRule *rule)
{
    UmiStoredPolicyRule *stored;

    if (engine == NULL || rule == NULL || rule->principal == NULL ||
        rule->capability == NULL || rule->resource == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (engine->count >= UMI_POLICY_MAX_RULES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strlen(rule->principal) >= UMI_POLICY_TEXT_CAPACITY ||
        strlen(rule->capability) >= UMI_POLICY_TEXT_CAPACITY ||
        strlen(rule->resource) >= UMI_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    stored = &engine->rules[engine->count++];
    (void)snprintf(stored->principal,
                   sizeof(stored->principal),
                   "%s",
                   rule->principal);
    (void)snprintf(stored->capability,
                   sizeof(stored->capability),
                   "%s",
                   rule->capability);
    (void)snprintf(stored->resource,
                   sizeof(stored->resource),
                   "%s",
                   rule->resource);
    stored->effect = rule->effect;
    return UMI_STATUS_OK;
}

UmiStatus umi_policy_engine_clear(UmiPolicyEngine *engine)
{
    if (engine == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(engine->rules, 0, sizeof(engine->rules));
    engine->count = 0U;
    return UMI_STATUS_OK;
}

UmiPolicyDecision umi_policy_engine_authorize(
    const UmiPolicyEngine *engine,
    const char *principal,
    const char *capability,
    const char *resource)
{
    UmiPolicyDecision decision = {UMI_POLICY_DENY, NULL, NULL, NULL};
    size_t index;

    if (engine == NULL || principal == NULL || capability == NULL ||
        resource == NULL) {
        return decision;
    }

    for (index = engine->count; index > 0U; --index) {
        const UmiStoredPolicyRule *rule = &engine->rules[index - 1U];
        if (umi_policy_match(rule->principal, principal) &&
            umi_policy_match(rule->capability, capability) &&
            umi_policy_match(rule->resource, resource)) {
            decision.effect = rule->effect;
            decision.matched_principal = rule->principal;
            decision.matched_capability = rule->capability;
            decision.matched_resource = rule->resource;
            return decision;
        }
    }
    return decision;
}

size_t umi_policy_engine_count(const UmiPolicyEngine *engine)
{
    return engine != NULL ? engine->count : 0U;
}
