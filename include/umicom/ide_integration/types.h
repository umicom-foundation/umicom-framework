/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/types.h
 *
 * PURPOSE:
 *   Define toolkit-neutral IDE integration records shared by editor/document,
 *   Problems, Tests, Source Control, Debug, language intelligence and AI coding.
 *
 * ARCHITECTURE:
 *   This layer does not replace any underlying service. It normalizes cross-
 *   navigation, active-context and self-hosting state so thin IDE applications
 *   can compose the existing Framework consistently.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_TYPES_H
#define UMICOM_IDE_INTEGRATION_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_IDE_INTEGRATION_API_VERSION 1U
#define UMI_IDE_INTEGRATION_ID_CAPACITY 128U
#define UMI_IDE_INTEGRATION_PATH_CAPACITY 2048U
#define UMI_IDE_INTEGRATION_URI_CAPACITY 2048U
#define UMI_IDE_INTEGRATION_TEXT_CAPACITY 4096U
#define UMI_IDE_INTEGRATION_SELECTION_CAPACITY 8192U
#define UMI_IDE_INTEGRATION_GATE_CAPACITY 32U
#define UMI_IDE_INTEGRATION_NAVIGATION_HISTORY_CAPACITY 128U
#define UMI_IDE_INTEGRATION_INLINE_TEXT_CAPACITY 8192U

typedef enum UmiIdeIntegrationDomain {
    UMI_IDE_DOMAIN_EDITOR = 1,
    UMI_IDE_DOMAIN_PROBLEMS = 2,
    UMI_IDE_DOMAIN_TESTS = 3,
    UMI_IDE_DOMAIN_SOURCE_CONTROL = 4,
    UMI_IDE_DOMAIN_DEBUG = 5,
    UMI_IDE_DOMAIN_LANGUAGE = 6,
    UMI_IDE_DOMAIN_AI = 7,
    UMI_IDE_DOMAIN_BUILD = 8,
    UMI_IDE_DOMAIN_WORKSPACE = 9
} UmiIdeIntegrationDomain;

typedef enum UmiIdeNavigationReason {
    UMI_IDE_NAVIGATION_DIRECT = 0,
    UMI_IDE_NAVIGATION_PROBLEM = 1,
    UMI_IDE_NAVIGATION_TEST = 2,
    UMI_IDE_NAVIGATION_SOURCE_CONTROL = 3,
    UMI_IDE_NAVIGATION_DEBUG = 4,
    UMI_IDE_NAVIGATION_SYMBOL = 5,
    UMI_IDE_NAVIGATION_REFERENCE = 6,
    UMI_IDE_NAVIGATION_AI = 7
} UmiIdeNavigationReason;

typedef enum UmiIdeInlineSuggestionState {
    UMI_IDE_INLINE_IDLE = 0,
    UMI_IDE_INLINE_REQUESTED = 1,
    UMI_IDE_INLINE_READY = 2,
    UMI_IDE_INLINE_ACCEPTED = 3,
    UMI_IDE_INLINE_REJECTED = 4,
    UMI_IDE_INLINE_CANCELLED = 5,
    UMI_IDE_INLINE_FAILED = 6
} UmiIdeInlineSuggestionState;

typedef enum UmiIdeWorkflowGateState {
    UMI_IDE_GATE_UNKNOWN = 0,
    UMI_IDE_GATE_PASS = 1,
    UMI_IDE_GATE_WARN = 2,
    UMI_IDE_GATE_BLOCK = 3
} UmiIdeWorkflowGateState;

typedef struct UmiIdeLocation {
    char path[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    char uri[UMI_IDE_INTEGRATION_URI_CAPACITY];
    uint32_t line;
    uint32_t column;
    uint32_t end_line;
    uint32_t end_column;
    uint64_t revision;
    int has_path;
    int has_uri;
} UmiIdeLocation;

typedef struct UmiIdeNavigationTarget {
    UmiIdeIntegrationDomain domain;
    UmiIdeNavigationReason reason;
    char subject_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char label[256];
    UmiIdeLocation location;
    uint64_t sequence;
} UmiIdeNavigationTarget;

typedef struct UmiIdeEditorSelection {
    char document_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char path[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    char language_id[128];
    char text[UMI_IDE_INTEGRATION_SELECTION_CAPACITY];
    uint32_t start_line;
    uint32_t start_column;
    uint32_t end_line;
    uint32_t end_column;
    uint64_t document_revision;
    int dirty;
} UmiIdeEditorSelection;

typedef struct UmiIdeInlineSuggestion {
    char suggestion_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char document_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char path[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    char language_id[128];
    char text[UMI_IDE_INTEGRATION_INLINE_TEXT_CAPACITY];
    uint32_t line;
    uint32_t column;
    uint64_t document_revision;
    uint64_t request_sequence;
    UmiIdeInlineSuggestionState state;
    UmiStatus status;
} UmiIdeInlineSuggestion;

typedef struct UmiIdeWorkflowGate {
    char gate_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char label[256];
    char detail[UMI_IDE_INTEGRATION_TEXT_CAPACITY];
    UmiIdeWorkflowGateState state;
    int required;
    uint64_t revision;
} UmiIdeWorkflowGate;

const char *umi_ide_domain_text(UmiIdeIntegrationDomain domain);
const char *umi_ide_navigation_reason_text(UmiIdeNavigationReason reason);
const char *umi_ide_inline_state_text(UmiIdeInlineSuggestionState state);
const char *umi_ide_gate_state_text(UmiIdeWorkflowGateState state);

#ifdef __cplusplus
}
#endif
#endif
