/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/path.h
 *
 * PURPOSE:
 *   Provide deterministic UTF-8 path manipulation without accessing storage.
 *   The service keeps path parsing, normalisation, comparison and relative-path
 *   calculation separate from filesystem input/output operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_PATH_H
#define UMICOM_PLATFORM_PATH_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PATH_CAPACITY 2048U

/**
 * Copy path into module-owned storage so callers keep ownership of their input values.
 */
UmiStatus umi_path_copy(char *out_path,
                        size_t capacity,
                        const char *path);
/**
 * Provide the path normalise operation used by this module and its client applications.
 */
UmiStatus umi_path_normalise(const char *path,
                             char *out_path,
                             size_t capacity);
/**
 * Provide the path join operation used by this module and its client applications.
 */
UmiStatus umi_path_join(const char *left,
                        const char *right,
                        char *out_path,
                        size_t capacity);
/**
 * Provide the path parent operation used by this module and its client applications.
 */
UmiStatus umi_path_parent(const char *path,
                          char *out_path,
                          size_t capacity);
/**
 * Provide the path basename operation used by this module and its client applications.
 */
UmiStatus umi_path_basename(const char *path,
                            char *out_name,
                            size_t capacity);
/**
 * Provide the path extension operation used by this module and its client applications.
 */
UmiStatus umi_path_extension(const char *path,
                             char *out_extension,
                             size_t capacity);
/**
 * Provide the path stem operation used by this module and its client applications.
 */
UmiStatus umi_path_stem(const char *path,
                        char *out_stem,
                        size_t capacity);
/**
 * Provide the path absolute operation used by this module and its client applications.
 */
UmiStatus umi_path_absolute(const char *path,
                            const char *base_directory,
                            char *out_path,
                            size_t capacity);
/**
 * Provide the path relative operation used by this module and its client applications.
 */
UmiStatus umi_path_relative(const char *base_directory,
                            const char *target_path,
                            char *out_path,
                            size_t capacity);
/**
 * Provide the path is absolute operation used by this module and its client applications.
 */
int umi_path_is_absolute(const char *path);
/**
 * Provide the path equal operation used by this module and its client applications.
 */
int umi_path_equal(const char *left, const char *right);
/**
 * Provide the path is within operation used by this module and its client applications.
 */
int umi_path_is_within(const char *parent, const char *candidate);

#ifdef __cplusplus
}
#endif

#endif
