/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media/png_memory.c
 * PURPOSE: Keep libpng allocation accounting and longjmp error reporting in one private Framework owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "png_memory.h"
#include <stddef.h>
#include <stdlib.h>
#if UMI_PNG_MEMORY_AVAILABLE
/* A maximally aligned header keeps native allocation alignment intact while
 * recording the complete allocation charge, including its accounting bytes. */
typedef union PngAllocation
{
    max_align_t alignment;
    size_t size;
} PngAllocation;
png_voidp UmiPngAllocate(png_structp png, png_alloc_size_t requested)
{
    UmiPngMemory *memory = png_get_mem_ptr(png);
    const size_t budget = 8U * 1024U * 1024U;
    if (requested > budget || (size_t)requested > budget - sizeof(PngAllocation) ||
        memory->allocated > budget - sizeof(PngAllocation) - (size_t)requested)
    {
        memory->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return NULL;
    }
    size_t size = sizeof(PngAllocation) + (size_t)requested;
    PngAllocation *block = malloc(size);
    if (block == NULL)
    {
        memory->status = UMI_STATUS_OUT_OF_MEMORY;
        return NULL;
    }
    block->size = size;
    memory->allocated += size;
    return block + 1;
}
void UmiPngRelease(png_structp png, png_voidp pointer)
{
    if (pointer == NULL)
        return;
    UmiPngMemory *memory = png_get_mem_ptr(png);
    PngAllocation *block = (PngAllocation *)pointer - 1;
    memory->allocated -= block->size;
    free(block);
}
void UmiPngFailed(png_structp png, png_const_charp message)
{
    (void)message;
    UmiPngMemory *memory = png_get_error_ptr(png);
    if (memory->status == UMI_STATUS_OK)
        memory->status = UMI_STATUS_PARSE_ERROR;
    png_longjmp(png, 1);
}
void UmiPngWarned(png_structp png, png_const_charp message)
{
    /* Do not echo untrusted metadata. Refuse uncertain output through the
     * owning operation's existing error boundary and cleanup path. */
    UmiPngFailed(png, message);
}
#endif
