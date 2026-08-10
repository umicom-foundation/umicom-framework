/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/signature.c
 *
 * PURPOSE:
 *   Verify the built-in checksum signature scheme while retaining a provider-
 *   neutral public decision model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/signature.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
UmiStatus umi_plugin_signature_verify_checksum(const UmiPluginSignature *signature, uint64_t checksum, UmiPluginSignatureDecision *out_decision)
{
    char expected[17];
    if (signature == NULL || out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_decision->verified = 0; out_decision->reason[0] = '\0';
    if (strcmp(signature->algorithm, "fnv64") != 0) { (void)snprintf(out_decision->reason, sizeof(out_decision->reason), "unsupported signature algorithm"); return UMI_STATUS_NOT_IMPLEMENTED; }
    (void)snprintf(expected, sizeof(expected), "%016llx", (unsigned long long)checksum);
    out_decision->verified = strcmp(signature->value, expected) == 0;
    (void)snprintf(out_decision->reason, sizeof(out_decision->reason), "%s", out_decision->verified ? "verified" : "signature mismatch");
    return out_decision->verified ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}
