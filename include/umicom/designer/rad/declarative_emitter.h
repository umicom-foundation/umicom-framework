/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/declarative_emitter.h
 *
 * PURPOSE:
 *   Build inspectable declarative application text into a bounded buffer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DECLARATIVE_EMITTER_H
#define UMICOM_DESIGNER_RAD_DECLARATIVE_EMITTER_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDeclarativeEmitter { char buffer[UMI_RAD_EMIT_CAPACITY]; size_t length; } UmiRadDeclarativeEmitter;
UmiStatus umi_rad_declarative_emitter_init(UmiRadDeclarativeEmitter *emitter);
UmiStatus umi_rad_declarative_emitter_append(UmiRadDeclarativeEmitter *emitter,const char *text);
const char *umi_rad_declarative_emitter_text(const UmiRadDeclarativeEmitter *emitter);
#ifdef __cplusplus
}
#endif
#endif
