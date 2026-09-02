/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_undo_command.c
 *
 * PURPOSE:
 *   Validate represent a reversible designer mutation without toolkit dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/undo_command.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadUndoCommand item;CHECK(umi_rad_undo_command_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_undo_command_is_valid(&item));return 0;}
