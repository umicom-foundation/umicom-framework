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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/dependency_resolution.h"
#include <string.h>
void umi_plugin_extension_host_dependency_resolution_init(UmiPluginExtensionHostDependencyResolution *r) { if(r!=NULL) memset(r,0,sizeof(*r)); }
void umi_plugin_extension_host_dependency_resolution_record(UmiPluginExtensionHostDependencyResolution *r,int required,int present,int compatible) { if(r==NULL) return; if(required) { ++r->required; if(present&&compatible) ++r->satisfied; else if(present&&!compatible) ++r->incompatible; } else if(!present) ++r->optional_missing; }
int umi_plugin_extension_host_dependency_resolution_ready(const UmiPluginExtensionHostDependencyResolution *r) { return r!=NULL && r->required==r->satisfied && r->incompatible==0U; }
uint32_t umi_plugin_extension_host_dependency_resolution_missing_required(const UmiPluginExtensionHostDependencyResolution *r) { return r==NULL?0U:(r->required-r->satisfied); }
