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

/*
 * Provide the ai helix protocol specification operation used by this module and its client
 * applications.
 */
const char *umi_ai_helix_protocol_specification(void)
{
    return
        "Return exactly six single-line fields and no markdown: "
        "disposition=<propose_action|complete|needs_context|defer>; "
        "action=<read|filesystem|build|test|review|source_control|none>; "
        "target=<target or ->; summary=<summary>; confidence=<0..1>; "
        "rationale=<reason>.";
}

/*
 * Provide the ai helix protocol action text operation used by this module and its client
 * applications.
 */
const char *umi_ai_helix_protocol_action_text(UmiHelixActionKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the ai helix protocol parse disposition operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_helix_protocol_parse_disposition(
    const char *text,
    UmiAiHelixDisposition *out_disposition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_disposition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "propose_action") == 0) {
        *out_disposition = UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "complete") == 0) {
        *out_disposition = UMI_AI_HELIX_DISPOSITION_COMPLETE;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "needs_context") == 0) {
        *out_disposition = UMI_AI_HELIX_DISPOSITION_NEEDS_CONTEXT;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "defer") == 0) {
        *out_disposition = UMI_AI_HELIX_DISPOSITION_DEFER;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ai helix protocol parse action operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_helix_protocol_parse_action(
    const char *text,
    UmiHelixActionKind *out_kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_kind == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "none") == 0) {
        *out_kind = (UmiHelixActionKind)0;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "read") == 0) {
        *out_kind = UMI_HELIX_ACTION_READ;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "filesystem") == 0) {
        *out_kind = UMI_HELIX_ACTION_FILESYSTEM;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "build") == 0) {
        *out_kind = UMI_HELIX_ACTION_BUILD;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "test") == 0) {
        *out_kind = UMI_HELIX_ACTION_TEST;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "review") == 0) {
        *out_kind = UMI_HELIX_ACTION_REVIEW;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "source_control") == 0) {
        *out_kind = UMI_HELIX_ACTION_SOURCE_CONTROL;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
