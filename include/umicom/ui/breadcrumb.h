/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/breadcrumb.h
 *
 * PURPOSE:
 *   Represent workspace paths and semantic locations as frontend-neutral breadcrumb segments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The model preserves full paths while allowing GTK4, web and future frontends to choose their own breadcrumb widgets.
 */

#ifndef UMICOM_UI_BREADCRUMB_H
#define UMICOM_UI_BREADCRUMB_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_BREADCRUMB_MAX_SEGMENTS 32U
#define UMI_UI_PATH_CAPACITY 1024U

typedef struct UmiUiBreadcrumbSegment {
    char segment_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char path[UMI_UI_PATH_CAPACITY];
    int current;
} UmiUiBreadcrumbSegment;

typedef struct UmiUiBreadcrumbSnapshot {
    UmiUiBreadcrumbSegment segments[UMI_UI_BREADCRUMB_MAX_SEGMENTS];
    size_t count;
    uint64_t revision;
} UmiUiBreadcrumbSnapshot;

typedef struct UmiUiBreadcrumbModel UmiUiBreadcrumbModel;

UmiStatus umi_ui_breadcrumb_model_create(UmiUiBreadcrumbModel **out_model);
void umi_ui_breadcrumb_model_destroy(UmiUiBreadcrumbModel *model);
UmiStatus umi_ui_breadcrumb_set_path(UmiUiBreadcrumbModel *model,
                                     const char *path);
UmiStatus umi_ui_breadcrumb_set_segments(
    UmiUiBreadcrumbModel *model,
    const UmiUiBreadcrumbSegment *segments,
    size_t count);
UmiStatus umi_ui_breadcrumb_snapshot(const UmiUiBreadcrumbModel *model,
                                     UmiUiBreadcrumbSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
