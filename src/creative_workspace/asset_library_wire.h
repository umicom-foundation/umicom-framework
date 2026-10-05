/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_library_wire.h
 * PURPOSE: Share streaming library encoding across memory and native-file destinations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_ASSET_LIBRARY_WIRE_H
#define UMICOM_CREATIVE_ASSET_LIBRARY_WIRE_H
#include "umicom/creative_workspace/asset_library_archive.h"
typedef UmiStatus (*UmiCreativeLibraryEmit)(void *context, const void *bytes, size_t count);
UmiStatus UmiCreativeLibraryArchiveSize(const UmiCreativeAssetLibrary *library, size_t *out);
UmiStatus UmiCreativeLibraryArchiveWrite(const UmiCreativeAssetLibrary *library,
                                         const UmiCancellationToken *cancel, UmiCreativeLibraryEmit emit,
                                         void *context);
#endif
