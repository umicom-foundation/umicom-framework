/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/quarantine_record.c
 *
 * PURPOSE:
 *   Capture quarantine state, reason and recovery eligibility.
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
#include "umicom/plugin/extension_host/quarantine_record.h"
#include <string.h>

/*
 * Copy plugin extension host quarantine record into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_quarantine_record_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

/*
 * Initialise plugin extension host quarantine record from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_quarantine_record_init(UmiPluginExtensionHostQuarantineRecord *r) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL) memset(r,0,sizeof(*r)); }
/*
 * Provide the plugin extension host quarantine record enter operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_quarantine_record_enter(UmiPluginExtensionHostQuarantineRecord *r,const char *id,const char *reason,uint64_t since,uint64_t evidence,int recovery) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||!umi_plugin_extension_host_text_valid(id,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)||reason==NULL||reason[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; umi_plugin_extension_host_quarantine_record_init(r); umi_plugin_extension_host_quarantine_record_copy(r->plugin_id,sizeof(r->plugin_id),id); umi_plugin_extension_host_quarantine_record_copy(r->reason,sizeof(r->reason),reason); r->since_ms=since; r->evidence=evidence; r->active=1; r->recovery_allowed=recovery?1:0; return UMI_STATUS_OK; }
/*
 * Release or reset state held by plugin extension host quarantine record so the same
 * storage can be reused safely.
 */
UmiStatus umi_plugin_extension_host_quarantine_record_clear(UmiPluginExtensionHostQuarantineRecord *r) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!r->active) return UMI_STATUS_INVALID_STATE; r->active=0; return UMI_STATUS_OK; }
