/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/provider.h
 *
 * PURPOSE:
 *   Define a provider-neutral language-intelligence record that can be backed by LSP, native analysers or future Umicom language engines.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiLanguageProviderRegistry UmiLanguageProviderRegistry;

UmiStatus umi_language_provider_registry_create(UmiLanguageProviderRegistry **out_registry);
void umi_language_provider_registry_destroy(UmiLanguageProviderRegistry *registry);
UmiStatus umi_language_provider_registry_upsert(UmiLanguageProviderRegistry *registry, const UmiLanguageProviderSnapshot *item);
UmiStatus umi_language_provider_registry_remove(UmiLanguageProviderRegistry *registry, const char *id);
UmiStatus umi_language_provider_registry_find(const UmiLanguageProviderRegistry *registry, const char *id, UmiLanguageProviderSnapshot *out_item);
UmiStatus umi_language_provider_registry_at(const UmiLanguageProviderRegistry *registry, size_t index, UmiLanguageProviderSnapshot *out_item);
size_t umi_language_provider_registry_count(const UmiLanguageProviderRegistry *registry);
uint64_t umi_language_provider_registry_revision(const UmiLanguageProviderRegistry *registry);
void umi_language_provider_registry_clear(UmiLanguageProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
