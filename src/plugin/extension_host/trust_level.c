/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/trust_level.c
 *
 * PURPOSE:
 *   Calculate extension trust from signature, publisher and provenance evidence.
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
#include "umicom/plugin/extension_host/trust_level.h"
/*
 * Provide the plugin extension host trust level score operation used by this module and
 * its client applications.
 */
uint32_t umi_plugin_extension_host_trust_level_score(const UmiPluginExtensionHostTrustLevelEvidence *e) { int score; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||e->risk>100U) return 0U; score=20; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->signature_valid) score+=30; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->publisher_known) score+=20; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->checksum_valid) score+=15; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->provenance_complete) score+=15; score-=(int)(e->risk/4U); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(score<0) score=0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(score>100) score=100; return (uint32_t)score; }
/*
 * Provide the plugin extension host trust level classify operation used by this module and
 * its client applications.
 */
UmiPluginExtensionHostTrust umi_plugin_extension_host_trust_level_classify(const UmiPluginExtensionHostTrustLevelEvidence *e) { uint32_t s=umi_plugin_extension_host_trust_level_score(e); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s>=90U) return UMI_PLUGIN_EXTENSION_HOST_TRUST_SYSTEM; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s>=70U) return UMI_PLUGIN_EXTENSION_HOST_TRUST_TRUSTED; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s>=40U) return UMI_PLUGIN_EXTENSION_HOST_TRUST_UNKNOWN; return UMI_PLUGIN_EXTENSION_HOST_TRUST_UNTRUSTED; }
