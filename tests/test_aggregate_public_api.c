/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_aggregate_public_api.c
 *
 * PURPOSE:
 *   Guard the aggregate umicom/umicom.h contract so established Web, browser,
 *   chart, frontend, Designer and Developer APIs remain visible to consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiStatus (*browser_create)(UmiBrowserEventSink, void *, UmiBrowser *) =
        umi_browser_headless_create;
    UmiStatus (*json_object)(const char *, const char *, char *, size_t) =
        umi_web_json_object1;
    UmiStatus (*rest_json)(UmiWebResponse *, int, const char *) =
        umi_web_rest_json;
    UmiStatus (*runtime_process)(UmiWebService *, const char *, size_t,
                                 char *, size_t, size_t *) =
        umi_web_runtime_process;

    return browser_create == NULL || json_object == NULL ||
           rest_json == NULL || runtime_process == NULL;
}
