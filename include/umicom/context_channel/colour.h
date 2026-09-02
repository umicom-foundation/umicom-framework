/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/colour.h
 *
 * PURPOSE:
 *   Map typed channel colours to stable theme tokens and GTK CSS classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_COLOUR_H
#define UMICOM_CONTEXT_CHANNEL_COLOUR_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context colour token data shared with callers of this public contract.
 */
typedef struct UmiContextColourToken { UmiContextChannelColour colour; char token_id[UMI_CONTEXT_TEXT_CAPACITY]; char display_name[UMI_CONTEXT_TEXT_CAPACITY]; char css_class[UMI_CONTEXT_TEXT_CAPACITY]; } UmiContextColourToken;
/**
 * Return the number of records represented by context colour token without changing their
 * state.
 */
size_t umi_context_colour_token_count(void);
/**
 * Find context colour token while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_context_colour_token_at(size_t index,UmiContextColourToken *out_token);
/**
 * Provide the context colour token for operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_colour_token_for(UmiContextChannelColour colour,UmiContextColourToken *out_token);
#ifdef __cplusplus
}
#endif
#endif
