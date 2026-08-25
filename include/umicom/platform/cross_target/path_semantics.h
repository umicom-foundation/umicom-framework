/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/path_semantics.h
 *
 * PURPOSE:
 *   Describe path separators, case sensitivity and drive/root rules for portable Framework filesystem code.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_PATH_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_PATH_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtPathSemantics { char separator; char alternate_separator; bool case_sensitive; bool drive_letters; bool unc_paths; uint32_t max_path_hint; } UmiCtPathSemantics;
UmiCtPathSemantics umi_ct_path_semantics_default(UmiCtOperatingSystem os);
bool umi_ct_path_is_absolute(const UmiCtPathSemantics *semantics,const char *path);

#ifdef __cplusplus
}
#endif

#endif
