/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rule_registry.c
 *
 * PURPOSE:
 *   Collect built-in CodeGuard rules into one growable registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/rule_registry.h"
#include <stdlib.h>
#include "umicom/codeguard/rules/memory.h"
#include "umicom/codeguard/rules/buffer.h"
#include "umicom/codeguard/rules/string.h"
#include "umicom/codeguard/rules/allocation.h"
#include "umicom/codeguard/rules/pointer.h"
#include "umicom/codeguard/rules/lifetime_rules.h"
#include "umicom/codeguard/rules/format.h"
#include "umicom/codeguard/rules/integer.h"
#include "umicom/codeguard/rules/arithmetic.h"
#include "umicom/codeguard/rules/file.h"
#include "umicom/codeguard/rules/path.h"
#include "umicom/codeguard/rules/temp_file.h"
#include "umicom/codeguard/rules/command.h"
#include "umicom/codeguard/rules/process.h"
#include "umicom/codeguard/rules/environment.h"
#include "umicom/codeguard/rules/secret.h"
#include "umicom/codeguard/rules/logging.h"
#include "umicom/codeguard/rules/network.h"
#include "umicom/codeguard/rules/sql.h"
#include "umicom/codeguard/rules/concurrency.h"
#include "umicom/codeguard/rules/atomic.h"
#include "umicom/codeguard/rules/lock.h"
#include "umicom/codeguard/rules/resource.h"
#include "umicom/codeguard/rules/error.h"
#include "umicom/codeguard/rules/input.h"
#include "umicom/codeguard/rules/portability.h"
#include "umicom/codeguard/rules/global_state.h"
#include "umicom/codeguard/rules/private_header.h"
#include "umicom/codeguard/rules/database_boundary.h"
#include "umicom/codeguard/rules/ui_boundary.h"
#include "umicom/codeguard/rules/coupling_rules.h"
#include "umicom/codeguard/rules/duplicate_rules.h"
struct UmiCodeGuardRuleRegistry { UmiCodeGuardRule *items; size_t count; size_t capacity; };
UmiStatus umi_codeguard_rule_registry_create(UmiCodeGuardRuleRegistry **out_registry){UmiCodeGuardRuleRegistry *r;if(out_registry==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_registry=NULL;r=(UmiCodeGuardRuleRegistry*)calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_registry=r;return UMI_STATUS_OK;}
void umi_codeguard_rule_registry_destroy(UmiCodeGuardRuleRegistry *r){if(r!=NULL){free(r->items);free(r);}}
UmiStatus umi_codeguard_rule_registry_add(UmiCodeGuardRuleRegistry *r,const UmiCodeGuardRule *rules,size_t count){UmiCodeGuardRule *p;size_t cap;if(r==NULL||(rules==NULL&&count>0U))return UMI_STATUS_INVALID_ARGUMENT;if(r->count+count>r->capacity){cap=r->capacity==0U?128U:r->capacity;while(cap<r->count+count)cap*=2U;p=(UmiCodeGuardRule*)realloc(r->items,cap*sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->items=p;r->capacity=cap;}for(size_t i=0U;i<count;++i)r->items[r->count++]=rules[i];return UMI_STATUS_OK;}
size_t umi_codeguard_rule_registry_count(const UmiCodeGuardRuleRegistry *r){return r!=NULL?r->count:0U;}
const UmiCodeGuardRule *umi_codeguard_rule_registry_at(const UmiCodeGuardRuleRegistry *r,size_t i){return r!=NULL&&i<r->count?&r->items[i]:NULL;}
UmiStatus umi_codeguard_rule_registry_add_builtin(UmiCodeGuardRuleRegistry *registry){const UmiCodeGuardRule *rules;size_t count;UmiStatus status;if(registry==NULL)return UMI_STATUS_INVALID_ARGUMENT;    rules=umi_codeguard_rules_memory(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_buffer(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_string(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_allocation(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_pointer(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_lifetime_rules(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_format(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_integer(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_arithmetic(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_file(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_path(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_temp_file(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_command(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_process(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_environment(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_secret(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_logging(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_network(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_sql(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_concurrency(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_atomic(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_lock(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_resource(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_error(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_input(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_portability(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_global_state(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); if(status!=UMI_STATUS_OK)return status;
    return UMI_STATUS_OK;}
