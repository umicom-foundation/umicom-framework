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

typedef struct UmiLanguageCodeActionRegistry UmiLanguageCodeActionRegistry;

UmiStatus umi_language_code_action_registry_create(UmiLanguageCodeActionRegistry **out_registry);
void umi_language_code_action_registry_destroy(UmiLanguageCodeActionRegistry *registry);
UmiStatus umi_language_code_action_registry_upsert(UmiLanguageCodeActionRegistry *registry, const UmiLanguageCodeActionSnapshot *item);
UmiStatus umi_language_code_action_registry_remove(UmiLanguageCodeActionRegistry *registry, const char *id);
UmiStatus umi_language_code_action_registry_find(const UmiLanguageCodeActionRegistry *registry, const char *id, UmiLanguageCodeActionSnapshot *out_item);
UmiStatus umi_language_code_action_registry_at(const UmiLanguageCodeActionRegistry *registry, size_t index, UmiLanguageCodeActionSnapshot *out_item);
size_t umi_language_code_action_registry_count(const UmiLanguageCodeActionRegistry *registry);
uint64_t umi_language_code_action_registry_revision(const UmiLanguageCodeActionRegistry *registry);
void umi_language_code_action_registry_clear(UmiLanguageCodeActionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
