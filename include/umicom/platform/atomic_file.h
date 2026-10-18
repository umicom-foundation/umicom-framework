/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/atomic_file.h
 *
 * PURPOSE:
 *   Provide crash-resistant file replacement for settings, sessions,
 *   documents, manifests and recovery data by writing beside the destination
 *   and replacing it only after the complete content has reached storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_ATOMIC_FILE_H
#define UMICOM_PLATFORM_ATOMIC_FILE_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Write atomic file in its stable representation and report capacity or input failures to
 * the caller.
 */
/* Paths use UTF-8 on Windows. Relative paths are captured as absolute once.
 * Missing parent directories are created. Writes use a same-directory,
 * exclusively created temporary file, flush it, then replace the destination.
 * Existing temporary names are never overwritten or removed; bounded collision
 * retries can return ALREADY_EXISTS. Parent directories must be trusted and
 * stable: this is not an adversarial filesystem sandbox. Independent successful
 * writes to the same destination replace one another in completion order. */
UmiStatus umi_atomic_file_write(const char *path,
                                const void *data,
                                size_t size);
/**
 * Provide the atomic file write text operation used by this module and its client
 * applications.
 */
UmiStatus umi_atomic_file_write_text(const char *path,
                                     const char *text);

#ifdef __cplusplus
}
#endif

#endif
