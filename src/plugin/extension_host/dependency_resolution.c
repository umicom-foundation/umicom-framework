/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/dependency_resolution.c
 *
 * PURPOSE:
 *   Evaluate whether required extension dependencies are present and compatible.
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
#include "umicom/plugin/extension_host/dependency_resolution.h"
#include <string.h>
/*
 * Initialise plugin extension host dependency resolution from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_dependency_resolution_init(UmiPluginExtensionHostDependencyResolution *r) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL) memset(r,0,sizeof(*r)); }
/*
 * Provide the plugin extension host dependency resolution record operation used by this
 * module and its client applications.
 */
void umi_plugin_extension_host_dependency_resolution_record(UmiPluginExtensionHostDependencyResolution *r,int required,int present,int compatible) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL) return; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(required) { ++r->required; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(present&&compatible) ++r->satisfied; else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(present&&!compatible) ++r->incompatible; } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!present) ++r->optional_missing; }
/*
 * Provide the plugin extension host dependency resolution ready operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_dependency_resolution_ready(const UmiPluginExtensionHostDependencyResolution *r) { return r!=NULL && r->required==r->satisfied && r->incompatible==0U; }
/*
 * Provide the plugin extension host dependency resolution missing required operation used
 * by this module and its client applications.
 */
uint32_t umi_plugin_extension_host_dependency_resolution_missing_required(const UmiPluginExtensionHostDependencyResolution *r) { return r==NULL?0U:(r->required-r->satisfied); }
