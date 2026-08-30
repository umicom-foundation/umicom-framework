/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/types.c
 *
 * PURPOSE:
 *   Define stable C23 capacities, states, protocols and utility helpers shared by the enterprise Integration Fabric.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/types.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_copy_text(char *destination,size_t capacity,const char *source) {
    size_t length;
    if (destination==NULL || source==NULL || capacity==0U) return UMI_STATUS_INVALID_ARGUMENT;
    length=strlen(source);
    if (length>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination,source,length+1U);
    return UMI_STATUS_OK;
}
uint64_t umi_fabric_hash64(const void *data,size_t size,uint64_t seed) {
    const unsigned char *bytes=(const unsigned char *)data;
    uint64_t hash=seed!=0U?seed:UINT64_C(1469598103934665603);
    size_t i;
    if (data==NULL && size!=0U) return 0U;
    for (i=0U;i<size;++i) { hash^=(uint64_t)bytes[i]; hash*=UINT64_C(1099511628211); }
    return hash;
}
int umi_fabric_version_compare(UmiFabricVersion left,UmiFabricVersion right) {
    if (left.major!=right.major) return left.major<right.major?-1:1;
    if (left.minor!=right.minor) return left.minor<right.minor?-1:1;
    if (left.patch!=right.patch) return left.patch<right.patch?-1:1;
    return 0;
}
const char *umi_fabric_state_text(UmiFabricState state) {
    switch(state) {
        case UMI_FABRIC_STATE_READY:return "ready";
        case UMI_FABRIC_STATE_DEGRADED:return "degraded";
        case UMI_FABRIC_STATE_BLOCKED:return "blocked";
        case UMI_FABRIC_STATE_FAILED:return "failed";
        case UMI_FABRIC_STATE_UNKNOWN:default:return "unknown";
    }
}
