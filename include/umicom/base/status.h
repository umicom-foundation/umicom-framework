/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/status.h
 *
 * PURPOSE:
 *   Define stable Framework status values shared across module, platform,
 *   messaging, task, storage, plug-in, toolchain and product boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_STATUS_H
#define UMICOM_BASE_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiStatus {
    UMI_STATUS_OK = 0,
    UMI_STATUS_INVALID_ARGUMENT = 1,
    UMI_STATUS_INVALID_STATE = 2,
    UMI_STATUS_CAPACITY_EXCEEDED = 3,
    UMI_STATUS_NOT_FOUND = 4,
    UMI_STATUS_ALREADY_EXISTS = 5,
    UMI_STATUS_OUT_OF_MEMORY = 6,
    UMI_STATUS_PERMISSION_DENIED = 7,
    UMI_STATUS_IO_ERROR = 8,
    UMI_STATUS_PARSE_ERROR = 9,
    UMI_STATUS_UNAVAILABLE = 10,
    UMI_STATUS_NOT_IMPLEMENTED = 11,
    UMI_STATUS_INTERNAL_ERROR = 12,
    UMI_STATUS_CANCELLED = 13,
    UMI_STATUS_TIMEOUT = 14,
    UMI_STATUS_BUSY = 15
} UmiStatus;

const char *umi_status_text(UmiStatus status);

#ifdef __cplusplus
}
#endif

#endif
