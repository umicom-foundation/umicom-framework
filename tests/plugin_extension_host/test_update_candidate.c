/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_update_candidate.c
 *
 * PURPOSE:
 *   Exercise describe one extension update candidate and compatibility/risk evidence.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/update_candidate.h"
int main(void)
{
    UmiPluginExtensionHostUpdateCandidate value; umi_plugin_extension_host_update_candidate_init(&value);
    if (umi_plugin_extension_host_update_candidate_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_update_candidate_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_update_candidate_fingerprint(&value) == 0U) return 3;
    return 0;
}
