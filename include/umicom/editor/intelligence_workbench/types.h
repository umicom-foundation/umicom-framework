/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/types.h
 *
 * PURPOSE:
 *   Define stable search/refactoring identifiers, source locations, phases and shared value types.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_TYPES_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTEL_ID_CAPACITY 128U
#define UMI_EDITOR_INTEL_PATH_CAPACITY 320U
#define UMI_EDITOR_INTEL_TEXT_CAPACITY 256U
#define UMI_EDITOR_INTEL_DETAIL_CAPACITY 384U
#define UMI_EDITOR_INTEL_MAX_ITEMS 128U

typedef enum UmiEditorIntelMatchKind {
    UMI_EDITOR_INTEL_MATCH_TEXT = 1,
    UMI_EDITOR_INTEL_MATCH_SYMBOL = 2,
    UMI_EDITOR_INTEL_MATCH_SEMANTIC = 3
} UmiEditorIntelMatchKind;

typedef enum UmiEditorIntelApplicability {
    UMI_EDITOR_INTEL_APPLICABILITY_UNKNOWN = 0,
    UMI_EDITOR_INTEL_APPLICABILITY_DISABLED = 1,
    UMI_EDITOR_INTEL_APPLICABILITY_AVAILABLE = 2,
    UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED = 3
} UmiEditorIntelApplicability;

typedef enum UmiEditorIntelPhase {
    UMI_EDITOR_INTEL_PHASE_IDLE = 0,
    UMI_EDITOR_INTEL_PHASE_PREPARING = 1,
    UMI_EDITOR_INTEL_PHASE_READY = 2,
    UMI_EDITOR_INTEL_PHASE_APPLYING = 3,
    UMI_EDITOR_INTEL_PHASE_COMMITTED = 4,
    UMI_EDITOR_INTEL_PHASE_ROLLED_BACK = 5,
    UMI_EDITOR_INTEL_PHASE_CANCELLED = 6
} UmiEditorIntelPhase;

typedef enum UmiEditorIntelEditKind {
    UMI_EDITOR_INTEL_EDIT_INSERT = 1,
    UMI_EDITOR_INTEL_EDIT_REPLACE = 2,
    UMI_EDITOR_INTEL_EDIT_DELETE = 3,
    UMI_EDITOR_INTEL_EDIT_CREATE_FILE = 4,
    UMI_EDITOR_INTEL_EDIT_RENAME_FILE = 5
} UmiEditorIntelEditKind;

typedef struct UmiEditorIntelPosition { uint32_t line; uint32_t column; } UmiEditorIntelPosition;
typedef struct UmiEditorIntelRange { UmiEditorIntelPosition start; UmiEditorIntelPosition end; } UmiEditorIntelRange;
typedef struct UmiEditorIntelLocation { char path[UMI_EDITOR_INTEL_PATH_CAPACITY]; UmiEditorIntelRange range; } UmiEditorIntelLocation;
typedef struct UmiEditorIntelEntry {
    char id[UMI_EDITOR_INTEL_ID_CAPACITY];
    char label[UMI_EDITOR_INTEL_TEXT_CAPACITY];
    char detail[UMI_EDITOR_INTEL_DETAIL_CAPACITY];
    UmiEditorIntelLocation location;
    uint32_t score;
    uint32_t flags;
    uint64_t revision;
} UmiEditorIntelEntry;

UmiStatus umi_editor_intel_copy_text(char *destination,size_t capacity,const char *source);
int umi_editor_intel_id_valid(const char *identifier);
int umi_editor_intel_position_compare(UmiEditorIntelPosition left,UmiEditorIntelPosition right);
UmiEditorIntelRange umi_editor_intel_range_normalize(UmiEditorIntelRange range);
UmiStatus umi_editor_intel_location_init(UmiEditorIntelLocation *location,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_entry_init(UmiEditorIntelEntry *entry,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
int umi_editor_intel_entry_valid(const UmiEditorIntelEntry *entry);

#ifdef __cplusplus
}
#endif
#endif
