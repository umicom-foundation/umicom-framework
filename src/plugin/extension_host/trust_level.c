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
uint32_t umi_plugin_extension_host_trust_level_score(const UmiPluginExtensionHostTrustLevelEvidence *e) { int score; if(e==NULL||e->risk>100U) return 0U; score=20; if(e->signature_valid) score+=30; if(e->publisher_known) score+=20; if(e->checksum_valid) score+=15; if(e->provenance_complete) score+=15; score-=(int)(e->risk/4U); if(score<0) score=0; if(score>100) score=100; return (uint32_t)score; }
UmiPluginExtensionHostTrust umi_plugin_extension_host_trust_level_classify(const UmiPluginExtensionHostTrustLevelEvidence *e) { uint32_t s=umi_plugin_extension_host_trust_level_score(e); if(s>=90U) return UMI_PLUGIN_EXTENSION_HOST_TRUST_SYSTEM; if(s>=70U) return UMI_PLUGIN_EXTENSION_HOST_TRUST_TRUSTED; if(s>=40U) return UMI_PLUGIN_EXTENSION_HOST_TRUST_UNKNOWN; return UMI_PLUGIN_EXTENSION_HOST_TRUST_UNTRUSTED; }
