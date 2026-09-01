/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/external_tool.h
 *
 * PURPOSE:
 *   Describe optional Clang analyzer and sanitizer commands that complement
 *   native scanning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_EXTERNAL_TOOL_H
#define UMICOM_CODEGUARD_EXTERNAL_TOOL_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiCodeGuardExternalTool {
    UMI_CODEGUARD_TOOL_CLANG_ANALYZER = 0,
    UMI_CODEGUARD_TOOL_CLANG_TIDY,
    UMI_CODEGUARD_TOOL_ADDRESS_SANITIZER,
    UMI_CODEGUARD_TOOL_UB_SANITIZER,
    UMI_CODEGUARD_TOOL_LEAK_SANITIZER
} UmiCodeGuardExternalTool;
const char *umi_codeguard_external_tool_name(UmiCodeGuardExternalTool tool);
int umi_codeguard_external_tool_command(UmiCodeGuardExternalTool tool,
                                        const char *source,
                                        char *buffer, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
