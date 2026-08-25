/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/environment_semantics.h
 *
 * PURPOSE:
 *   Describe process environment key comparison, list separators and encoding expectations across targets.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_ENVIRONMENT_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_ENVIRONMENT_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtEnvironmentSemantics { char list_separator; bool key_case_sensitive; bool utf8_native; bool environment_available; } UmiCtEnvironmentSemantics;
UmiCtEnvironmentSemantics umi_ct_environment_semantics_default(UmiCtOperatingSystem os);
bool umi_ct_environment_key_equal(const UmiCtEnvironmentSemantics *semantics,const char *left,const char *right);

#ifdef __cplusplus
}
#endif

#endif
