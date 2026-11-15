/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/tool.h
 *
 * PURPOSE:
 *   Define one callable AI tool and its permission metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Tools are explicit Framework objects with permission names; the model only proposes a call and cannot directly execute arbitrary functions.
 */

#ifndef INCLUDE_UMICOM_AI_TOOL_H
#define INCLUDE_UMICOM_AI_TOOL_H

#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiAiToolInvoke)(const char *arguments_json,
                                     char *output,
                                     size_t output_capacity,
                                     void *user_data);

/**
 * Represent the ai tool data shared with callers of this public contract.
 */
typedef struct UmiAiTool {
    char tool_id[UMI_AI_ID_CAPACITY];
    char description[UMI_AI_SMALL_TEXT_CAPACITY];
    char permission[UMI_AI_ID_CAPACITY];
    uint32_t flags;
    UmiAiToolInvoke invoke;
    void *user_data;
} UmiAiTool;

/**
 * Initialise ai tool from caller-provided values so later operations receive a known
 * state.
 */
/* Inputs are readable terminated strings and may refer to tool's old text.
 * Every refusal preserves tool. Empty descriptions and permission names remain
 * valid metadata; tool_id must be nonempty. No user_data ownership is taken. */
UmiStatus umi_ai_tool_init(UmiAiTool *tool,
                           const char *tool_id,
                           const char *description,
                           const char *permission,
                           UmiAiToolInvoke invoke,
                           void *user_data);

/* Validate complete inline text and the callback without executing it.
 * Unknown flag bits remain compatible opaque metadata. This is not permission
 * checking, JSON validation, callback lifetime protection or a native sandbox. */
UmiStatus UmiAiToolValidate(const UmiAiTool *tool);

#ifdef __cplusplus
}
#endif

#endif
