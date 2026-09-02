/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/platform/filesystem.h
 *
 * PURPOSE:
 *   Declare the filesystem contract shared by Framework services and thin
 *   applications.
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the fs read text operation used by this module and its client applications.
 */
UmiStatus umi_fs_read_text(const char *path, char **out_text, size_t *out_size);
/**
 * Provide the fs write text operation used by this module and its client applications.
 */
UmiStatus umi_fs_write_text(const char *path, const char *text);
/**
 * Provide the fs exists operation used by this module and its client applications.
 */
int umi_fs_exists(const char *path);
/**
 * Provide the fs free text operation used by this module and its client applications.
 */
void umi_fs_free_text(char *text);

#ifdef __cplusplus
}
#endif

#endif
