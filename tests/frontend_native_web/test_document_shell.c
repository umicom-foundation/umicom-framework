/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_document_shell.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web document shell.
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
#include "umicom/frontend/native_web/document_shell.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebRenderContext c; UmiNativeWebMarkupBuffer d; CHECK(umi_native_web_render_context_init(&c,"s","/")==UMI_STATUS_OK); CHECK(umi_native_web_document_shell_build(&c,"Umicom","<section>ok</section>","body{}","window.x=1;",&d)==UMI_STATUS_OK); CHECK(strstr(d.data,"<!doctype html>")!=NULL); CHECK(strstr(d.data,"umicom-root")!=NULL);
    return 0;
}
