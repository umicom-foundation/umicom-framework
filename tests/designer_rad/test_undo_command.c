/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_undo_command.c
 *
 * PURPOSE:
 *   Validate represent a reversible designer mutation without toolkit dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/undo_command.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadUndoCommand item;CHECK(umi_rad_undo_command_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_undo_command_is_valid(&item));return 0;}
