/* Umicom Framework debugger adapter profile test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/debug/adapter_profile.h"
int main(void){UmiDebugAdapterProfileRegistry*r=NULL;UmiDebugAdapterProfile p={0},o;strcpy(p.id,"gdb");strcpy(p.executable,"gdb");p.supports_launch=1;assert(umi_debug_adapter_profile_registry_create(&r)==UMI_STATUS_OK);assert(umi_debug_adapter_profile_registry_upsert(r,&p)==UMI_STATUS_OK);assert(umi_debug_adapter_profile_registry_find(r,"gdb",&o)==UMI_STATUS_OK&&o.supports_launch);umi_debug_adapter_profile_registry_destroy(r);return 0;}
