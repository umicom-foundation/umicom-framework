#include "umicom/platform/filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus umi_fs_read_text(const char *path, char **out_text, size_t *out_size)
{
    FILE *file;
    long length;
    char *text;
    size_t read_count;
    if (path == 0 || out_text == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_text = 0;
    if (out_size != 0) *out_size = 0U;
    file = fopen(path, "rb");
    if (file == 0) return UMI_STATUS_IO_ERROR;
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return UMI_STATUS_IO_ERROR; }
    length = ftell(file);
    if (length < 0) { fclose(file); return UMI_STATUS_IO_ERROR; }
    rewind(file);
    text = malloc((size_t)length + 1U);
    if (text == 0) { fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    read_count = fread(text, 1U, (size_t)length, file);
    fclose(file);
    if (read_count != (size_t)length) { free(text); return UMI_STATUS_IO_ERROR; }
    text[read_count] = '\0';
    *out_text = text;
    if (out_size != 0) *out_size = read_count;
    return UMI_STATUS_OK;
}
UmiStatus umi_fs_write_text(const char *path, const char *text)
{
    FILE *file;
    size_t length;
    if (path == 0 || text == 0) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "wb");
    if (file == 0) return UMI_STATUS_IO_ERROR;
    length = strlen(text);
    if (fwrite(text, 1U, length, file) != length) { fclose(file); return UMI_STATUS_IO_ERROR; }
    if (fclose(file) != 0) return UMI_STATUS_IO_ERROR;
    return UMI_STATUS_OK;
}
int umi_fs_exists(const char *path)
{
    FILE *file;
    if (path == 0) return 0;
    file = fopen(path, "rb");
    if (file == 0) return 0;
    fclose(file);
    return 1;
}
void umi_fs_free_text(char *text) { free(text); }
