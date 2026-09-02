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
/*
 * Initialise codeguard rule registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_codeguard_rule_registry_create(UmiCodeGuardRuleRegistry **out_registry){UmiCodeGuardRuleRegistry *r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_registry==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_registry=NULL;r=(UmiCodeGuardRuleRegistry*)calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_registry=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by codeguard rule registry so the same storage can be reused
 * safely.
 */
void umi_codeguard_rule_registry_destroy(UmiCodeGuardRuleRegistry *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL){free(r->items);free(r);}}
/*
 * Add codeguard rule registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_codeguard_rule_registry_add(UmiCodeGuardRuleRegistry *r,const UmiCodeGuardRule *rules,size_t count){UmiCodeGuardRule *p;size_t cap;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||(rules==NULL&&count>0U))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count+count>r->capacity){cap=r->capacity==0U?128U:r->capacity;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(cap<r->count+count)cap*=2U;p=(UmiCodeGuardRule*)realloc(r->items,cap*sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->items=p;r->capacity=cap;}/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<count;++i)r->items[r->count++]=rules[i];return UMI_STATUS_OK;}
/*
 * Return the number of records represented by codeguard rule registry without changing
 * their state.
 */
size_t umi_codeguard_rule_registry_count(const UmiCodeGuardRuleRegistry *r){return r!=NULL?r->count:0U;}
/*
 * Find codeguard rule registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiCodeGuardRule *umi_codeguard_rule_registry_at(const UmiCodeGuardRuleRegistry *r,size_t i){return r!=NULL&&i<r->count?&r->items[i]:NULL;}
/*
 * Provide the codeguard rule registry add builtin operation used by this module and its
 * client applications.
 */
UmiStatus umi_codeguard_rule_registry_add_builtin(UmiCodeGuardRuleRegistry *registry){const UmiCodeGuardRule *rules;size_t count;UmiStatus status;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL)return UMI_STATUS_INVALID_ARGUMENT;    rules=umi_codeguard_rules_memory(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_buffer(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_string(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_allocation(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_pointer(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_lifetime_rules(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_format(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_integer(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_arithmetic(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_file(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_path(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_temp_file(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_command(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_process(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_environment(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_secret(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_logging(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_network(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_sql(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_concurrency(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_atomic(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_lock(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_resource(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_error(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_input(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_portability(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    rules=umi_codeguard_rules_global_state(&count); status=umi_codeguard_rule_registry_add(registry,rules,count); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
    return UMI_STATUS_OK;}
