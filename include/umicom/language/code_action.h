/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/code_action.h
 *
 * PURPOSE:
 *   Define a provider-neutral language-intelligence record that can be backed by LSP, native analysers or future Umicom language engines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_LANGUAGE_CODE_ACTION_H
#define UMICOM_LANGUAGE_CODE_ACTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_CODE_ACTION_CAPACITY 2048U
#define UMI_LANGUAGE_CODE_ACTION_API_VERSION 1U

/**
 * Represent the language code action snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageCodeActionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char title[512];
    char kind[128];
    char command_id[128];
    char argument[1024];
    int preferred;
    uint64_t revision;
} UmiLanguageCodeActionSnapshot;

/**
 * Represent the language code action registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageCodeActionRegistry UmiLanguageCodeActionRegistry;

/**
 * Initialise language code action registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_code_action_registry_create(UmiLanguageCodeActionRegistry **out_registry);
/**
 * Release or reset state held by language code action registry so the same storage can be
 * reused safely.
 */
void umi_language_code_action_registry_destroy(UmiLanguageCodeActionRegistry *registry);
/**
 * Provide the language code action registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_code_action_registry_upsert(UmiLanguageCodeActionRegistry *registry, const UmiLanguageCodeActionSnapshot *item);
/**
 * Remove language code action registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_code_action_registry_remove(UmiLanguageCodeActionRegistry *registry, const char *id);
/**
 * Find language code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_code_action_registry_find(const UmiLanguageCodeActionRegistry *registry, const char *id, UmiLanguageCodeActionSnapshot *out_item);
/**
 * Find language code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_code_action_registry_at(const UmiLanguageCodeActionRegistry *registry, size_t index, UmiLanguageCodeActionSnapshot *out_item);
/**
 * Return the number of records represented by language code action registry without
 * changing their state.
 */
size_t umi_language_code_action_registry_count(const UmiLanguageCodeActionRegistry *registry);
/**
 * Provide the language code action registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_code_action_registry_revision(const UmiLanguageCodeActionRegistry *registry);
/**
 * Release or reset state held by language code action registry so the same storage can be
 * reused safely.
 */
void umi_language_code_action_registry_clear(UmiLanguageCodeActionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
