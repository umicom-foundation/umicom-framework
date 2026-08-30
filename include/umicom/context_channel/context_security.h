/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_security.h
 *
 * PURPOSE:
 *   Record security decisions for context publication and cross-application sharing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_SECURITY_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_SECURITY_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextSecurity {
    uint32_t structure_size;
    char decision_id[UMI_CONTEXT_VALUE_CAPACITY];
    char subject_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextSecurity;
void umi_context_security_init(UmiContextSecurity *state);
UmiStatus umi_context_security_set_field(UmiContextSecurity *state,size_t field_index,const char *value);
const char *umi_context_security_field(const UmiContextSecurity *state,size_t field_index);
UmiStatus umi_context_security_record_success(UmiContextSecurity *state,uint64_t sequence);
UmiStatus umi_context_security_record_failure(UmiContextSecurity *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_security_validate(const UmiContextSecurity *state);
bool umi_context_security_covers_sequence(const UmiContextSecurity *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
