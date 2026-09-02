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
const UmiCodeGuardRule *umi_codeguard_rule_registry_at(const UmiCodeGuardRuleRegistry *registry,
                                                       size_t index);
/**
 * Provide the codeguard rule registry add builtin operation used by this module and its
 * client applications.
 */
UmiStatus umi_codeguard_rule_registry_add_builtin(UmiCodeGuardRuleRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
