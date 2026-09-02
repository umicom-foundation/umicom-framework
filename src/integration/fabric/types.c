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


/* Provide the fabric copy text operation used by this module and its client applications. */
UmiStatus umi_fabric_copy_text(char *destination,size_t capacity,const char *source) {
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination==NULL || source==NULL || capacity==0U) return UMI_STATUS_INVALID_ARGUMENT;
    length=strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination,source,length+1U);
    return UMI_STATUS_OK;
}
/* Provide the fabric hash64 operation used by this module and its client applications. */
uint64_t umi_fabric_hash64(const void *data,size_t size,uint64_t seed) {
    const unsigned char *bytes=(const unsigned char *)data;
    uint64_t hash=seed!=0U?seed:UINT64_C(1469598103934665603);
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data==NULL && size!=0U) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<size;++i) { hash^=(uint64_t)bytes[i]; hash*=UINT64_C(1099511628211); }
    return hash;
}
/*
 * Provide the fabric version compare operation used by this module and its client
 * applications.
 */
int umi_fabric_version_compare(UmiFabricVersion left,UmiFabricVersion right) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.major!=right.major) return left.major<right.major?-1:1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.minor!=right.minor) return left.minor<right.minor?-1:1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.patch!=right.patch) return left.patch<right.patch?-1:1;
    return 0;
}
/* Provide the fabric state text operation used by this module and its client applications. */
const char *umi_fabric_state_text(UmiFabricState state) {
    /* Select the behaviour associated with the requested command or state value. */
    switch(state) {
        case UMI_FABRIC_STATE_READY:return "ready";
        case UMI_FABRIC_STATE_DEGRADED:return "degraded";
        case UMI_FABRIC_STATE_BLOCKED:return "blocked";
        case UMI_FABRIC_STATE_FAILED:return "failed";
        case UMI_FABRIC_STATE_UNKNOWN:default:return "unknown";
    }
}
