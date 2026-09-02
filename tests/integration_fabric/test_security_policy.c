/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_security_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the security policy Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/security_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricSecurityPolicy p; bool ok=false; umi_fabric_security_policy_default(&p); CHECK(umi_fabric_security_policy_evaluate(&p,true,true,"tenant",false,&ok)==UMI_STATUS_OK); CHECK(ok); CHECK(umi_fabric_security_policy_evaluate(&p,false,true,"tenant",false,&ok)==UMI_STATUS_OK); CHECK(!ok);
    return 0;
}
