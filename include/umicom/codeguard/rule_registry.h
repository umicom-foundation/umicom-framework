/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/rule_registry.h
 *
 * PURPOSE:
 *   Store built-in and future plug-in CodeGuard rules behind one registry
 *   contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_RULE_REGISTRY_H
#define UMICOM_CODEGUARD_RULE_REGISTRY_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/codeguard/rule.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the code guard rule registry data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardRuleRegistry UmiCodeGuardRuleRegistry;
/**
 * Initialise codeguard rule registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_codeguard_rule_registry_create(UmiCodeGuardRuleRegistry **out_registry);
/**
 * Release or reset state held by codeguard rule registry so the same storage can be reused
 * safely.
 */
void umi_codeguard_rule_registry_destroy(UmiCodeGuardRuleRegistry *registry);
/**
 * Add codeguard rule registry only after its inputs and available capacity have been
 * checked.
 */
/* Copy count descriptors as one batch; zero count accepts NULL rules. The
 * caller supplies a readable array and keeps all referenced strings alive and
 * unchanged until the registry is destroyed. needle must be non-NULL; an empty
 * needle retains its existing match behavior. Other text fields may be NULL.
 * A range borrowed through registry_at may be appended back to this registry
 * while it remains valid. Successful growth invalidates descriptor borrows;
 * a failed batch leaves count, storage and existing borrows unchanged.
 * This service is not synchronized; serialize mutation and matching. */
UmiStatus umi_codeguard_rule_registry_add(UmiCodeGuardRuleRegistry *registry,
                                          const UmiCodeGuardRule *rules,
                                          size_t count);
/**
 * Return the number of records represented by codeguard rule registry without changing
 * their state.
 */
size_t umi_codeguard_rule_registry_count(const UmiCodeGuardRuleRegistry *registry);
/**
 * Find codeguard rule registry while leaving the underlying catalogue or model owned by
 * this module.
 */
/* Borrowed descriptor, valid until successful growth or destruction. Do not
 * use an old descriptor pointer after an add that may have grown storage. */
const UmiCodeGuardRule *umi_codeguard_rule_registry_at(const UmiCodeGuardRuleRegistry *registry,
                                                       size_t index);
/**
 * Provide the codeguard rule registry add builtin operation used by this module and its
 * client applications.
 */
/* Builtins are appended in groups. A later failure can leave earlier groups
 * present; discard a failed scan candidate rather than treating it as complete. */
UmiStatus umi_codeguard_rule_registry_add_builtin(UmiCodeGuardRuleRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
