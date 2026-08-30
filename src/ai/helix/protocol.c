/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/protocol.c
 *
 * PURPOSE:
 *   Map strict response-protocol tokens to existing Helix action and bridge states.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/protocol.h"
#include <string.h>

const char *umi_ai_helix_protocol_specification(void)
{
    return
        "Return exactly six single-line fields and no markdown: "
        "disposition=<propose_action|complete|needs_context|defer>; "
        "action=<read|filesystem|build|test|review|source_control|none>; "
        "target=<target or ->; summary=<summary>; confidence=<0..1>; "
        "rationale=<reason>.";
}

const char *umi_ai_helix_protocol_action_text(UmiHelixActionKind kind)
{
    switch (kind) {
        case UMI_HELIX_ACTION_READ: return "read";
        case UMI_HELIX_ACTION_FILESYSTEM: return "filesystem";
        case UMI_HELIX_ACTION_BUILD: return "build";
        case UMI_HELIX_ACTION_TEST: return "test";
        case UMI_HELIX_ACTION_REVIEW: return "review";
        case UMI_HELIX_ACTION_SOURCE_CONTROL: return "source_control";
        default: return "none";
    }
}

UmiStatus umi_ai_helix_protocol_parse_disposition(
    const char *text,
    UmiAiHelixDisposition *out_disposition)
{
    if (text == NULL || out_disposition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(text, "propose_action") == 0) {
        *out_disposition = UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION;
    } else if (strcmp(text, "complete") == 0) {
        *out_disposition = UMI_AI_HELIX_DISPOSITION_COMPLETE;
    } else if (strcmp(text, "needs_context") == 0) {
        *out_disposition = UMI_AI_HELIX_DISPOSITION_NEEDS_CONTEXT;
    } else if (strcmp(text, "defer") == 0) {
        *out_disposition = UMI_AI_HELIX_DISPOSITION_DEFER;
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_helix_protocol_parse_action(
    const char *text,
    UmiHelixActionKind *out_kind)
{
    if (text == NULL || out_kind == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(text, "none") == 0) {
        *out_kind = (UmiHelixActionKind)0;
    } else if (strcmp(text, "read") == 0) {
        *out_kind = UMI_HELIX_ACTION_READ;
    } else if (strcmp(text, "filesystem") == 0) {
        *out_kind = UMI_HELIX_ACTION_FILESYSTEM;
    } else if (strcmp(text, "build") == 0) {
        *out_kind = UMI_HELIX_ACTION_BUILD;
    } else if (strcmp(text, "test") == 0) {
        *out_kind = UMI_HELIX_ACTION_TEST;
    } else if (strcmp(text, "review") == 0) {
        *out_kind = UMI_HELIX_ACTION_REVIEW;
    } else if (strcmp(text, "source_control") == 0) {
        *out_kind = UMI_HELIX_ACTION_SOURCE_CONTROL;
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
