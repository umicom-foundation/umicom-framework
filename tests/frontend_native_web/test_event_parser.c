/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_event_parser.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web event parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/event_parser.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebUiEvent e; CHECK(umi_native_web_event_parse("command|build|id|Build",7U,&e)==UMI_STATUS_OK); CHECK(e.kind==UMI_NATIVE_WEB_EVENT_COMMAND); CHECK(strcmp(e.target_id,"build")==0);
    return 0;
}
