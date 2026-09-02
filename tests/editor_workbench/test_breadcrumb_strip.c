/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_breadcrumb_strip.c
 *
 * PURPOSE:
 *   Implement the test breadcrumb strip behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/breadcrumb_strip.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbBreadcrumbStrip s; umi_editor_wb_breadcrumb_strip_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_breadcrumb_strip_append(&s,"one",1U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_breadcrumb_strip_append(&s,"two",2U)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_breadcrumb_strip_activate(&s,1U)!=UMI_STATUS_OK||s.active_index!=1U)return 3; return 0; }
