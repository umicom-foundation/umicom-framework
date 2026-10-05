/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media/png_memory.h
 * PURPOSE: Share allocation limits and error state between PNG reading and writing.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_PNG_MEMORY_H
#define UMICOM_MEDIA_PNG_MEMORY_H
#include "umicom/base/status.h"
#ifdef UMICOM_MEDIA_PNG_ENABLED
#include <png.h>
#endif
#if defined(UMICOM_MEDIA_PNG_ENABLED) && defined(PNG_USER_MEM_SUPPORTED) && defined(PNG_SETJMP_SUPPORTED)
#define UMI_PNG_MEMORY_AVAILABLE 1
typedef struct UmiPngMemory
{
    size_t allocated;
    UmiStatus status;
} UmiPngMemory;
png_voidp UmiPngAllocate(png_structp png, png_alloc_size_t requested);
void UmiPngRelease(png_structp png, png_voidp pointer);
void UmiPngFailed(png_structp png, png_const_charp message);
void UmiPngWarned(png_structp png, png_const_charp message);
#else
#define UMI_PNG_MEMORY_AVAILABLE 0
#endif
#endif
