/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/declarative_emitter.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DECLARATIVE_EMITTER_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DECLARATIVE_EMITTER_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer declarative emitter data shared with callers of this public contract.
 */
typedef struct UmiRadDeclarativeEmitter { char buffer[UMI_RAD_EMIT_CAPACITY]; size_t length; } UmiRadDeclarativeEmitter;
/**
 * Initialise visual designer declarative emitter from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_declarative_emitter_init(UmiRadDeclarativeEmitter *emitter);
/**
 * Add visual designer declarative emitter only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_rad_declarative_emitter_append(UmiRadDeclarativeEmitter *emitter,const char *text);
/**
 * Provide the visual designer declarative emitter text operation used by this module and its client
 * applications.
 */
const char *umi_rad_declarative_emitter_text(const UmiRadDeclarativeEmitter *emitter);
#ifdef __cplusplus
}
#endif
#endif
