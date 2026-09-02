/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/recovery_policy.c
 *
 * PURPOSE:
 *   Define bounded restart, backoff and checkpoint requirements after extension failure.
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
#include "umicom/plugin/extension_host/recovery_policy.h"
/*
 * Initialise plugin extension host recovery policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_recovery_policy_init(UmiPluginExtensionHostRecoveryPolicy *p) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL) { p->maximum_restarts=4U; p->base_backoff_ms=500U; p->maximum_backoff_ms=30000U; p->require_checkpoint=1; p->require_trust_revalidation=1; } }
/*
 * Provide the plugin extension host recovery policy evaluate operation used by this module
 * and its client applications.
 */
UmiPluginExtensionHostDecision umi_plugin_extension_host_recovery_policy_evaluate(const UmiPluginExtensionHostRecoveryPolicy *p,uint32_t restarts,int checkpoint,int trust) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||restarts>=p->maximum_restarts||(p->require_trust_revalidation&&!trust)) return UMI_PLUGIN_EXTENSION_HOST_DENY; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->require_checkpoint&&!checkpoint) return UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL; return UMI_PLUGIN_EXTENSION_HOST_ALLOW; }
/*
 * Provide the plugin extension host recovery policy backoff ms operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_recovery_policy_backoff_ms(const UmiPluginExtensionHostRecoveryPolicy *p,uint32_t restarts) { uint64_t d; uint32_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL) return 0U; d=p->base_backoff_ms; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<restarts&&d<p->maximum_backoff_ms;++i) d=d>p->maximum_backoff_ms/2U?p->maximum_backoff_ms:d*2U; return d>p->maximum_backoff_ms?p->maximum_backoff_ms:d; }
