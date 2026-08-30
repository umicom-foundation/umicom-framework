/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/framing.h
 *
 * PURPOSE:
 *   Encode/decode incremental LSP Content-Length framing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_FRAMING_H
#define UMICOM_LANGUAGE_RUNTIME_FRAMING_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLanguageRuntimeFramer { char buffer[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY]; size_t length,expected_body,header_length; int header_ready; } UmiLanguageRuntimeFramer;
void umi_language_runtime_framer_init(UmiLanguageRuntimeFramer*f);
UmiStatus umi_language_runtime_frame_encode(const char*json,char*out,size_t cap,size_t*out_len);
UmiStatus umi_language_runtime_framer_feed(UmiLanguageRuntimeFramer*f,const void*b,size_t n);
int umi_language_runtime_framer_has_message(const UmiLanguageRuntimeFramer*f);
UmiStatus umi_language_runtime_framer_pop(UmiLanguageRuntimeFramer*f,char*out,size_t cap,size_t*out_len);
#ifdef __cplusplus
}
#endif
#endif
