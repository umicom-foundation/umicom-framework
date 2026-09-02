/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/crash_record.c
 *
 * PURPOSE:
 *   Capture one extension crash with deterministic failure evidence.
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
#include "umicom/plugin/extension_host/crash_record.h"
#include <string.h>

/*
 * Copy plugin extension host crash record into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_crash_record_copy(char *destination, size_t capacity, const char *source)
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
 * Initialise plugin extension host crash record from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_crash_record_init(UmiPluginExtensionHostCrashRecord *r) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL) memset(r,0,sizeof(*r)); }
/*
 * Provide the plugin extension host crash record capture operation used by this module and
 * its client applications.
 */
UmiStatus umi_plugin_extension_host_crash_record_capture(UmiPluginExtensionHostCrashRecord *r,const char *id,uint64_t ts,int32_t code,uint32_t sig,uint32_t restarts) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||!umi_plugin_extension_host_text_valid(id,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT; umi_plugin_extension_host_crash_record_init(r); umi_plugin_extension_host_crash_record_copy(r->plugin_id,sizeof(r->plugin_id),id); r->timestamp_ms=ts; r->exit_code=code; r->signal_code=sig; r->restart_count=restarts; r->evidence=umi_plugin_extension_host_hash_text(id)^ts^(uint64_t)(uint32_t)code^((uint64_t)sig<<32U); return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host crash record severe operation used by this module and
 * its client applications.
 */
int umi_plugin_extension_host_crash_record_severe(const UmiPluginExtensionHostCrashRecord *r) { return r!=NULL&&(r->signal_code!=0U||r->restart_count>=3U||r->exit_code<0); }
