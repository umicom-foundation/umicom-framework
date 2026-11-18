/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/entity_tag.c
 *
 * PURPOSE:
 *   Validate complete conditional headers before publishing a weak entity-tag comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/web/entity_tag.h"
#include <string.h>
static void Space(const char *s, size_t size, size_t *at)
{
    while(*at<size && (s[*at]==' ' || s[*at]=='\t')) ++*at;
}
static int Tag(const char *s,size_t size,size_t *at,size_t *first,size_t *length)
{
    if(size-*at>=2 && s[*at]=='W' && s[*at+1]=='/') *at+=2;
    if(*at==size || s[(*at)++]!='"') return 0;
    *first=*at;
    while(*at<size && s[*at]!='"') {
        const unsigned char c=(unsigned char)s[*at];
        if(c<0x21U || c==0x7fU) return 0;
        ++*at;
    }
    if(*at==size) return 0;
    *length=*at-*first;
    ++*at;
    return 1;
}
UmiStatus UmiWebIfNoneMatch(const char *condition,size_t condition_size,
    const char *current_tag,size_t current_size,bool *out_matches)
{
    if(!out_matches || (!condition && condition_size) || (!current_tag && current_size))
        return UMI_STATUS_INVALID_ARGUMENT;
    if(condition_size>UMI_WEB_ENTITY_TAG_LIST_LIMIT || current_size>UMI_WEB_ENTITY_TAG_LIST_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t current_first=0,current_length=0,at=0;
    const bool exists=current_tag!=NULL;
    if(exists && (!Tag(current_tag,current_size,&at,&current_first,&current_length) ||
        at!=current_size)) return UMI_STATUS_PARSE_ERROR;
    if(!condition) { *out_matches=false; return UMI_STATUS_OK; }
    at=0;Space(condition,condition_size,&at);
    if(at<condition_size && condition[at]=='*') {
        ++at;Space(condition,condition_size,&at);
        if(at!=condition_size) return UMI_STATUS_PARSE_ERROR;
        *out_matches=exists;return UMI_STATUS_OK;
    }
    bool matches=false;
    for(;;) {
        size_t first,length;
        if(!Tag(condition,condition_size,&at,&first,&length)) return UMI_STATUS_PARSE_ERROR;
        if(exists && length==current_length &&
            memcmp(condition+first,current_tag+current_first,length)==0) matches=true;
        Space(condition,condition_size,&at);
        if(at==condition_size) break;
        if(condition[at++]!=',') return UMI_STATUS_PARSE_ERROR;
        Space(condition,condition_size,&at);
    }
    *out_matches=matches;
    return UMI_STATUS_OK;
}
