/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/message_envelope.c
 *
 * PURPOSE:
 *   Carry bounded payload evidence separately from the canonical messaging implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/message_envelope.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric message envelope from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_message_envelope_init(UmiFabricMessageEnvelope *envelope,const UmiFabricMessageHeader *header,const void *payload,size_t payload_length,bool compressed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(envelope==NULL || header==NULL || (payload==NULL && payload_length!=0U)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(envelope,0,sizeof(*envelope)); envelope->header=*header; envelope->payload_length=(uint64_t)payload_length; envelope->payload_hash=umi_fabric_hash64(payload,payload_length,0U); envelope->compressed=compressed; return UMI_STATUS_OK;
}
/*
 * Provide the fabric message envelope matches operation used by this module and its client
 * applications.
 */
bool umi_fabric_message_envelope_matches(const UmiFabricMessageEnvelope *envelope,const void *payload,size_t payload_length) {
    return envelope!=NULL && envelope->payload_length==(uint64_t)payload_length && envelope->payload_hash==umi_fabric_hash64(payload,payload_length,0U);
}
