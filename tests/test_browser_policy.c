/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_browser_policy.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include "umicom/umicom.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiBrowserPolicy p=umi_browser_policy_default();assert(umi_browser_policy_check(&p,"https://example.com")==UMI_STATUS_OK);assert(umi_browser_policy_check(&p,"http://example.com")==UMI_STATUS_PERMISSION_DENIED);return 0;}
