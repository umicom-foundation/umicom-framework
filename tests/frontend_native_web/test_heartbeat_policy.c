/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_heartbeat_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web heartbeat policy.
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
#include "umicom/frontend/native_web/heartbeat_policy.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebHeartbeatPolicy p={1000U,5000U}; CHECK(!umi_native_web_heartbeat_expired(&p,1000U,5000U)); CHECK(umi_native_web_heartbeat_expired(&p,1000U,7000U));
    return 0;
}
