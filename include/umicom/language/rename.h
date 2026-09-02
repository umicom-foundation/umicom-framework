/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/rename.h
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
#ifndef UMICOM_LANGUAGE_RENAME_H
#define UMICOM_LANGUAGE_RENAME_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_RENAME_CAPACITY 2048U
#define UMI_LANGUAGE_RENAME_API_VERSION 1U

/**
 * Represent the language rename snapshot data shared with callers of this public contract.
 */
typedef struct UmiLanguageRenameSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char symbol_id[128];
    char old_name[256];
    char new_name[256];
    char document_id[128];
    int state;
    size_t conflict_count;
    uint64_t revision;
} UmiLanguageRenameSnapshot;

/**
 * Represent the language rename registry data shared with callers of this public contract.
 */
typedef struct UmiLanguageRenameRegistry UmiLanguageRenameRegistry;

/**
 * Initialise language rename registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_rename_registry_create(UmiLanguageRenameRegistry **out_registry);
/**
 * Release or reset state held by language rename registry so the same storage can be
 * reused safely.
 */
void umi_language_rename_registry_destroy(UmiLanguageRenameRegistry *registry);
/**
 * Provide the language rename registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_rename_registry_upsert(UmiLanguageRenameRegistry *registry, const UmiLanguageRenameSnapshot *item);
/**
 * Remove language rename registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_rename_registry_remove(UmiLanguageRenameRegistry *registry, const char *id);
/**
 * Find language rename registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_rename_registry_find(const UmiLanguageRenameRegistry *registry, const char *id, UmiLanguageRenameSnapshot *out_item);
/**
 * Find language rename registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_rename_registry_at(const UmiLanguageRenameRegistry *registry, size_t index, UmiLanguageRenameSnapshot *out_item);
/**
 * Return the number of records represented by language rename registry without changing
 * their state.
 */
size_t umi_language_rename_registry_count(const UmiLanguageRenameRegistry *registry);
/**
 * Provide the language rename registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_rename_registry_revision(const UmiLanguageRenameRegistry *registry);
/**
 * Release or reset state held by language rename registry so the same storage can be
 * reused safely.
 */
void umi_language_rename_registry_clear(UmiLanguageRenameRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
