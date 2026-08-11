/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_browser_policy.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
int main(void){UmiBrowserPolicy p=umi_browser_policy_default();assert(umi_browser_policy_check(&p,"https://example.com")==UMI_STATUS_OK);assert(umi_browser_policy_check(&p,"http://example.com")==UMI_STATUS_PERMISSION_DENIED);return 0;}
