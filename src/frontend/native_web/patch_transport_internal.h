/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/patch_transport_internal.h
 * PURPOSE: Share atomic transport framing around the canonical JSON encoder.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_NATIVE_WEB_PATCH_TRANSPORT_INTERNAL_H
#define UMICOM_NATIVE_WEB_PATCH_TRANSPORT_INTERNAL_H
#include "umicom/frontend/native_web/patch_serializer.h"
UmiStatus UmiNativeWebFramePatch(const UmiNativeWebPatchBatch *batch,
    const char *prefix, const char *suffix, UmiNativeWebMarkupBuffer *out);
#endif
