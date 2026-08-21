/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/colour.c
 *
 * PURPOSE:
 *   Map context colours to semantic resource tokens and CSS classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
size_t umi_context_colour_token_count(void){
return sizeof(tokens)/sizeof(tokens[0]);
}
UmiStatus umi_context_colour_token_at(size_t index,UmiContextColourToken *out){
if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(index>=umi_context_colour_token_count())return UMI_STATUS_NOT_FOUND;
*out=tokens[index];
return UMI_STATUS_OK;
}
UmiStatus umi_context_colour_token_for(UmiContextChannelColour colour,UmiContextColourToken *out){
size_t i;
if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<umi_context_colour_token_count();++i)if(tokens[i].colour==colour){
*out=tokens[i];
return UMI_STATUS_OK;
}
return UMI_STATUS_NOT_FOUND;
}
