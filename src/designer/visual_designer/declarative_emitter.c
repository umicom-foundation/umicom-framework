/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/declarative_emitter.c
 *
 * PURPOSE:
 *   Build inspectable declarative application text into a bounded buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/declarative_emitter.h"
#include <string.h>
/*
 * Initialise visual designer declarative emitter from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_declarative_emitter_init(UmiRadDeclarativeEmitter *e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL)return UMI_STATUS_INVALID_ARGUMENT;e->buffer[0]='\0';e->length=0U;return UMI_STATUS_OK;}
/*
 * Add visual designer declarative emitter only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_rad_declarative_emitter_append(UmiRadDeclarativeEmitter *e,const char *t){size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(t);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n>UMI_RAD_EMIT_CAPACITY-e->length-1U)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(e->buffer+e->length,t,n+1U);e->length+=n;return UMI_STATUS_OK;}
/*
 * Provide the visual designer declarative emitter text operation used by this module and its client
 * applications.
 */
const char *umi_rad_declarative_emitter_text(const UmiRadDeclarativeEmitter *e){return e==NULL?NULL:e->buffer;}
