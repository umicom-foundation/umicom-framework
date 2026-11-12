/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/manifest.h
 *
 * PURPOSE:
 *   Publish the public manifest contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_MANIFEST_H
#define UMICOM_COMPILER_MANIFEST_H
#include "umicom/compiler/request.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the compiler unit manifest data shared with callers of this public contract.
 */
typedef struct UmiCompilerUnitManifest { char unit_id[UMI_COMPILER_ID_CAPACITY]; char source[UMI_COMPILER_PATH_CAPACITY]; char output[UMI_COMPILER_PATH_CAPACITY]; char provider_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerLanguage language; char exports_abi[UMI_COMPILER_ID_CAPACITY]; bool entry_point; } UmiCompilerUnitManifest;
/**
 * Represent the compiler project manifest data shared with callers of this public
 * contract.
 */
typedef struct UmiCompilerProjectManifest { char project_id[UMI_COMPILER_ID_CAPACITY]; char name[UMI_COMPILER_NAME_CAPACITY]; UmiCompilerUnitManifest units[UMI_COMPILER_MAX_UNITS]; size_t unit_count; char target[UMI_COMPILER_NAME_CAPACITY]; uint64_t revision; } UmiCompilerProjectManifest;
/**
 * Initialise compiler manifest from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_compiler_manifest_init(UmiCompilerProjectManifest *manifest,const char *project_id,const char *name,const char *target);
/**
 * Provide the compiler manifest add unit operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_manifest_add_unit(UmiCompilerProjectManifest *manifest,const UmiCompilerUnitManifest *unit);
/**
 * Provide the compiler manifest find unit operation used by this module and its client
 * applications.
 */
const UmiCompilerUnitManifest *umi_compiler_manifest_find_unit(const UmiCompilerProjectManifest *manifest,const char *unit_id);
/**
 * Check that compiler manifest satisfies its contract before another service relies on it.
 */
UmiStatus umi_compiler_manifest_validate(const UmiCompilerProjectManifest *manifest,char *out_reason,size_t capacity);
/** Check complete required unit text, optional provider/ABI text and a known language. */
bool umi_compiler_unit_manifest_valid(const UmiCompilerUnitManifest *unit);
/** Bound scalar strings, unit count and all active unit rows. Empty scalar fields
 * and zero units permit draft composition; manifest_validate checks required
 * project/target metadata and the established entry-point rule separately.
 * Constructor failures preserve all destination bytes, including borrowed inputs.
 * Insert failures preserve all bytes. Duplicates precede slot/revision exhaustion.
 * Lookup returns a borrowed pointer only after full admission; callers serialize
 * access and keep the manifest alive. Inactive slots and target/provider existence
 * are not qualified. Diagnostic output must not overlap the manifest. */
bool umi_compiler_manifest_storage_valid(const UmiCompilerProjectManifest *manifest);
/** Derive the distinct known-language mask only after complete manifest storage
 * admission. Draft manifests with zero units return zero. Failure preserves the
 * output; it must be separate from manifest storage. This reads metadata only,
 * supplies no synchronization and does not qualify compiler or ABI support. */
UmiStatus umi_compiler_manifest_languages(const UmiCompilerProjectManifest *manifest,
    uint32_t *out_mask);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
