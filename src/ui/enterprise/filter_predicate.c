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
static int eqi(const char*a,const char*b){while(*a&&*b){if(tolower((unsigned char)*a)!=tolower((unsigned char)*b))return 0;a++;b++;}return *a==*b;}
static int prefixi(const char*a,const char*b){while(*b){if(!*a||tolower((unsigned char)*a)!=tolower((unsigned char)*b))return 0;a++;b++;}return 1;}
int umi_ui_ent_filter_predicate_validate(const UmiUiEntFilterPredicate *p){return p&&umi_ui_ent_id_valid(p->column_id)&&p->op>=UMI_UI_ENT_FILTER_CONTAINS&&p->op<=UMI_UI_ENT_FILTER_LESS;}
int umi_ui_ent_filter_predicate_matches(const UmiUiEntFilterPredicate *p,const UmiUiEntCellValue *v){if(!umi_ui_ent_filter_predicate_validate(p)||!v)return 0;if(v->kind==UMI_UI_ENT_VALUE_TEXT&&p->operand.kind==UMI_UI_ENT_VALUE_TEXT){if(p->op==UMI_UI_ENT_FILTER_EQUALS)return p->case_sensitive?strcmp(v->text,p->operand.text)==0:eqi(v->text,p->operand.text);if(p->op==UMI_UI_ENT_FILTER_PREFIX)return p->case_sensitive?strncmp(v->text,p->operand.text,strlen(p->operand.text))==0:prefixi(v->text,p->operand.text);if(p->op==UMI_UI_ENT_FILTER_CONTAINS){if(p->case_sensitive)return strstr(v->text,p->operand.text)!=NULL;size_t n=strlen(p->operand.text),i;for(i=0;v->text[i];i++)if(prefixi(v->text+i,p->operand.text)&&strlen(v->text+i)>=n)return 1;return 0;}}if(v->kind==UMI_UI_ENT_VALUE_INTEGER&&p->operand.kind==UMI_UI_ENT_VALUE_INTEGER){if(p->op==UMI_UI_ENT_FILTER_EQUALS)return v->integer_value==p->operand.integer_value;if(p->op==UMI_UI_ENT_FILTER_GREATER)return v->integer_value>p->operand.integer_value;if(p->op==UMI_UI_ENT_FILTER_LESS)return v->integer_value<p->operand.integer_value;}if(v->kind==UMI_UI_ENT_VALUE_REAL&&p->operand.kind==UMI_UI_ENT_VALUE_REAL){if(p->op==UMI_UI_ENT_FILTER_GREATER)return v->real_value>p->operand.real_value;if(p->op==UMI_UI_ENT_FILTER_LESS)return v->real_value<p->operand.real_value;if(p->op==UMI_UI_ENT_FILTER_EQUALS)return v->real_value==p->operand.real_value;}return 0;}
