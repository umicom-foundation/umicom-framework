/* Umicom Framework Tests | Structured redaction v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/security/redaction_policy.h"
int main(void)
{
    UmiSecurityRedactionPolicy policy;
    char output[64U];
    umi_security_redaction_policy_default(&policy);
    assert(umi_security_redaction_copy(&policy,"api_key","sensitive-value",output,sizeof(output)) == UMI_STATUS_OK);
    assert(strcmp(output,"[REDACTED]") == 0);
    assert(umi_security_redaction_copy(&policy,"component","builder",output,sizeof(output)) == UMI_STATUS_OK);
    assert(strcmp(output,"builder") == 0);
    return 0;
}
