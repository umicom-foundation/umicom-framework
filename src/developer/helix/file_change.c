/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/file_change.c
 *
 * PURPOSE:
 *   Represent a bounded file-level add, modify, delete or rename operation with content fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/file_change.h"
#include <string.h>

/* Initialise stable API metadata and conservative default policy/evidence. */
void umi_helix_file_change_init(UmiHelixFileChange *record,const char *id){
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
    record->risk=0.0;
}
/* Reject malformed or future-incompatible records before orchestration consumes them. */
UmiStatus umi_helix_file_change_validate(const UmiHelixFileChange *record){
    if(record==NULL || record->structure_size!=sizeof(*record) || record->api_version!=UMI_HELIX_API_VERSION || !umi_helix_valid_id(record->id)){
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
/* Reduce this record to one deterministic normalised score without executing side effects. */
double umi_helix_file_change_score(const UmiHelixFileChange *record){
    if(umi_helix_file_change_validate(record)!=UMI_STATUS_OK){
        return 0.0;
    }
    return 1.0-umi_helix_clamp_score(record->risk);
}
