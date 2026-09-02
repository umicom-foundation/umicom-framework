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
#endif
