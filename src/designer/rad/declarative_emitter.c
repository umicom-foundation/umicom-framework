/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/declarative_emitter.c
 *
 * PURPOSE:
 *   Build inspectable declarative application text into a bounded buffer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/declarative_emitter.h"
#include <string.h>
UmiStatus umi_rad_declarative_emitter_init(UmiRadDeclarativeEmitter *e){if(e==NULL)return UMI_STATUS_INVALID_ARGUMENT;e->buffer[0]='\0';e->length=0U;return UMI_STATUS_OK;}
UmiStatus umi_rad_declarative_emitter_append(UmiRadDeclarativeEmitter *e,const char *t){size_t n;if(e==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(t);if(n>UMI_RAD_EMIT_CAPACITY-e->length-1U)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(e->buffer+e->length,t,n+1U);e->length+=n;return UMI_STATUS_OK;}
const char *umi_rad_declarative_emitter_text(const UmiRadDeclarativeEmitter *e){return e==NULL?NULL:e->buffer;}
