/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/filesystem.h
 *
 * PURPOSE:
 *   Provide the public, toolkit-neutral filesystem and path contract used by
 *   Framework services, native tools, repository management, application
 *   scaffolding, tests, and every Umicom product.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_FILESYSTEM_H
#define UMICOM_PLATFORM_FILESYSTEM_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the fs read bytes operation used by this module and its client applications.
 */
UmiStatus umi_fs_read_bytes(const char *path,
                            unsigned char **out_bytes,
                            size_t *out_size);
/**
 * Provide the fs write bytes operation used by this module and its client applications.
 */
UmiStatus umi_fs_write_bytes(const char *path,
                             const void *bytes,
                             size_t size);
/**
 * Provide the fs read text operation used by this module and its client applications.
 */
UmiStatus umi_fs_read_text(const char *path, char **out_text, size_t *out_size);
/**
 * Provide the fs write text operation used by this module and its client applications.
 */
UmiStatus umi_fs_write_text(const char *path, const char *text);
/**
 * Provide the fs append text operation used by this module and its client applications.
 */
UmiStatus umi_fs_append_text(const char *path, const char *text);
/**
 * Provide the fs copy file operation used by this module and its client applications.
 */
UmiStatus umi_fs_copy_file(const char *source, const char *destination);
/**
 * Provide the fs make directories operation used by this module and its client
 * applications.
 */
UmiStatus umi_fs_make_directories(const char *path);
/**
 * Provide the fs remove tree operation used by this module and its client applications.
 */
UmiStatus umi_fs_remove_tree(const char *path);
/**
 * Provide the fs rename operation used by this module and its client applications.
 */
UmiStatus umi_fs_rename(const char *source, const char *destination);
/**
 * Provide the fs current directory operation used by this module and its client
 * applications.
 */
UmiStatus umi_fs_current_directory(char *out_path, size_t capacity);
/**
 * Provide the fs temp directory operation used by this module and its client applications.
 */
UmiStatus umi_fs_temp_directory(char *out_path, size_t capacity);
/**
 * Provide the fs executable path operation used by this module and its client
 * applications.
 */
UmiStatus umi_fs_executable_path(char *out_path, size_t capacity);
/**
 * Provide the fs join operation used by this module and its client applications.
 */
UmiStatus umi_fs_join(char *out_path,
                      size_t capacity,
                      const char *left,
                      const char *right);
/**
 * Provide the fs parent operation used by this module and its client applications.
 */
UmiStatus umi_fs_parent(char *out_path,
                        size_t capacity,
                        const char *path);
/**
 * Provide the fs normalise operation used by this module and its client applications.
 */
UmiStatus umi_fs_normalise(char *path);
/**
 * Provide the fs exists operation used by this module and its client applications.
 */
int umi_fs_exists(const char *path);
/**
 * Provide the fs is file operation used by this module and its client applications.
 */
int umi_fs_is_file(const char *path);
/**
 * Provide the fs is directory operation used by this module and its client applications.
 */
int umi_fs_is_directory(const char *path);
/**
 * Provide the fs is absolute operation used by this module and its client applications.
 */
int umi_fs_is_absolute(const char *path);
/**
 * Provide the fs free bytes operation used by this module and its client applications.
 */
void umi_fs_free_bytes(void *bytes);
/**
 * Provide the fs free text operation used by this module and its client applications.
 */
void umi_fs_free_text(char *text);

#ifdef __cplusplus
}
#endif

#endif
