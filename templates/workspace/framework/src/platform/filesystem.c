/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/platform/filesystem.c
 *
 * PURPOSE:
 *   Implement the filesystem behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the fs read text operation used by this module and its client applications. */
UmiStatus umi_fs_read_text(const char *path, char **out_text, size_t *out_size)
{
    FILE *file;
    long length;
    char *text;
    size_t read_count;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (path == 0 || out_text == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_text = 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_size != 0) *out_size = 0U;
    file = fopen(path, "rb");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (file == 0) return UMI_STATUS_IO_ERROR;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return UMI_STATUS_IO_ERROR; }
    length = ftell(file);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0) { fclose(file); return UMI_STATUS_IO_ERROR; }
    rewind(file);
    text = malloc((size_t)length + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (text == 0) { fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    read_count = fread(text, 1U, (size_t)length, file);
    fclose(file);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (read_count != (size_t)length) { free(text); return UMI_STATUS_IO_ERROR; }
    text[read_count] = '\0';
    *out_text = text;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_size != 0) *out_size = read_count;
    return UMI_STATUS_OK;
}
/* Provide the fs write text operation used by this module and its client applications. */
UmiStatus umi_fs_write_text(const char *path, const char *text)
{
    FILE *file;
    size_t length;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (path == 0 || text == 0) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "wb");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (file == 0) return UMI_STATUS_IO_ERROR;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (fwrite(text, 1U, length, file) != length) { fclose(file); return UMI_STATUS_IO_ERROR; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (fclose(file) != 0) return UMI_STATUS_IO_ERROR;
    return UMI_STATUS_OK;
}
/* Provide the fs exists operation used by this module and its client applications. */
int umi_fs_exists(const char *path)
{
    FILE *file;
    /* Apply this branch only when its contract condition is satisfied. */
    if (path == 0) return 0;
    file = fopen(path, "rb");
    /* Apply this branch only when its contract condition is satisfied. */
    if (file == 0) return 0;
    fclose(file);
    return 1;
}
/* Provide the fs free text operation used by this module and its client applications. */
void umi_fs_free_text(char *text) { free(text); }
