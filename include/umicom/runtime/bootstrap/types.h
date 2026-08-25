/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/types.h
 *
 * PURPOSE:
 *   Define fixed-capacity C23 data structures shared by the Framework-owned
 *   dependency injection, auto-configuration and application bootstrap layer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_TYPES_H
#define UMICOM_RUNTIME_BOOTSTRAP_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BOOTSTRAP_ID_CAPACITY 128U
#define UMI_BOOTSTRAP_TEXT_CAPACITY 256U
#define UMI_BOOTSTRAP_MAX_ITEMS 128U
#define UMI_BOOTSTRAP_MAX_EDGES 512U
#define UMI_BOOTSTRAP_MAX_DEPENDENCIES 32U
#define UMI_BOOTSTRAP_MAX_TRACE 128U
#define UMI_BOOTSTRAP_MAX_PROPERTIES 128U
#define UMI_BOOTSTRAP_MAX_STAGES 32U

typedef enum UmiBootstrapScopeKind {
    UMI_BOOTSTRAP_SCOPE_SINGLETON = 1,
    UMI_BOOTSTRAP_SCOPE_APPLICATION = 2,
    UMI_BOOTSTRAP_SCOPE_SESSION = 3,
    UMI_BOOTSTRAP_SCOPE_REQUEST = 4,
    UMI_BOOTSTRAP_SCOPE_TRANSIENT = 5
} UmiBootstrapScopeKind;

typedef enum UmiBootstrapLifetimeKind {
    UMI_BOOTSTRAP_LIFETIME_EAGER = 1,
    UMI_BOOTSTRAP_LIFETIME_LAZY = 2,
    UMI_BOOTSTRAP_LIFETIME_EXTERNAL = 3
} UmiBootstrapLifetimeKind;

typedef enum UmiBootstrapBindingKind {
    UMI_BOOTSTRAP_BIND_FACTORY = 1,
    UMI_BOOTSTRAP_BIND_INSTANCE = 2,
    UMI_BOOTSTRAP_BIND_ALIAS = 3
} UmiBootstrapBindingKind;

typedef enum UmiBootstrapLifecyclePhase {
    UMI_BOOTSTRAP_PHASE_DISCOVER = 1,
    UMI_BOOTSTRAP_PHASE_CONFIGURE = 2,
    UMI_BOOTSTRAP_PHASE_CREATE = 3,
    UMI_BOOTSTRAP_PHASE_START = 4,
    UMI_BOOTSTRAP_PHASE_READY = 5,
    UMI_BOOTSTRAP_PHASE_STOP = 6,
    UMI_BOOTSTRAP_PHASE_DESTROY = 7
} UmiBootstrapLifecyclePhase;

typedef enum UmiBootstrapStageState {
    UMI_BOOTSTRAP_STAGE_PENDING = 1,
    UMI_BOOTSTRAP_STAGE_READY = 2,
    UMI_BOOTSTRAP_STAGE_RUNNING = 3,
    UMI_BOOTSTRAP_STAGE_COMPLETE = 4,
    UMI_BOOTSTRAP_STAGE_FAILED = 5,
    UMI_BOOTSTRAP_STAGE_SKIPPED = 6
} UmiBootstrapStageState;

typedef enum UmiBootstrapConditionKind {
    UMI_BOOTSTRAP_CONDITION_PROPERTY_PRESENT = 1,
    UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS = 2,
    UMI_BOOTSTRAP_CONDITION_FEATURE_PRESENT = 3,
    UMI_BOOTSTRAP_CONDITION_CAPABILITY_PRESENT = 4,
    UMI_BOOTSTRAP_CONDITION_PLATFORM_EQUALS = 5,
    UMI_BOOTSTRAP_CONDITION_ENVIRONMENT_EQUALS = 6
} UmiBootstrapConditionKind;

typedef enum UmiBootstrapConditionMode {
    UMI_BOOTSTRAP_CONDITION_ALL = 1,
    UMI_BOOTSTRAP_CONDITION_ANY = 2
} UmiBootstrapConditionMode;

typedef struct UmiBootstrapServiceKey {
    char service_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char qualifier[UMI_BOOTSTRAP_ID_CAPACITY];
} UmiBootstrapServiceKey;

typedef struct UmiBootstrapServiceDescriptor {
    UmiBootstrapServiceKey key;
    char provider_id[UMI_BOOTSTRAP_ID_CAPACITY];
    UmiBootstrapScopeKind scope;
    UmiBootstrapLifetimeKind lifetime;
    int32_t priority;
    uint32_t flags;
    bool enabled;
} UmiBootstrapServiceDescriptor;

typedef struct UmiBootstrapDependencyDescriptor {
    UmiBootstrapServiceKey key;
    bool required;
    bool collection;
} UmiBootstrapDependencyDescriptor;

typedef struct UmiBootstrapBindingDescriptor {
    UmiBootstrapServiceKey key;
    char implementation_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char target_id[UMI_BOOTSTRAP_ID_CAPACITY];
    UmiBootstrapBindingKind kind;
    int32_t priority;
    void *instance;
    bool primary;
    bool enabled;
} UmiBootstrapBindingDescriptor;

typedef struct UmiBootstrapBindingRegistry {
    UmiBootstrapBindingDescriptor items[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiBootstrapBindingRegistry;

typedef struct UmiBootstrapGraphNode {
    char id[UMI_BOOTSTRAP_ID_CAPACITY];
    int32_t priority;
    bool enabled;
} UmiBootstrapGraphNode;

typedef struct UmiBootstrapGraphEdge {
    char dependency_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char dependent_id[UMI_BOOTSTRAP_ID_CAPACITY];
    bool required;
} UmiBootstrapGraphEdge;

typedef struct UmiBootstrapServiceGraph {
    UmiBootstrapGraphNode nodes[UMI_BOOTSTRAP_MAX_ITEMS];
    UmiBootstrapGraphEdge edges[UMI_BOOTSTRAP_MAX_EDGES];
    size_t node_count;
    size_t edge_count;
    uint64_t revision;
} UmiBootstrapServiceGraph;

typedef struct UmiBootstrapIdList {
    char ids[UMI_BOOTSTRAP_MAX_ITEMS][UMI_BOOTSTRAP_ID_CAPACITY];
    size_t count;
} UmiBootstrapIdList;

typedef struct UmiBootstrapIssueReport {
    size_t missing_dependencies;
    size_t ambiguous_bindings;
    size_t cycles;
    char first_issue[UMI_BOOTSTRAP_TEXT_CAPACITY];
} UmiBootstrapIssueReport;

typedef struct UmiBootstrapScopeContext {
    UmiBootstrapScopeKind stack[UMI_BOOTSTRAP_MAX_DEPENDENCIES];
    size_t depth;
} UmiBootstrapScopeContext;

typedef struct UmiBootstrapFactoryDescriptor {
    char factory_id[UMI_BOOTSTRAP_ID_CAPACITY];
    UmiBootstrapServiceKey produces;
    int32_t priority;
    bool enabled;
} UmiBootstrapFactoryDescriptor;

typedef struct UmiBootstrapFactoryRegistry {
    UmiBootstrapFactoryDescriptor items[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t count;
} UmiBootstrapFactoryRegistry;

typedef struct UmiBootstrapProviderDescriptor {
    char provider_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char module_id[UMI_BOOTSTRAP_ID_CAPACITY];
    int32_t priority;
    bool enabled;
} UmiBootstrapProviderDescriptor;

typedef struct UmiBootstrapProviderRegistry {
    UmiBootstrapProviderDescriptor items[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t count;
} UmiBootstrapProviderRegistry;

typedef struct UmiBootstrapConstructorBinding {
    char implementation_id[UMI_BOOTSTRAP_ID_CAPACITY];
    UmiBootstrapDependencyDescriptor dependencies[UMI_BOOTSTRAP_MAX_DEPENDENCIES];
    size_t dependency_count;
} UmiBootstrapConstructorBinding;

typedef struct UmiBootstrapResolutionRequest {
    UmiBootstrapServiceKey key;
    UmiBootstrapScopeKind scope;
    bool required;
} UmiBootstrapResolutionRequest;

typedef struct UmiBootstrapResolutionTrace {
    char steps[UMI_BOOTSTRAP_MAX_TRACE][UMI_BOOTSTRAP_TEXT_CAPACITY];
    size_t count;
} UmiBootstrapResolutionTrace;

typedef struct UmiBootstrapResolutionCacheEntry {
    UmiBootstrapServiceKey key;
    void *instance;
} UmiBootstrapResolutionCacheEntry;

typedef struct UmiBootstrapResolutionCache {
    UmiBootstrapResolutionCacheEntry items[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t count;
} UmiBootstrapResolutionCache;

typedef struct UmiBootstrapResolutionContext {
    UmiBootstrapServiceKey stack[UMI_BOOTSTRAP_MAX_DEPENDENCIES];
    size_t depth;
} UmiBootstrapResolutionContext;

typedef struct UmiBootstrapResolutionPlan {
    UmiBootstrapServiceKey keys[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t count;
} UmiBootstrapResolutionPlan;

typedef struct UmiBootstrapLifecycleHook {
    char hook_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char service_id[UMI_BOOTSTRAP_ID_CAPACITY];
    UmiBootstrapLifecyclePhase phase;
    int32_t order;
} UmiBootstrapLifecycleHook;

typedef struct UmiBootstrapLifecyclePlan {
    UmiBootstrapLifecycleHook hooks[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t count;
} UmiBootstrapLifecyclePlan;

typedef struct UmiBootstrapConfigurationProperty {
    char key[UMI_BOOTSTRAP_ID_CAPACITY];
    char value[UMI_BOOTSTRAP_TEXT_CAPACITY];
    int32_t precedence;
    bool secret;
} UmiBootstrapConfigurationProperty;

typedef struct UmiBootstrapPropertySet {
    UmiBootstrapConfigurationProperty items[UMI_BOOTSTRAP_MAX_PROPERTIES];
    size_t count;
    uint64_t revision;
} UmiBootstrapPropertySet;

typedef struct UmiBootstrapConfigurationSource {
    char source_id[UMI_BOOTSTRAP_ID_CAPACITY];
    int32_t precedence;
    bool trusted;
} UmiBootstrapConfigurationSource;

typedef struct UmiBootstrapConfigurationProfile {
    char profile_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char parent_profile_id[UMI_BOOTSTRAP_ID_CAPACITY];
    int32_t priority;
    bool active;
} UmiBootstrapConfigurationProfile;

typedef struct UmiBootstrapCondition {
    UmiBootstrapConditionKind kind;
    char key[UMI_BOOTSTRAP_ID_CAPACITY];
    char expected[UMI_BOOTSTRAP_TEXT_CAPACITY];
    bool negate;
} UmiBootstrapCondition;

typedef struct UmiBootstrapConditionSet {
    UmiBootstrapCondition items[UMI_BOOTSTRAP_MAX_DEPENDENCIES];
    size_t count;
    UmiBootstrapConditionMode mode;
} UmiBootstrapConditionSet;

typedef struct UmiBootstrapAutoConfiguration {
    char configuration_id[UMI_BOOTSTRAP_ID_CAPACITY];
    int32_t priority;
    UmiBootstrapConditionSet conditions;
    bool enabled;
} UmiBootstrapAutoConfiguration;

typedef struct UmiBootstrapAutoConfigurationCatalogue {
    UmiBootstrapAutoConfiguration items[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t count;
} UmiBootstrapAutoConfigurationCatalogue;

typedef struct UmiBootstrapAutoConfigurationPlan {
    char configuration_ids[UMI_BOOTSTRAP_MAX_ITEMS][UMI_BOOTSTRAP_ID_CAPACITY];
    size_t count;
} UmiBootstrapAutoConfigurationPlan;

typedef struct UmiBootstrapAutoConfigurationReport {
    size_t applied;
    size_t skipped;
    size_t failed;
    char last_message[UMI_BOOTSTRAP_TEXT_CAPACITY];
} UmiBootstrapAutoConfigurationReport;

typedef struct UmiBootstrapStarterDescriptor {
    char starter_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char feature_pack_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char auto_configuration_ids[UMI_BOOTSTRAP_MAX_DEPENDENCIES][UMI_BOOTSTRAP_ID_CAPACITY];
    size_t auto_configuration_count;
    int32_t priority;
    bool enabled;
} UmiBootstrapStarterDescriptor;

typedef struct UmiBootstrapStarterCatalogue {
    UmiBootstrapStarterDescriptor items[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t count;
} UmiBootstrapStarterCatalogue;

typedef struct UmiBootstrapStarterDependency {
    char starter_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char requires_starter_id[UMI_BOOTSTRAP_ID_CAPACITY];
    bool required;
} UmiBootstrapStarterDependency;

typedef struct UmiBootstrapStarterPlan {
    char starter_ids[UMI_BOOTSTRAP_MAX_ITEMS][UMI_BOOTSTRAP_ID_CAPACITY];
    size_t count;
} UmiBootstrapStarterPlan;

typedef struct UmiBootstrapStarterProfile {
    char profile_id[UMI_BOOTSTRAP_ID_CAPACITY];
    UmiBootstrapStarterPlan starters;
} UmiBootstrapStarterProfile;

typedef struct UmiBootstrapContext {
    char application_id[UMI_BOOTSTRAP_ID_CAPACITY];
    char platform_id[UMI_BOOTSTRAP_ID_CAPACITY];
    UmiBootstrapPropertySet properties;
    UmiBootstrapStarterProfile starter_profile;
    size_t resolved_service_count;
    bool dry_run;
} UmiBootstrapContext;

typedef struct UmiBootstrapStage {
    char stage_id[UMI_BOOTSTRAP_ID_CAPACITY];
    UmiBootstrapStageState state;
    int32_t order;
} UmiBootstrapStage;

typedef struct UmiBootstrapPlan {
    UmiBootstrapStage stages[UMI_BOOTSTRAP_MAX_STAGES];
    size_t count;
} UmiBootstrapPlan;

typedef struct UmiBootstrapReport {
    size_t completed;
    size_t skipped;
    size_t failed;
    char last_stage[UMI_BOOTSTRAP_ID_CAPACITY];
    char message[UMI_BOOTSTRAP_TEXT_CAPACITY];
} UmiBootstrapReport;

UmiStatus umi_bootstrap_copy_text(char *dst, size_t capacity, const char *src);
bool umi_bootstrap_id_valid(const char *text);
uint64_t umi_bootstrap_hash_text(const char *text);
int umi_bootstrap_text_compare(const char *left, const char *right);

#ifdef __cplusplus
}
#endif

#endif
