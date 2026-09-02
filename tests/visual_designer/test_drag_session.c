/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_drag_session.c
 *
 * PURPOSE:
 *   Validate track a visual component drag operation from press through commit/cancel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/drag_session.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDragSession item;CHECK(umi_rad_drag_session_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_drag_session_is_valid(&item));return 0;}
