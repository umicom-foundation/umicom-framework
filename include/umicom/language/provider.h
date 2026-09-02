/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/provider.h
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
#ifndef UMICOM_LANGUAGE_PROVIDER_H
#define UMICOM_LANGUAGE_PROVIDER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_PROVIDER_CAPACITY 2048U
#define UMI_LANGUAGE_PROVIDER_API_VERSION 1U

/**
 * Represent the language provider snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageProviderSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char language_id[128];
    char kind[64];
    char name[256];
    char command[1024];
    int32_t priority;
    int enabled;
    int healthy;
    uint64_t revision;
} UmiLanguageProviderSnapshot;

/**
 * Represent the language provider registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageProviderRegistry UmiLanguageProviderRegistry;

/**
 * Initialise language provider registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_provider_registry_create(UmiLanguageProviderRegistry **out_registry);
/**
 * Release or reset state held by language provider registry so the same storage can be
 * reused safely.
 */
void umi_language_provider_registry_destroy(UmiLanguageProviderRegistry *registry);
/**
 * Provide the language provider registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_provider_registry_upsert(UmiLanguageProviderRegistry *registry, const UmiLanguageProviderSnapshot *item);
/**
 * Remove language provider registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_provider_registry_remove(UmiLanguageProviderRegistry *registry, const char *id);
/**
 * Find language provider registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_provider_registry_find(const UmiLanguageProviderRegistry *registry, const char *id, UmiLanguageProviderSnapshot *out_item);
/**
 * Find language provider registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_provider_registry_at(const UmiLanguageProviderRegistry *registry, size_t index, UmiLanguageProviderSnapshot *out_item);
/**
 * Return the number of records represented by language provider registry without changing
 * their state.
 */
size_t umi_language_provider_registry_count(const UmiLanguageProviderRegistry *registry);
/**
 * Provide the language provider registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_provider_registry_revision(const UmiLanguageProviderRegistry *registry);
/**
 * Release or reset state held by language provider registry so the same storage can be
 * reused safely.
 */
void umi_language_provider_registry_clear(UmiLanguageProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
