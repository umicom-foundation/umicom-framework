/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/lineage_record.c
 *
 * PURPOSE:
 *   Record parent/child variant lineage and transformation reason for reproducible evolution history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/lineage_record.h"
#include <string.h>

/* Initialise stable API metadata and conservative default policy/evidence. */
void umi_helix_lineage_record_init(UmiHelixLineageRecord *record,const char *id){
    if(record==NULL){
        return;
    }
    memset(record,0,sizeof(*record));
    record->structure_size=(uint32_t)sizeof(*record);
    record->api_version=UMI_HELIX_API_VERSION;
    record->state=UMI_HELIX_IDLE;
    if(id!=NULL){
        (void)umi_helix_copy_text(record->id,sizeof(record->id),id);
    }
    record->confidence=0.0;
}
/* Reject malformed or future-incompatible records before orchestration consumes them. */
UmiStatus umi_helix_lineage_record_validate(const UmiHelixLineageRecord *record){
    if(record==NULL || record->structure_size!=sizeof(*record) || record->api_version!=UMI_HELIX_API_VERSION || !umi_helix_valid_id(record->id)){
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
/* Reduce this record to one deterministic normalised score without executing side effects. */
double umi_helix_lineage_record_score(const UmiHelixLineageRecord *record){
    if(umi_helix_lineage_record_validate(record)!=UMI_STATUS_OK){
        return 0.0;
    }
    return umi_helix_clamp_score(record->confidence);
}
