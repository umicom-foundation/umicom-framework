/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_undo_stack.c
 *
 * PURPOSE:
 *   Validate maintain deterministic undo/redo history for visual authoring operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/undo_stack.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadUndoStack s;UmiRadUndoCommand c;CHECK(umi_rad_undo_stack_init(&s)==UMI_STATUS_OK);CHECK(umi_rad_undo_command_init(&c)==UMI_STATUS_OK);CHECK(umi_rad_undo_stack_push(&s,&c)==UMI_STATUS_OK);CHECK(umi_rad_undo_stack_undo(&s)!=0);CHECK(umi_rad_undo_stack_redo(&s)!=0);return 0;}
