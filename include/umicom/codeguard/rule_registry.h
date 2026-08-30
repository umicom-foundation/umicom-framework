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

/* BEGINNER NOTE:
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
typedef struct UmiCodeGuardRuleRegistry UmiCodeGuardRuleRegistry;
UmiStatus umi_codeguard_rule_registry_create(UmiCodeGuardRuleRegistry **out_registry);
void umi_codeguard_rule_registry_destroy(UmiCodeGuardRuleRegistry *registry);
UmiStatus umi_codeguard_rule_registry_add(UmiCodeGuardRuleRegistry *registry,
                                          const UmiCodeGuardRule *rules,
                                          size_t count);
size_t umi_codeguard_rule_registry_count(const UmiCodeGuardRuleRegistry *registry);
const UmiCodeGuardRule *umi_codeguard_rule_registry_at(const UmiCodeGuardRuleRegistry *registry,
                                                       size_t index);
UmiStatus umi_codeguard_rule_registry_add_builtin(UmiCodeGuardRuleRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
