/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/rust_provider.h
 *
 * PURPOSE:
 *   Publish the public rust provider contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_RUST_PROVIDER_H
#define UMICOM_COMPILER_RUST_PROVIDER_H
#include "umicom/compiler/provider.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the compiler rust provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_rust_provider(const char *executable,const char *target);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
