/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_secret_inventory.c
 *
 * PURPOSE:
 *   Implement the test security secret inventory behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Secret inventory v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/security/secret_inventory.h"
int main(void)
{
    UmiSecuritySecretInventory inventory = {0};
    UmiSecuritySecretMetadata metadata = {0};
    (void)snprintf(metadata.secret.reference,sizeof(metadata.secret.reference),"vault://broker/ibkr");
    (void)snprintf(metadata.purpose,sizeof(metadata.purpose),"Brokerage execution adapter credential");
    metadata.rotation_due_ns = 100U; metadata.enabled = true;
    assert(umi_security_secret_inventory_add(&inventory,&metadata) == UMI_STATUS_OK);
    assert(strcmp(inventory.items[0].secret.provider,"vault") == 0);
    assert(strcmp(inventory.items[0].secret.name,"broker/ibkr") == 0);
    assert(umi_security_secret_rotation_due(&inventory.items[0],100U));
    assert(strstr((const char *)&inventory.items[0],"password") == NULL);
    return 0;
}
