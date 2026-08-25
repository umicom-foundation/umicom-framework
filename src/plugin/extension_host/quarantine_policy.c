/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/quarantine_policy.c
 *
 * PURPOSE:
 *   Determine when repeated crashes, trust failures or signature failures require quarantine.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/quarantine_policy.h"
void umi_plugin_extension_host_quarantine_policy_init(UmiPluginExtensionHostQuarantinePolicy *p) { if(p!=NULL) { p->crash_threshold=3U; p->trust_failure_threshold=1U; p->signature_failure_threshold=1U; p->policy_failure_threshold=2U; } }
int umi_plugin_extension_host_quarantine_policy_should_quarantine(const UmiPluginExtensionHostQuarantinePolicy *p,uint32_t c,uint32_t t,uint32_t s,uint32_t f) { return p!=NULL&&(c>=p->crash_threshold||t>=p->trust_failure_threshold||s>=p->signature_failure_threshold||f>=p->policy_failure_threshold); }
