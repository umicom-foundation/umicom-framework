/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/checksum.c
 *
 * PURPOSE:
 *   Calculate FNV-1a file fingerprints for inventory evidence; cryptographic
 *   signing remains a separate adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/checksum.h"

#include <stddef.h>
#include <stdio.h>
/*
 * Provide the plugin checksum file operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_checksum_file(const char *path, uint64_t *out_checksum, uint64_t *out_size)
{
    FILE *stream; unsigned char buffer[4096]; size_t count, i; uint64_t hash = UINT64_C(1469598103934665603), size = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_checksum == NULL || out_size == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(path, "rb"); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (stream == NULL) return UMI_STATUS_IO_ERROR;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((count = fread(buffer, 1U, sizeof(buffer), stream)) > 0U) { /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < count; ++i) { hash ^= buffer[i]; hash *= UINT64_C(1099511628211); } size += count; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (ferror(stream)) { (void)fclose(stream); return UMI_STATUS_IO_ERROR; }
    (void)fclose(stream); *out_checksum = hash; *out_size = size; return UMI_STATUS_OK;
}
/*
 * Provide the plugin checksum text operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_checksum_text(uint64_t checksum, char *out_text, size_t capacity)
{ int written; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (out_text == NULL || capacity < UMI_PLUGIN_CHECKSUM_TEXT_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT; written = snprintf(out_text, capacity, "%016llx", (unsigned long long)checksum); return written == 16 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR; }
