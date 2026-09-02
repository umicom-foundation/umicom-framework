/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/header.c
 *
 * PURPOSE:
 *   Implement bounded HTTP header assignment and case-insensitive names.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/header.h"
#include <ctype.h>
#include <string.h>
/* Provide the name equal operation used by this module and its client applications. */
static int name_equal(const char *a,const char *b){size_t i=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||b==NULL)return 0;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(a[i]!='\0'&&b[i]!='\0'){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tolower((unsigned char)a[i])!=tolower((unsigned char)b[i]))return 0;++i;}return a[i]=='\0'&&b[i]=='\0';}
/*
 * Copy web header into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_web_header_set(UmiWebHeader *header,const char *name,const char *value){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(header==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_web_copy_text(header->name,sizeof(header->name),name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_web_copy_text(header->value,sizeof(header->value),value);}
/*
 * Provide the web header name equal operation used by this module and its client
 * applications.
 */
int umi_web_header_name_equal(const UmiWebHeader *header,const char *name){return header!=NULL&&name_equal(header->name,name);}
