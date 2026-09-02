/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/target_triple.h
 *
 * PURPOSE:
 *   Parse and normalise runtime target triples used to bridge platform and existing compiler/toolchain contracts.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TARGET_TRIPLE_H
#define UMICOM_PLATFORM_CROSS_TARGET_TARGET_TRIPLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read ct target triple into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_ct_target_triple_parse(const char *triple, UmiCtTarget *out_target);
/**
 * Provide the ct target triple format operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_target_triple_format(const UmiCtTarget *target, char *out_text, size_t capacity);
/**
 * Provide the ct target compatible operation used by this module and its client
 * applications.
 */
bool umi_ct_target_compatible(const UmiCtTarget *left, const UmiCtTarget *right);

#ifdef __cplusplus
}
#endif

#endif
