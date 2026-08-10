/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/checksum.c
 *
 * PURPOSE:
 *   Calculate FNV-1a file fingerprints for inventory evidence; cryptographic
 *   signing remains a separate adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/checksum.h"

#include <stddef.h>
#include <stdio.h>
UmiStatus umi_plugin_checksum_file(const char *path, uint64_t *out_checksum, uint64_t *out_size)
{
    FILE *stream; unsigned char buffer[4096]; size_t count, i; uint64_t hash = UINT64_C(1469598103934665603), size = 0U;
    if (path == NULL || out_checksum == NULL || out_size == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(path, "rb"); if (stream == NULL) return UMI_STATUS_IO_ERROR;
    while ((count = fread(buffer, 1U, sizeof(buffer), stream)) > 0U) { for (i = 0U; i < count; ++i) { hash ^= buffer[i]; hash *= UINT64_C(1099511628211); } size += count; }
    if (ferror(stream)) { (void)fclose(stream); return UMI_STATUS_IO_ERROR; }
    (void)fclose(stream); *out_checksum = hash; *out_size = size; return UMI_STATUS_OK;
}
UmiStatus umi_plugin_checksum_text(uint64_t checksum, char *out_text, size_t capacity)
{ int written; if (out_text == NULL || capacity < UMI_PLUGIN_CHECKSUM_TEXT_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT; written = snprintf(out_text, capacity, "%016llx", (unsigned long long)checksum); return written == 16 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR; }
