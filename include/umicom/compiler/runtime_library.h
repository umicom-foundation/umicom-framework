/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/runtime_library.h
 *
 * PURPOSE:
 *   Publish the public runtime library contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_RUNTIME_LIBRARY_H
#define UMICOM_COMPILER_RUNTIME_LIBRARY_H
#include "umicom/compiler/abi.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMPILER_MAX_RUNTIME_LIBRARIES 32U
/**
 * Represent the compiler runtime library data shared with callers of this public contract.
 */
typedef struct UmiCompilerRuntimeLibrary { char runtime_id[UMI_COMPILER_ID_CAPACITY]; char path[UMI_COMPILER_PATH_CAPACITY]; UmiCompilerLanguage language; UmiCompilerAbiProfile abi; bool static_link; bool available; } UmiCompilerRuntimeLibrary;
/**
 * Represent the compiler runtime catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiCompilerRuntimeCatalogue { UmiCompilerRuntimeLibrary items[UMI_COMPILER_MAX_RUNTIME_LIBRARIES]; size_t count; uint64_t revision; } UmiCompilerRuntimeCatalogue;
/**
 * Add compiler runtime only after its inputs and available capacity have been checked.
 */
UmiStatus umi_compiler_runtime_register(UmiCompilerRuntimeCatalogue *catalogue,const UmiCompilerRuntimeLibrary *runtime);
/**
 * Provide the compiler runtime resolve operation used by this module and its client
 * applications.
 */
const UmiCompilerRuntimeLibrary *umi_compiler_runtime_resolve(const UmiCompilerRuntimeCatalogue *catalogue,UmiCompilerLanguage language,const UmiCompilerAbiProfile *abi);
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
bool umi_compiler_runtime_catalogue_valid(const UmiCompilerRuntimeCatalogue *catalogue);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
