/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/types.c
 *
 * PURPOSE:
 *   Define shared Helix constants, lifecycle states, governance decisions and deterministic utility helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/types.h"
#include <string.h>

/* Copy a complete string and reject silent truncation. */
UmiStatus umi_helix_copy_text(char *destination,size_t capacity,const char *source){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(destination==NULL || capacity==0U || source==NULL){ return UMI_STATUS_INVALID_ARGUMENT; }
    size_t n=strlen(source); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>=capacity){ return UMI_STATUS_CAPACITY_EXCEEDED; }
    memcpy(destination,source,n+1U); return UMI_STATUS_OK;
}
/* Keep identifiers printable, bounded and stable across serialization boundaries. */
bool umi_helix_valid_id(const char *id){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id==NULL || id[0]=='\0'){ return false; }
    size_t n=strlen(id);
    return n<UMI_HELIX_ID_CAPACITY;
}
/* Normalise scores before they enter ranking or policy decisions. */
double umi_helix_clamp_score(double value){ /* Apply this branch only when its contract condition is satisfied. */ if(value<0.0){ return 0.0; } /* Apply this branch only when its contract condition is satisfied. */ if(value>1.0){ return 1.0; } return value; }
/* Protect policy calculations from divide-by-zero evidence. */
double umi_helix_safe_ratio(double numerator,double denominator){ return denominator==0.0?0.0:numerator/denominator; }
/* Fingerprint text deterministically without platform-specific hashing dependencies. */
uint64_t umi_helix_hash_text(const char *text){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(text==NULL){ return 0U; }
    uint64_t h=1469598103934665603ULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(const unsigned char *p=(const unsigned char*)text;*p!=0U;++p){ h^=(uint64_t)(*p); h*=1099511628211ULL; } return h;
}
/* Convert normalised risk to a conservative severity tier. */
UmiHelixSeverity umi_helix_severity_from_risk(double risk){
    double r=umi_helix_clamp_score(risk); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r>=0.85){ return UMI_HELIX_CRITICAL; } /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r>=0.60){ return UMI_HELIX_ERROR; } /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r>=0.30){ return UMI_HELIX_WARNING; } return UMI_HELIX_INFO;
}
/* Return stable state text for logs, reports and frontends. */
const char *umi_helix_state_text(UmiHelixState state){
    /* Select the behaviour associated with the requested command or state value. */
    switch(state){case UMI_HELIX_IDLE:return "idle";case UMI_HELIX_PLANNED:return "planned";case UMI_HELIX_READY:return "ready";case UMI_HELIX_RUNNING:return "running";case UMI_HELIX_BLOCKED:return "blocked";case UMI_HELIX_SUCCEEDED:return "succeeded";case UMI_HELIX_FAILED:return "failed";case UMI_HELIX_ROLLED_BACK:return "rolled-back";case UMI_HELIX_CANCELLED:return "cancelled";default:return "unknown";}
}
