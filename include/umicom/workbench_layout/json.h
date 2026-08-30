/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/json.h
 *
 * PURPOSE:
 *   Encode and decode portable .umilayout JSON while keeping the canonical model independent of GTK, Qt, Wt and filesystem APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_JSON_H
#define UMICOM_WORKBENCH_LAYOUT_JSON_H

#include "umicom/workbench_layout/document.h"
#include "umicom/workbench_layout/validation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutJsonOptions {
    uint32_t structure_size;
    bool pretty;
    bool include_audit;
    bool include_runtime_geometry;
    bool include_content_hash;
    uint32_t indent_width;
} UmiWorkbenchLayoutJsonOptions;

typedef struct UmiWorkbenchLayoutJsonResult {
    uint32_t structure_size;
    UmiStatus status;
    size_t bytes_written;
    size_t bytes_required;
    size_t error_offset;
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutJsonResult;

UmiWorkbenchLayoutJsonOptions
umi_workbench_layout_json_options_default(void);

UmiStatus umi_workbench_layout_json_encode(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutJsonOptions *options,
    char *buffer,
    size_t capacity,
    UmiWorkbenchLayoutJsonResult *out_result);

UmiStatus umi_workbench_layout_json_decode(
    const char *json,
    size_t length,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutJsonResult *out_result);

UmiStatus umi_workbench_layout_json_validate(
    const char *json,
    size_t length,
    UmiWorkbenchLayoutValidationReport *out_report,
    UmiWorkbenchLayoutJsonResult *out_result);

UmiStatus umi_workbench_layout_json_escape(
    const char *text,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_json_unescape(
    const char *text,
    size_t length,
    char *buffer,
    size_t capacity,
    size_t *out_required);

#ifdef __cplusplus
}
#endif

#endif
