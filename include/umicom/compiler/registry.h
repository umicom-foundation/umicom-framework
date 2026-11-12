/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/registry.h
 *
 * PURPOSE:
 *   Publish the public registry contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_REGISTRY_H
#define UMICOM_COMPILER_REGISTRY_H
#include "umicom/compiler/provider.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the compiler registry data shared with callers of this public contract.
 */
typedef struct UmiCompilerRegistry { UmiCompilerProvider items[UMI_COMPILER_MAX_PROVIDERS]; size_t count; uint64_t revision; } UmiCompilerRegistry;
/**
 * Initialise compiler registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_compiler_registry_init(UmiCompilerRegistry *registry);
/**
 * Add compiler registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_compiler_registry_register(UmiCompilerRegistry *registry,const UmiCompilerProvider *provider);
/**
 * Find compiler registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCompilerProvider *umi_compiler_registry_find(const UmiCompilerRegistry *registry,const char *provider_id);
/**
 * Provide the compiler registry resolve operation used by this module and its client
 * applications.
 */
const UmiCompilerProvider *umi_compiler_registry_resolve(const UmiCompilerRegistry *registry,UmiCompilerLanguage language,bool require_available);
/**
 * Find compiler registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCompilerProvider *umi_compiler_registry_at(const UmiCompilerRegistry *registry,size_t index);
/**
 * Validate the count and every active record's bounded text. Unused slots and
 * revision capacity are not validity conditions; saturated owners remain readable.
 * This does not qualify tool availability, paths, target semantics or ABI support.
 * Register returns INVALID_STATE for malformed owner storage and
 * CAPACITY_EXCEEDED for a new insert without slot/revision capacity, leaving the
 * owner unchanged. Existing identifiers still return ALREADY_EXISTS first.
 * Lookup returns NULL on malformed storage. Returned records are borrowed;
 * callers must serialize access and keep the catalogue alive.
 */
bool umi_compiler_registry_valid(const UmiCompilerRegistry *registry);
/** Explicitly add or replace one complete provider by identifier. Register keeps
 * its add-only contract. Malformed storage, full insertion and exhausted revisions
 * leave the registry unchanged; replacing an existing row is allowed when full. */
UmiStatus umi_compiler_registry_upsert(UmiCompilerRegistry *registry,const UmiCompilerProvider *provider);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
