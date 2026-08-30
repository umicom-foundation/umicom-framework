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

UmiStatus umi_atomic_file_write(const char *path,
                                const void *data,
                                size_t size);
UmiStatus umi_atomic_file_write_text(const char *path,
                                     const char *text);

#ifdef __cplusplus
}
#endif

#endif
