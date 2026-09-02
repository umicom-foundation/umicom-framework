/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEditorIntelEntry entry;UmiEditorIntelRange range={{5U,9U},{2U,1U}};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_entry_init(&entry,"entry-1","main","src/main.c",range)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_entry_valid(&entry))return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(entry.location.range.start.line!=2U||entry.location.range.end.line!=5U)return 3;return 0;}
