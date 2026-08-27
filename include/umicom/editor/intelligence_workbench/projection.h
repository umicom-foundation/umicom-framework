/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/projection.h
 *
 * PURPOSE:
 *   Convert canonical Framework editor locations and results into the bounded,
 *   toolkit-neutral presentation records used by the intelligence workbench.
 *
 * ARCHITECTURE:
 *   This is an adapter between existing authoritative editor services and the
 *   workbench view models. It does not parse source, execute searches or apply
 *   edits. Applications consume the projection and remain thin composition
 *   clients.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_PROJECTION_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_PROJECTION_H

#include "umicom/editor/intelligence_workbench/types.h"
#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTEL_PROJECTION_API_VERSION 1U

typedef uint32_t UmiEditorIntelProjectionFlags;

enum {
    UMI_EDITOR_INTEL_PROJECTION_SELECTED = 1U << 0,
    UMI_EDITOR_INTEL_PROJECTION_PRIMARY = 1U << 1,
    UMI_EDITOR_INTEL_PROJECTION_VISIBLE = 1U << 2,
    UMI_EDITOR_INTEL_PROJECTION_EXPANDED = 1U << 3,
    UMI_EDITOR_INTEL_PROJECTION_READ_ONLY = 1U << 4,
    UMI_EDITOR_INTEL_PROJECTION_GENERATED = 1U << 5,
    UMI_EDITOR_INTEL_PROJECTION_TRUNCATED = 1U << 6,
    UMI_EDITOR_INTEL_PROJECTION_CONFLICT = 1U << 7,
    UMI_EDITOR_INTEL_PROJECTION_REQUIRED = 1U << 8,
    UMI_EDITOR_INTEL_PROJECTION_SAFE = 1U << 9,
    UMI_EDITOR_INTEL_PROJECTION_PREVIEW = 1U << 10,
    UMI_EDITOR_INTEL_PROJECTION_UNRESOLVED = 1U << 11,
    UMI_EDITOR_INTEL_PROJECTION_CYCLE = 1U << 12
};

UmiStatus umi_editor_intel_projection_entry_from_source(
    UmiEditorIntelEntry *entry,
    const char *id,
    const char *label,
    const char *detail,
    const UmiEditorSourceLocation *source,
    uint32_t score,
    UmiEditorIntelProjectionFlags flags,
    uint64_t revision);

UmiStatus umi_editor_intel_projection_entry_without_source(
    UmiEditorIntelEntry *entry,
    const char *id,
    const char *label,
    const char *detail,
    uint32_t score,
    UmiEditorIntelProjectionFlags flags,
    uint64_t revision);

uint64_t umi_editor_intel_projection_hash_text(const char *text);
uint32_t umi_editor_intel_projection_score_from_rank(int32_t rank);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_PROJECTION_H */
