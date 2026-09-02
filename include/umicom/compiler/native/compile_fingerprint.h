/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/compile_fingerprint.h
 *
 * PURPOSE:
 *   Compose stable compilation fingerprints from source, options, target and dependency state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_COMPILE_FINGERPRINT_H
#define UMICOM_COMPILER_NATIVE_COMPILE_FINGERPRINT_H
#include <stdint.h>
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native compile fingerprint input data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeCompileFingerprintInput { uint64_t source_hash; uint64_t options_hash; uint64_t target_hash; uint64_t dependency_hash; uint64_t compiler_revision_hash; } UmiNativeCompileFingerprintInput;
/**
 * Provide the nc compile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_compile_fingerprint(const UmiNativeCompileFingerprintInput *input);
#ifdef __cplusplus
}
#endif
#endif
