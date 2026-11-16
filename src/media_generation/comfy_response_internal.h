/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_response_internal.h
 *
 * PURPOSE:
 *   Share bounded JSON admission and receipt identity checks across ComfyUI response readers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_COMFY_RESPONSE_INTERNAL_H
#define UMICOM_COMFY_RESPONSE_INTERNAL_H
#include "umicom/media_generation/comfy_recovery.h"
#include "umicom/language_runtime/json_tree.h"
UmiStatus UmiComfyResponseOpen(const void *bytes, size_t length, size_t maximum,
    const UmiCancellationToken *cancel, UmiJsonTree **out);
UmiStatus UmiComfyResponseMember(const UmiJsonTree *tree, int object, const char *name, int *out);
UmiStatus UmiComfyResponseText(const UmiJsonTree *tree, int object, const char *name,
    char *out, size_t capacity, int allow_empty);
UmiStatus UmiComfyReceiptCheck(const UmiProviderTaskReceipt *receipt, uint64_t revision);
UmiStatus UmiComfyConnectionCheck(const UmiProviderConnection *connection);
#endif
