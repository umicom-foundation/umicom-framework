/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_command_internal.h
 *
 * PURPOSE:
 *   Share bounded unsigned page-option parsing between native ComfyUI observation commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_COMFY_COMMAND_INTERNAL_H
#define UMICOM_COMFY_COMMAND_INTERNAL_H
#include <stdbool.h>
#include <stddef.h>
/* Locale-independent decimal values never enter a shell. Retain exact maximum
 * checks before multiplication so arbitrarily long input cannot wrap. */
static inline bool UmiComfyCommandNumber(const char *text,size_t maximum,size_t *out)
{
    if(text==NULL || out==NULL || *text=='\0')return false;
    size_t value=0U;
    for(const unsigned char *p=(const unsigned char *)text;*p!='\0';++p){
        if(*p<'0' || *p>'9')return false;
        size_t digit=(size_t)(*p-'0');
        if(value>maximum/10U || (value==maximum/10U && digit>maximum%10U))return false;
        value=value*10U+digit;
    }
    *out=value;return true;
}
#endif
