#ifndef UMICOM_PLATFORM_FILESYSTEM_H
#define UMICOM_PLATFORM_FILESYSTEM_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_fs_read_text(const char *path, char **out_text, size_t *out_size);
UmiStatus umi_fs_write_text(const char *path, const char *text);
int umi_fs_exists(const char *path);
void umi_fs_free_text(char *text);

#ifdef __cplusplus
}
#endif

#endif
