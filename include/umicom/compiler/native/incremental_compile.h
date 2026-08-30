/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/incremental_compile.h
 *
 * PURPOSE:
 *   Decide translation-unit rebuilds from source, options, dependency and artifact fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_INCREMENTAL_COMPILE_H
#define UMICOM_COMPILER_NATIVE_INCREMENTAL_COMPILE_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeIncrementalUnit { char path[UMI_NC_PATH_CAPACITY]; uint64_t source_hash; uint64_t options_hash; uint64_t dependency_hash; uint64_t artifact_hash; bool artifact_present; } UmiNativeIncrementalUnit;
UmiStatus umi_nc_incremental_unit_init(UmiNativeIncrementalUnit *unit,const char *path,uint64_t source_hash,uint64_t options_hash,uint64_t dependency_hash);
bool umi_nc_incremental_needs_rebuild(const UmiNativeIncrementalUnit *previous,const UmiNativeIncrementalUnit *current);
#ifdef __cplusplus
}
#endif
#endif
