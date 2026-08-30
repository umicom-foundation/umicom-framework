/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/types.h
 *
 * PURPOSE:
 *   Define stable, toolkit-neutral primitives shared by the professional
 *   source-control, diff/merge, terminal, diagnostics and navigation platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TYPES_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PRODUCTIVITY_API_VERSION 1U
#define UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY 192U
#define UMI_DEVELOPER_PRODUCTIVITY_NAME_CAPACITY 256U
#define UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY 2048U
#define UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY 1024U
#define UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY 1024U
#define UMI_DEVELOPER_PRODUCTIVITY_OUTPUT_CAPACITY 65536U

typedef enum UmiDeveloperProductivitySeverity {
    UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_HINT = 0,
    UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_INFO = 1,
    UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING = 2,
    UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR = 3,
    UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_FATAL = 4
} UmiDeveloperProductivitySeverity;

typedef struct UmiDeveloperProductivityLocation {
    char uri[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    size_t line;
    size_t column;
    size_t end_line;
    size_t end_column;
} UmiDeveloperProductivityLocation;

const char *umi_developer_productivity_severity_text(
    UmiDeveloperProductivitySeverity severity);

UmiStatus umi_developer_productivity_location_validate(
    const UmiDeveloperProductivityLocation *location);

#ifdef __cplusplus
}
#endif

#endif
