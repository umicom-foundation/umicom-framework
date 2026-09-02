/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/types.h
 *
 * PURPOSE:
 *   Define stable toolkit-neutral RAD designer identifiers, limits and shared enums.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_TYPES_H
#define UMICOM_DESIGNER_RAD_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RAD_ID_CAPACITY 128U
#define UMI_RAD_TEXT_CAPACITY 256U
#define UMI_RAD_PATH_CAPACITY 320U
#define UMI_RAD_VALUE_CAPACITY 512U
#define UMI_RAD_MAX_ITEMS 128U
#define UMI_RAD_MAX_BINDINGS 128U
#define UMI_RAD_MAX_HISTORY 128U
#define UMI_RAD_EMIT_CAPACITY 16384U

/**
 * List the named rad orientation values accepted by this public contract.
 */
typedef enum UmiRadOrientation { UMI_RAD_HORIZONTAL = 1, UMI_RAD_VERTICAL = 2 } UmiRadOrientation;
/**
 * List the named rad severity values accepted by this public contract.
 */
typedef enum UmiRadSeverity { UMI_RAD_INFO = 1, UMI_RAD_WARNING = 2, UMI_RAD_ERROR = 3 } UmiRadSeverity;
/**
 * List the named rad target kind values accepted by this public contract.
 */
typedef enum UmiRadTargetKind { UMI_RAD_TARGET_ABSTRACT = 1, UMI_RAD_TARGET_GTK4 = 2, UMI_RAD_TARGET_QT6 = 3, UMI_RAD_TARGET_NATIVE_WEB = 4 } UmiRadTargetKind;
/**
 * List the named rad transaction state values accepted by this public contract.
 */
typedef enum UmiRadTransactionState { UMI_RAD_TRANSACTION_IDLE = 0, UMI_RAD_TRANSACTION_ACTIVE = 1, UMI_RAD_TRANSACTION_COMMITTED = 2, UMI_RAD_TRANSACTION_ROLLED_BACK = 3 } UmiRadTransactionState;

/**
 * Represent the rad point data shared with callers of this public contract.
 */
typedef struct UmiRadPoint { int32_t x; int32_t y; } UmiRadPoint;
/**
 * Represent the rad size data shared with callers of this public contract.
 */
typedef struct UmiRadSize { int32_t width; int32_t height; } UmiRadSize;
/**
 * Represent the rad rect data shared with callers of this public contract.
 */
typedef struct UmiRadRect { int32_t x; int32_t y; int32_t width; int32_t height; } UmiRadRect;

/* Copy text into a bounded public ABI buffer and always terminate it. */
UmiStatus umi_rad_copy_text(char *destination, size_t capacity, const char *source);
/* Validate stable designer identifiers used across component and document models. */
int umi_rad_id_valid(const char *identifier);

#ifdef __cplusplus
}
#endif
#endif
