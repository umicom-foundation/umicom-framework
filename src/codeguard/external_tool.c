/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/external_tool.c
 *
 * PURPOSE:
 *   Build transparent external-analysis commands without hiding which compiler
 *   tool is being requested.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/external_tool.h"
#include <stdio.h>
const char *umi_codeguard_external_tool_name(UmiCodeGuardExternalTool t){switch(t){case UMI_CODEGUARD_TOOL_CLANG_ANALYZER:return "clang --analyze";case UMI_CODEGUARD_TOOL_CLANG_TIDY:return "clang-tidy";case UMI_CODEGUARD_TOOL_ADDRESS_SANITIZER:return "AddressSanitizer";case UMI_CODEGUARD_TOOL_UB_SANITIZER:return "UndefinedBehaviorSanitizer";case UMI_CODEGUARD_TOOL_LEAK_SANITIZER:return "LeakSanitizer";default:return "unknown";}}
int umi_codeguard_external_tool_command(UmiCodeGuardExternalTool t,const char *src,char *buf,size_t cap){int n;if(src==NULL||buf==NULL||cap==0U)return 0;switch(t){case UMI_CODEGUARD_TOOL_CLANG_ANALYZER:n=snprintf(buf,cap,"clang --analyze -std=c23 -Wall -Wextra %s",src);break;case UMI_CODEGUARD_TOOL_CLANG_TIDY:n=snprintf(buf,cap,"clang-tidy %s -- -std=c23",src);break;case UMI_CODEGUARD_TOOL_ADDRESS_SANITIZER:n=snprintf(buf,cap,"clang -fsanitize=address -fno-omit-frame-pointer %s",src);break;case UMI_CODEGUARD_TOOL_UB_SANITIZER:n=snprintf(buf,cap,"clang -fsanitize=undefined -fno-omit-frame-pointer %s",src);break;case UMI_CODEGUARD_TOOL_LEAK_SANITIZER:n=snprintf(buf,cap,"clang -fsanitize=leak -fno-omit-frame-pointer %s",src);break;default:return 0;}return n>0&&(size_t)n<cap;}
