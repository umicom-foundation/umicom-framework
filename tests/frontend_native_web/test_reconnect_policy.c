/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_reconnect_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web reconnect policy.
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
#include "umicom/frontend/native_web/reconnect_policy.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebReconnectPolicy p={5U,100U,1000U}; CHECK(umi_native_web_reconnect_delay(&p,0U)==100U); CHECK(umi_native_web_reconnect_delay(&p,3U)==800U); CHECK(umi_native_web_reconnect_delay(&p,5U)==0U);
    return 0;
}
