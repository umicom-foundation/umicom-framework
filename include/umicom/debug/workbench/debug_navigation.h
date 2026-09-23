/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_navigation.h
 *
 * PURPOSE:
 *   Coordinate debugger selection between stack frames, source locations and
 *   disassembled machine instructions without coupling the workflow to a GUI.
 *
 * ARCHITECTURE:
 *   The established Debug Service, runtime platform, inspection session and
 *   disassembly view remain authoritative.  This header only composes those
 *   existing Framework contracts so Studio and future Umicom applications can
 *   share one navigation rule.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_NAVIGATION_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_NAVIGATION_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "umicom/debug_runtime/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_WORKBENCH_NAVIGATION_API_VERSION 1U

/**
 * Copy the currently selected debugger location into a toolkit-neutral value
 * snapshot.  DAP frame identifier zero is valid and is therefore represented
 * separately from has_frame.
 */
typedef struct UmiDebugWorkbenchNavigationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t frame_id;
    uint64_t instruction_address;
    char frame_name[256];
    char instruction_id[UMI_DEBUG_INSTRUCTION_ID_CAPACITY];
    char memory_reference[UMI_DEBUG_INSTRUCTION_MEMORY_REFERENCE_CAPACITY];
    char source_uri[UMI_DEBUG_INSTRUCTION_SOURCE_URI_CAPACITY];
    uint32_t source_line;
    uint32_t source_column;
    uint64_t revision;
    int has_frame;
    int has_instruction;
    int has_source;
} UmiDebugWorkbenchNavigationSnapshot;

static inline UmiDebugDisassemblyView *
umi_debug_workbench_navigation_disassembly(UmiDebugRuntimePlatform *platform)
{
    UmiDebugAdvancedPlatform *advanced;
    UmiDebugInspectionSession *inspection;

    if (platform == NULL) return NULL;
    advanced = umi_debug_runtime_platform_advanced(platform);
    inspection = advanced != NULL
        ? umi_debug_advanced_platform_inspection(advanced)
        : NULL;
    return inspection != NULL
        ? umi_debug_inspection_session_disassembly(inspection)
        : NULL;
}

static inline void
umi_debug_workbench_navigation_copy_source(
    UmiDebugWorkbenchNavigationSnapshot *snapshot,
    const char *uri,
    uint32_t line,
    uint32_t column)
{
    if (snapshot == NULL || uri == NULL || uri[0] == '\0') return;
    (void)snprintf(snapshot->source_uri, sizeof(snapshot->source_uri),
                   "%s", uri);
    snapshot->source_line = line;
    snapshot->source_column = column;
    snapshot->has_source = 1;
}

/**
 * Capture the selected stack-frame and machine-instruction location.  A
 * selected disassembly row takes precedence for source correlation; otherwise
 * the active stack frame supplies the source location.
 */
static inline UmiStatus
umi_debug_workbench_navigation_snapshot(
    UmiDebugRuntimePlatform *platform,
    UmiDebugWorkbenchNavigationSnapshot *out_snapshot)
{
    UmiDebugRuntimePlatformSnapshot runtime;
    UmiDebugService *service;
    UmiDebugStackFrameRegistry *frames;
    UmiDebugStackFrameSnapshot frame;
    UmiDebugDisassemblyView *disassembly;
    UmiDebugInstruction instruction;
    char frame_id[64];
    UmiStatus status;

    if (platform == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_WORKBENCH_NAVIGATION_API_VERSION;

    status = umi_debug_runtime_platform_snapshot(platform, &runtime);
    if (status != UMI_STATUS_OK) return status;
    if (!runtime.active) return UMI_STATUS_INVALID_STATE;

    out_snapshot->frame_id = runtime.active_frame_id;
    out_snapshot->revision = runtime.revision;

    service = umi_debug_runtime_platform_service(platform);
    frames = service != NULL ? umi_debug_service_stack_frame(service) : NULL;
    if (frames != NULL) {
        (void)snprintf(frame_id, sizeof(frame_id), "%llu",
                       (unsigned long long)runtime.active_frame_id);
        if (umi_debug_stack_frame_registry_find(
                frames, frame_id, &frame) == UMI_STATUS_OK) {
            out_snapshot->has_frame = 1;
            (void)snprintf(out_snapshot->frame_name,
                           sizeof(out_snapshot->frame_name), "%s", frame.name);
            umi_debug_workbench_navigation_copy_source(
                out_snapshot, frame.source_uri, frame.line, frame.column);
        }
    }

    disassembly = umi_debug_workbench_navigation_disassembly(platform);
    if (disassembly == NULL) return UMI_STATUS_OK;

    status = umi_debug_disassembly_view_selected(disassembly, &instruction);
    if (status != UMI_STATUS_OK) {
        status = umi_debug_disassembly_view_current(disassembly, &instruction);
    }
    if (status == UMI_STATUS_OK) {
        out_snapshot->has_instruction = 1;
        out_snapshot->instruction_address = instruction.address;
        (void)snprintf(out_snapshot->instruction_id,
                       sizeof(out_snapshot->instruction_id), "%s",
                       instruction.id);
        (void)snprintf(out_snapshot->memory_reference,
                       sizeof(out_snapshot->memory_reference), "%s",
                       instruction.memory_reference);
        if (instruction.has_source) {
            umi_debug_workbench_navigation_copy_source(
                out_snapshot, instruction.source_uri,
                instruction.source_line, instruction.source_column);
        }
    } else if (status != UMI_STATUS_NOT_FOUND) {
        return status;
    }

    return UMI_STATUS_OK;
}

/**
 * Select an existing disassembly instruction and return the resulting
 * cross-navigation location.
 */
static inline UmiStatus
umi_debug_workbench_navigation_select_instruction(
    UmiDebugRuntimePlatform *platform,
    const char *instruction_id,
    UmiDebugWorkbenchNavigationSnapshot *out_snapshot)
{
    UmiDebugDisassemblyView *disassembly;
    UmiStatus status;

    if (platform == NULL || instruction_id == NULL ||
        instruction_id[0] == '\0' || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    disassembly = umi_debug_workbench_navigation_disassembly(platform);
    if (disassembly == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_debug_disassembly_view_select(disassembly, instruction_id);
    if (status != UMI_STATUS_OK) return status;
    return umi_debug_workbench_navigation_snapshot(platform, out_snapshot);
}

/**
 * Select the first retained instruction correlated with the requested source
 * line.  This does not open an editor and never mutates source text; frontend
 * applications decide how to present the returned source location.
 */
static inline UmiStatus
umi_debug_workbench_navigation_follow_source(
    UmiDebugRuntimePlatform *platform,
    const char *source_uri,
    uint32_t source_line,
    UmiDebugWorkbenchNavigationSnapshot *out_snapshot)
{
    UmiDebugDisassemblyView *disassembly;
    size_t index;
    UmiDebugInstruction instruction;
    UmiStatus status;

    if (platform == NULL || source_uri == NULL || source_uri[0] == '\0' ||
        source_line == 0U || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    disassembly = umi_debug_workbench_navigation_disassembly(platform);
    if (disassembly == NULL) return UMI_STATUS_INVALID_STATE;

    for (index = 0U; index < umi_debug_disassembly_view_count(disassembly);
         ++index) {
        status = umi_debug_disassembly_view_at(
            disassembly, index, &instruction);
        if (status != UMI_STATUS_OK) return status;
        if (!instruction.has_source ||
            instruction.source_line != source_line ||
            strcmp(instruction.source_uri, source_uri) != 0) {
            continue;
        }
        status = umi_debug_disassembly_view_select(
            disassembly, instruction.id);
        if (status != UMI_STATUS_OK) return status;
        return umi_debug_workbench_navigation_snapshot(
            platform, out_snapshot);
    }

    return UMI_STATUS_NOT_FOUND;
}

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_WORKBENCH_DEBUG_NAVIGATION_H */
