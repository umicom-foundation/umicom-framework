/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_resize_handle.c
 *
 * PURPOSE:
 *   Validate describe resize-handle semantics without depending on a toolkit cursor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/resize_handle.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadResizeHandle item;CHECK(umi_rad_resize_handle_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_resize_handle_is_valid(&item));return 0;}
