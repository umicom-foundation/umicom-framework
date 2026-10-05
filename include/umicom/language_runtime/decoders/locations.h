/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/locations.h
 *
 * PURPOSE:
 *   Decode Location or LocationLink results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_LOCATIONS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_LOCATIONS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode locations operation used by this module and its
 * client applications.
 */
/* Validate every returned location before publishing the bounded list.
 * Malformed entries, missing link ranges and excess capacity fail explicitly;
 * no entry is silently skipped or truncated. Failures clear the output.
 * Standalone link objects remain accepted by this compatibility API.
 * Callers must correlate the envelope; use LocationCatalogueReadResponse when
 * request ownership and an allocated complete result are required. */
UmiStatus umi_language_runtime_decode_locations(const char*json,UmiLanguageRuntimeLocationList*out);
#ifdef __cplusplus
}
#endif
#endif
