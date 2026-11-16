/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/ascii_search_internal.h
 *
 * PURPOSE:
 *   Share deterministic ASCII-folded substring matching for already validated owned text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_ASCII_SEARCH_INTERNAL_H
#define UMICOM_ASCII_SEARCH_INTERNAL_H
#include <stddef.h>
/* Private helper: both inputs are readable terminated byte strings supplied
 * by validated owners. This changes ASCII letters only, avoiding process-wide
 * locale state and preserving the callers' explicit Unicode matching policy. */
static inline unsigned char UmiAsciiSearchFold(unsigned char value)
{
    return value>='A' && value<='Z' ? (unsigned char)(value+('a'-'A')) : value;
}
static inline int UmiAsciiSearchContains(const char *text,const char *needle)
{
    if(needle[0]=='\0')return 1;
    for(size_t start=0U;text[start]!='\0';++start){
        size_t i=0U;
        while(needle[i]!='\0' && text[start+i]!='\0' &&
            UmiAsciiSearchFold((unsigned char)needle[i])==UmiAsciiSearchFold((unsigned char)text[start+i]))++i;
        if(needle[i]=='\0')return 1;
    }
    return 0;
}
#endif
