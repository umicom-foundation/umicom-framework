/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/dynamic_library_semantics.h
 *
 * PURPOSE:
 *   Describe shared-library naming and dynamic-loading support for plug-in portability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_DYNAMIC_LIBRARY_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_DYNAMIC_LIBRARY_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ct dynamic library semantics data shared with callers of this public
 * contract.
 */
typedef struct UmiCtDynamicLibrarySemantics { char prefix[16]; char suffix[16]; bool dynamic_loading; bool global_symbols; bool unload; } UmiCtDynamicLibrarySemantics;
/**
 * Provide the ct dynamic library semantics default operation used by this module and its
 * client applications.
 */
UmiCtDynamicLibrarySemantics umi_ct_dynamic_library_semantics_default(UmiCtOperatingSystem os);
/**
 * Provide the ct dynamic library name operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_dynamic_library_name(const UmiCtDynamicLibrarySemantics *semantics,const char *base,char *out_name,size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
