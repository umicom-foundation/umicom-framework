/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/locator.h
 *
 * PURPOSE:
 *   Locate an installed SDK from an explicit prefix or conventional directory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_LOCATOR_H
#define UMICOM_SDK_LOCATOR_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Locate an SDK without changing output on failure.
 * A nonempty preferred_prefix is authoritative and must be an absolute,
 * readable NUL-terminated path. Null or empty selects the same conventional
 * search as umi_sdk_locate. The selected prefix may overlap output storage.
 * On success, only the selected path and its terminator are written. On any
 * failure, every output byte remains unchanged. capacity includes the NUL;
 * null output or zero capacity returns INVALID_ARGUMENT without probing.
 * Keep preferred storage stable during the call and serialize concurrent use.
 * This checks installed file presence; it does not qualify a compiler, linker,
 * binary ABI, runtime or the trustworthiness of the selected package. */
UmiStatus umi_sdk_locate_checked(const char *preferred_prefix,char *out_prefix,size_t capacity);

/**
 * Provide the sdk locate operation used by this module and its client applications.
 */
/** A supplied prefix must be absolute and is the only location inspected.
 * Null or empty selects the retained conventional-location search. Output clears
 * on failure; presence is not compiler, linker or ABI qualification. */
/* The legacy locator also accepts overlapping preferred/output storage. Its
 * documented failure clearing occurs after probing, never before selection. */
UmiStatus umi_sdk_locate(const char *preferred_prefix,char *out_prefix,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
