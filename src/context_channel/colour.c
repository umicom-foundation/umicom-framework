/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/colour.c
 *
 * PURPOSE:
 *   Map context colours to semantic resource tokens and CSS classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/colour.h"
#include <string.h>
static const UmiContextColourToken tokens[] = {
 {
UMI_CONTEXT_COLOUR_RED,"umicom.context.colour.red","Red","umicom-context-red"
},
 {
UMI_CONTEXT_COLOUR_ORANGE,"umicom.context.colour.orange","Orange","umicom-context-orange"
},
 {
UMI_CONTEXT_COLOUR_YELLOW,"umicom.context.colour.yellow","Yellow","umicom-context-yellow"
},
 {
UMI_CONTEXT_COLOUR_GREEN,"umicom.context.colour.green","Green","umicom-context-green"
},
 {
UMI_CONTEXT_COLOUR_CYAN,"umicom.context.colour.cyan","Cyan","umicom-context-cyan"
},
 {
UMI_CONTEXT_COLOUR_BLUE,"umicom.context.colour.blue","Blue","umicom-context-blue"
},
 {
UMI_CONTEXT_COLOUR_PURPLE,"umicom.context.colour.purple","Purple","umicom-context-purple"
},
 {
UMI_CONTEXT_COLOUR_MAGENTA,"umicom.context.colour.magenta","Magenta","umicom-context-magenta"
}
};
/*
 * Return the number of records represented by context colour token without changing their
 * state.
 */
size_t umi_context_colour_token_count(void){
return sizeof(tokens)/sizeof(tokens[0]);
}
/*
 * Find context colour token while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_context_colour_token_at(size_t index,UmiContextColourToken *out){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(index>=umi_context_colour_token_count())return UMI_STATUS_NOT_FOUND;
*out=tokens[index];
return UMI_STATUS_OK;
}
/*
 * Provide the context colour token for operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_colour_token_for(UmiContextChannelColour colour,UmiContextColourToken *out){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<umi_context_colour_token_count();++i)/* Preserve the original failure result so the caller can respond to the correct cause. */ if(tokens[i].colour==colour){
*out=tokens[i];
return UMI_STATUS_OK;
}
return UMI_STATUS_NOT_FOUND;
}
