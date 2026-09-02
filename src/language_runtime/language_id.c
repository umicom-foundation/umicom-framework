/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/language_id.c
 *
 * PURPOSE:
 *   Implement stable Framework/project to Language Server identity mapping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/language_id.h"
#include <stdio.h>
#include <string.h>
typedef struct A{const char*s,*t;}A;static const A a[]={{"developer.language.c23","c"},{"c23","c"},{"developer.language.cpp","cpp"},{"c++","cpp"},{"developer.language.assembly","assembly"},{"asm","assembly"},{"developer.language.cpython","python"},{"cpython","python"},{"developer.language.rust","rust"},{"developer.language.zig","zig"},{"developer.language.java","java"},{"developer.language.php","php"},{"developer.language.javascript","javascript"},{"developer.language.typescript","typescript"},{"developer.language.html","html"},{"developer.language.css","css"},{"developer.language.markdown","markdown"},{"developer.language.json","json"},{"developer.language.yaml","yaml"},{"developer.language.bash","bash"},{"developer.language.cmake","cmake"},{"developer.language.sql","sql"}};
/*
 * Provide the language runtime normalize language id operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_normalize_language_id(const char*s,char*out,size_t cap){size_t i;const char*r=s;int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!*s||out==NULL||!cap)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<sizeof(a)/sizeof(a[0]);i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(a[i].s,s)==0){r=a[i].t;break;}n=snprintf(out,cap,"%s",r);return n>=0&&(size_t)n<cap?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED;}
