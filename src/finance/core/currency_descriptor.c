/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/currency_descriptor.c
 *
 * PURPOSE:
 *   Implement normalized currency metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/currency_descriptor.h"

#include <ctype.h>
#include <string.h>
/* Initialize normalized currency metadata. */
UmiStatus umi_currency_descriptor_init(UmiCurrencyDescriptor *d,const char *code,const char *name,uint8_t scale){size_t i;UmiStatus st;if(d==NULL||code==NULL||strlen(code)!=3U||scale>6U)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof *d);for(i=0U;i<3U;i++){if(!isalpha((unsigned char)code[i]))return UMI_STATUS_INVALID_ARGUMENT;d->currency.code[i]=(char)toupper((unsigned char)code[i]);}st=umi_financial_core_copy(d->name,sizeof d->name,name);if(st!=UMI_STATUS_OK)return st;d->minor_scale=scale;d->active=true;return UMI_STATUS_OK;}
/* Validate normalized currency metadata. */
bool umi_currency_descriptor_is_valid(const UmiCurrencyDescriptor *d){return d!=NULL&&strlen(d->currency.code)==3U&&d->name[0]!='\0'&&d->minor_scale<=6U;}
/* Match a three-letter currency code case-insensitively. */
bool umi_currency_descriptor_matches(const UmiCurrencyDescriptor *d,const char *code){char c[4]={0};size_t i;if(d==NULL||code==NULL||strlen(code)!=3U)return false;for(i=0U;i<3U;i++)c[i]=(char)toupper((unsigned char)code[i]);return strcmp(d->currency.code,c)==0;}
