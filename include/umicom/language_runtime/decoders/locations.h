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
UmiStatus umi_language_runtime_decode_locations(const char*json,UmiLanguageRuntimeLocationList*out);
#ifdef __cplusplus
}
#endif
#endif
