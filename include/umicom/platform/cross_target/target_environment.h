/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/target_environment.h
 *
 * PURPOSE:
 *   Parse target ABI/runtime environment names independently from operating-system identity.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TARGET_ENVIRONMENT_H
#define UMICOM_PLATFORM_CROSS_TARGET_TARGET_ENVIRONMENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ct_environment_parse(const char *text, UmiCtEnvironment *out_environment);
const char *umi_ct_environment_text(UmiCtEnvironment environment);

#ifdef __cplusplus
}
#endif

#endif
