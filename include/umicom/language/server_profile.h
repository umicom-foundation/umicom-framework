/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/server_profile.h
 *
 * PURPOSE:
 *   Publish the public server profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_SERVER_PROFILE_H
#define UMICOM_LANGUAGE_SERVER_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_SERVER_PROFILE_CAPACITY 128U

enum UmiLanguageServerCapability {
    UMI_LANGUAGE_CAP_COMPLETION = 1ULL << 0,
    UMI_LANGUAGE_CAP_HOVER = 1ULL << 1,
    UMI_LANGUAGE_CAP_SIGNATURE = 1ULL << 2,
    UMI_LANGUAGE_CAP_DEFINITION = 1ULL << 3,
    UMI_LANGUAGE_CAP_REFERENCES = 1ULL << 4,
    UMI_LANGUAGE_CAP_SYMBOLS = 1ULL << 5,
    UMI_LANGUAGE_CAP_RENAME = 1ULL << 6,
    UMI_LANGUAGE_CAP_FORMATTING = 1ULL << 7,
    UMI_LANGUAGE_CAP_CODE_ACTION = 1ULL << 8,
    UMI_LANGUAGE_CAP_SEMANTIC_TOKENS = 1ULL << 9,
    UMI_LANGUAGE_CAP_INLAY_HINTS = 1ULL << 10,
    UMI_LANGUAGE_CAP_FOLDING = 1ULL << 11
};

/**
 * Represent the language server profile data shared with callers of this public contract.
 */
typedef struct UmiLanguageServerProfile {
    char id[128];
    char display_name[256];
    char executable[1024];
    char arguments[2048];
    char language_ids[512];
    uint64_t capabilities;
    int enabled;
    int autostart;
    uint64_t revision;
} UmiLanguageServerProfile;

/**
 * Represent the language server profile registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageServerProfileRegistry UmiLanguageServerProfileRegistry;
/**
 * Initialise language server profile registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_server_profile_registry_create(
    UmiLanguageServerProfileRegistry **out_registry);
/**
 * Release or reset state held by language server profile registry so the same storage can
 * be reused safely.
 */
void umi_language_server_profile_registry_destroy(
    UmiLanguageServerProfileRegistry *registry);
/**
 * Provide the language server profile registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_server_profile_registry_upsert(
    UmiLanguageServerProfileRegistry *registry,
    const UmiLanguageServerProfile *profile);
/**
 * Find language server profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_server_profile_registry_find(
    const UmiLanguageServerProfileRegistry *registry, const char *id,
    UmiLanguageServerProfile *out_profile);
/**
 * Find language server profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_server_profile_registry_at(
    const UmiLanguageServerProfileRegistry *registry, size_t index,
    UmiLanguageServerProfile *out_profile);
/**
 * Return the number of records represented by language server profile registry without
 * changing their state.
 */
size_t umi_language_server_profile_registry_count(
    const UmiLanguageServerProfileRegistry *registry);
/**
 * Provide the language server profile supports operation used by this module and its
 * client applications.
 */
int umi_language_server_profile_supports(
    const UmiLanguageServerProfile *profile, uint64_t capability);

#ifdef __cplusplus
}
#endif
#endif
