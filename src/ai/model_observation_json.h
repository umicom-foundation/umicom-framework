/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/model_observation_json.h
 *
 * PURPOSE:
 *   Share exact optional JSON field admission between model lists and per-model observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_MODEL_OBSERVATION_JSON_H
#define UMICOM_AI_MODEL_OBSERVATION_JSON_H
#include "umicom/language_runtime/json_tree.h"
/* The destination starts initialized by its owning decoder. Missing optional
 * fields leave that value untouched; present fields retain their exact type. */
static inline UmiStatus UmiAiObservationText(const UmiJsonTree *tree,int object,
    const char *name,int required,char *out,size_t capacity)
{
    int node=-1;UmiStatus status=UmiJsonTreeMember(tree,object,name,&node);
    if(status==UMI_STATUS_NOT_FOUND)return required?UMI_STATUS_PARSE_ERROR:UMI_STATUS_OK;
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_PARSE_ERROR;
    status=UmiJsonTreeText(tree,node,out,capacity);
    if(status==UMI_STATUS_OK && required && out[0]=='\0')return UMI_STATUS_PARSE_ERROR;
    return status;
}
static inline UmiStatus UmiAiObservationUnsigned(const UmiJsonTree *tree,int object,
    const char *name,uint64_t *out,uint32_t bit,uint32_t *known)
{
    int node=-1;UmiStatus status=UmiJsonTreeMember(tree,object,name,&node);
    if(status==UMI_STATUS_NOT_FOUND)return UMI_STATUS_OK;
    if(status!=UMI_STATUS_OK)return status;
    int64_t value=0;status=UmiJsonTreeInteger(tree,node,&value);
    if(status!=UMI_STATUS_OK)return status;
    if(value<0)return UMI_STATUS_PARSE_ERROR;
    *out=(uint64_t)value;*known|=bit;return UMI_STATUS_OK;
}
#endif
