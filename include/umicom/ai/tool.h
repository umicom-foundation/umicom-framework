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

typedef struct UmiAiTool {
    char tool_id[UMI_AI_ID_CAPACITY];
    char description[UMI_AI_SMALL_TEXT_CAPACITY];
    char permission[UMI_AI_ID_CAPACITY];
    uint32_t flags;
    UmiAiToolInvoke invoke;
    void *user_data;
} UmiAiTool;

UmiStatus umi_ai_tool_init(UmiAiTool *tool,
                           const char *tool_id,
                           const char *description,
                           const char *permission,
                           UmiAiToolInvoke invoke,
                           void *user_data);

#ifdef __cplusplus
}
#endif

#endif
