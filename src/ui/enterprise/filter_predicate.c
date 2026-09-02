/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/filter_predicate.c
 *
 * PURPOSE:
 *   Implement text and numeric filter predicates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/filter_predicate.h"
#include <string.h>
#include <ctype.h>
/* Provide the eqi operation used by this module and its client applications. */
static int eqi(const char*a,const char*b){/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*a&&*b){/* Apply this branch only when its contract condition is satisfied. */ if(tolower((unsigned char)*a)!=tolower((unsigned char)*b))return 0;a++;b++;}return *a==*b;}
/* Provide the prefixi operation used by this module and its client applications. */
static int prefixi(const char*a,const char*b){/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*b){/* Apply this branch only when its contract condition is satisfied. */ if(!*a||tolower((unsigned char)*a)!=tolower((unsigned char)*b))return 0;a++;b++;}return 1;}
/*
 * Check that ui ent filter predicate satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_filter_predicate_validate(const UmiUiEntFilterPredicate *p){return p&&umi_ui_ent_id_valid(p->column_id)&&p->op>=UMI_UI_ENT_FILTER_CONTAINS&&p->op<=UMI_UI_ENT_FILTER_LESS;}
/*
 * Provide the ui ent filter predicate matches operation used by this module and its client
 * applications.
 */
int umi_ui_ent_filter_predicate_matches(const UmiUiEntFilterPredicate *p,const UmiUiEntCellValue *v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_ui_ent_filter_predicate_validate(p)||!v)return 0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v->kind==UMI_UI_ENT_VALUE_TEXT&&p->operand.kind==UMI_UI_ENT_VALUE_TEXT){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_EQUALS)return p->case_sensitive?strcmp(v->text,p->operand.text)==0:eqi(v->text,p->operand.text);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_PREFIX)return p->case_sensitive?strncmp(v->text,p->operand.text,strlen(p->operand.text))==0:prefixi(v->text,p->operand.text);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_CONTAINS){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->case_sensitive)return strstr(v->text,p->operand.text)!=NULL;size_t n=strlen(p->operand.text),i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;v->text[i];i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(prefixi(v->text+i,p->operand.text)&&strlen(v->text+i)>=n)return 1;return 0;}}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v->kind==UMI_UI_ENT_VALUE_INTEGER&&p->operand.kind==UMI_UI_ENT_VALUE_INTEGER){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_EQUALS)return v->integer_value==p->operand.integer_value;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_GREATER)return v->integer_value>p->operand.integer_value;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_LESS)return v->integer_value<p->operand.integer_value;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v->kind==UMI_UI_ENT_VALUE_REAL&&p->operand.kind==UMI_UI_ENT_VALUE_REAL){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_GREATER)return v->real_value>p->operand.real_value;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_LESS)return v->real_value<p->operand.real_value;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->op==UMI_UI_ENT_FILTER_EQUALS)return v->real_value==p->operand.real_value;}return 0;}
