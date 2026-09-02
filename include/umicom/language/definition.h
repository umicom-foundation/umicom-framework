/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/definition.h
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
#ifndef UMICOM_LANGUAGE_DEFINITION_H
#define UMICOM_LANGUAGE_DEFINITION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_DEFINITION_CAPACITY 2048U
#define UMI_LANGUAGE_DEFINITION_API_VERSION 1U

/**
 * Represent the language definition snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageDefinitionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char file_extensions[256];
    char mime_types[256];
    char language_server[512];
    char formatter[512];
    int enabled;
    uint64_t revision;
} UmiLanguageDefinitionSnapshot;

/**
 * Represent the language definition registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageDefinitionRegistry UmiLanguageDefinitionRegistry;

/**
 * Initialise language definition registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_definition_registry_create(UmiLanguageDefinitionRegistry **out_registry);
/**
 * Release or reset state held by language definition registry so the same storage can be
 * reused safely.
 */
void umi_language_definition_registry_destroy(UmiLanguageDefinitionRegistry *registry);
/**
 * Provide the language definition registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_definition_registry_upsert(UmiLanguageDefinitionRegistry *registry, const UmiLanguageDefinitionSnapshot *item);
/**
 * Remove language definition registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_definition_registry_remove(UmiLanguageDefinitionRegistry *registry, const char *id);
/**
 * Find language definition registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_definition_registry_find(const UmiLanguageDefinitionRegistry *registry, const char *id, UmiLanguageDefinitionSnapshot *out_item);
/**
 * Find language definition registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_definition_registry_at(const UmiLanguageDefinitionRegistry *registry, size_t index, UmiLanguageDefinitionSnapshot *out_item);
/**
 * Return the number of records represented by language definition registry without
 * changing their state.
 */
size_t umi_language_definition_registry_count(const UmiLanguageDefinitionRegistry *registry);
/**
 * Provide the language definition registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_definition_registry_revision(const UmiLanguageDefinitionRegistry *registry);
/**
 * Release or reset state held by language definition registry so the same storage can be
 * reused safely.
 */
void umi_language_definition_registry_clear(UmiLanguageDefinitionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
