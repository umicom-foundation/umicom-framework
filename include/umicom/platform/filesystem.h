/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/filesystem.h
 *
 * PURPOSE:
 *   Provide the public, toolkit-neutral filesystem and path contract used by
 *   Framework services, native tools, repository management, application
 *   scaffolding, tests, and every Umicom product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_FILESYSTEM_H
#define UMICOM_PLATFORM_FILESYSTEM_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_fs_read_bytes(const char *path,
                            unsigned char **out_bytes,
                            size_t *out_size);
UmiStatus umi_fs_write_bytes(const char *path,
                             const void *bytes,
                             size_t size);
UmiStatus umi_fs_read_text(const char *path, char **out_text, size_t *out_size);
UmiStatus umi_fs_write_text(const char *path, const char *text);
UmiStatus umi_fs_append_text(const char *path, const char *text);
UmiStatus umi_fs_copy_file(const char *source, const char *destination);
UmiStatus umi_fs_make_directories(const char *path);
UmiStatus umi_fs_remove_tree(const char *path);
UmiStatus umi_fs_rename(const char *source, const char *destination);
UmiStatus umi_fs_current_directory(char *out_path, size_t capacity);
UmiStatus umi_fs_temp_directory(char *out_path, size_t capacity);
UmiStatus umi_fs_executable_path(char *out_path, size_t capacity);
UmiStatus umi_fs_join(char *out_path,
                      size_t capacity,
                      const char *left,
                      const char *right);
UmiStatus umi_fs_parent(char *out_path,
                        size_t capacity,
                        const char *path);
UmiStatus umi_fs_normalise(char *path);
int umi_fs_exists(const char *path);
int umi_fs_is_file(const char *path);
int umi_fs_is_directory(const char *path);
int umi_fs_is_absolute(const char *path);
void umi_fs_free_bytes(void *bytes);
void umi_fs_free_text(char *text);

#ifdef __cplusplus
}
#endif

#endif
