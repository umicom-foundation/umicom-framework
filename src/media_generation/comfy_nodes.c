/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_nodes.c
 *
 * PURPOSE:
 *   Own ComfyUI node observations through the shared bounded JSON and connection identity services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_nodes.h"
#include "umicom/media_generation/comfy_numeric_input.h"
#include "umicom/media_generation/comfy_choice_input.h"
#include "comfy_response_internal.h"
#include "umicom/security/secrets.h"
#include "umicom/base/utf8.h"
#include <stdlib.h>
#include <string.h>
typedef struct NodeEntry {
    UmiComfyNodeInfo info;
    int object, required, optional, outputs;
} NodeEntry;
struct UmiComfyNodeCatalogue {
    UmiJsonTree *tree;
    NodeEntry *entries;
    size_t count;
    uint64_t connection_revision;
    unsigned char connection_sha256[UMI_SHA256_BYTES];
    unsigned char response_sha256[UMI_SHA256_BYTES];
};
static UmiStatus OptionalText(const UmiJsonTree *tree,int object,const char *name,char *out,size_t capacity)
{
    int node=-1; UmiStatus status=UmiJsonTreeMember(tree,object,name,&node);
    if(status==UMI_STATUS_NOT_FOUND)return UMI_STATUS_OK;
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_PARSE_ERROR;
    return UmiJsonTreeText(tree,node,out,capacity);
}
static UmiStatus InputGroup(const UmiJsonTree *tree,int inputs,const char *name,
    const UmiCancellationToken *cancel,int *out,size_t *count)
{
    int group=-1;UmiStatus status=UmiJsonTreeMember(tree,inputs,name,&group);
    if(status==UMI_STATUS_NOT_FOUND){*out=-1;*count=0U;return UMI_STATUS_OK;}
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,group)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return UMI_STATUS_PARSE_ERROR;
    if(UmiJsonTreeCount(tree,group)>UMI_COMFY_NODE_INPUT_LIMIT)return UMI_STATUS_CAPACITY_EXCEEDED;
    for(int key=UmiJsonTreeFirst(tree,group);key>=0;){
        if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
        char field[129];status=UmiJsonTreeText(tree,key,field,sizeof field);
        if(status!=UMI_STATUS_OK)return status;
        if(field[0]=='\0')return UMI_STATUS_PARSE_ERROR;
        key=UmiJsonTreeNext(tree,UmiJsonTreeNext(tree,key));
    }
    *out=group;*count=UmiJsonTreeCount(tree,group);return UMI_STATUS_OK;
}
static UmiStatus ReadNode(const UmiJsonTree *tree,int key,const UmiCancellationToken *cancel,NodeEntry *entry)
{
    entry->required=entry->optional=-1;
    entry->object=UmiJsonTreeNext(tree,key);
    if(UmiJsonTreeKind(tree,entry->object)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return UMI_STATUS_PARSE_ERROR;
    UmiStatus status=UmiJsonTreeText(tree,key,entry->info.class_type,sizeof entry->info.class_type);
    if(status!=UMI_STATUS_OK)return status;
    if(entry->info.class_type[0]=='\0')return UMI_STATUS_PARSE_ERROR;
    int name=-1;
    status=UmiJsonTreeMember(tree,entry->object,"name",&name);
    if(status==UMI_STATUS_OK){
        char actual[129];
        if(UmiJsonTreeKind(tree,name)!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_PARSE_ERROR;
        status=UmiJsonTreeText(tree,name,actual,sizeof actual);
        if(status!=UMI_STATUS_OK)return status;
        if(strcmp(actual,entry->info.class_type)!=0)return UMI_STATUS_PARSE_ERROR;
    } else if(status!=UMI_STATUS_NOT_FOUND)return status;
    status=OptionalText(tree,entry->object,"display_name",entry->info.display_name,sizeof entry->info.display_name);
    if(status==UMI_STATUS_OK)status=OptionalText(tree,entry->object,"category",entry->info.category,sizeof entry->info.category);
    if(status!=UMI_STATUS_OK)return status;
    static const char *const names[]={"output_node","deprecated","experimental","api_node"};
    static const unsigned bits[]={UMI_COMFY_NODE_OUTPUT_NODE,UMI_COMFY_NODE_DEPRECATED,UMI_COMFY_NODE_EXPERIMENTAL,UMI_COMFY_NODE_API_NODE};
    for(size_t i=0U;i<4U;++i){
        int node=-1,value=0;status=UmiJsonTreeMember(tree,entry->object,names[i],&node);
        if(status==UMI_STATUS_NOT_FOUND)continue;
        if(status!=UMI_STATUS_OK)return status;
        status=UmiJsonTreeBoolean(tree,node,&value);if(status!=UMI_STATUS_OK)return status;
        entry->info.known_flags|=bits[i];if(value)entry->info.enabled_flags|=bits[i];
    }
    int inputs=-1;
    status=UmiComfyResponseMember(tree,entry->object,"input",&inputs);
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,inputs)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return UMI_STATUS_PARSE_ERROR;
    status=InputGroup(tree,inputs,"required",cancel,&entry->required,&entry->info.required_count);
    if(status==UMI_STATUS_OK)status=InputGroup(tree,inputs,"optional",cancel,&entry->optional,&entry->info.optional_count);
    if(status!=UMI_STATUS_OK)return status;
    if(entry->required>=0 && entry->optional>=0){
        for(int field=UmiJsonTreeFirst(tree,entry->required);field>=0;){
            if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
            char text[129];int duplicate=-1;status=UmiJsonTreeText(tree,field,text,sizeof text);
            if(status!=UMI_STATUS_OK)return status;
            status=UmiJsonTreeMember(tree,entry->optional,text,&duplicate);
            if(status==UMI_STATUS_OK)return UMI_STATUS_ALREADY_EXISTS;
            if(status!=UMI_STATUS_NOT_FOUND)return status;
            field=UmiJsonTreeNext(tree,UmiJsonTreeNext(tree,field));
        }
    }
    status=UmiComfyResponseMember(tree,entry->object,"output",&entry->outputs);
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,entry->outputs)!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return UMI_STATUS_PARSE_ERROR;
    entry->info.output_count=UmiJsonTreeCount(tree,entry->outputs);
    if(entry->info.output_count>UMI_COMFY_NODE_OUTPUT_LIMIT)return UMI_STATUS_CAPACITY_EXCEEDED;
    for(int output=UmiJsonTreeFirst(tree,entry->outputs);output>=0;output=UmiJsonTreeNext(tree,output)){
        char type[129];
        if(UmiJsonTreeKind(tree,output)!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_PARSE_ERROR;
        status=UmiJsonTreeText(tree,output,type,sizeof type);
        if(status!=UMI_STATUS_OK)return status;
        if(type[0]=='\0')return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
void UmiComfyNodeCatalogueDestroy(UmiComfyNodeCatalogue *catalogue)
{
    if(catalogue==NULL)return;
    UmiJsonTreeDestroy(catalogue->tree);
    if(catalogue->entries){umi_secret_clear(catalogue->entries,catalogue->count*sizeof *catalogue->entries);free(catalogue->entries);}
    umi_secret_clear(catalogue,sizeof *catalogue);free(catalogue);
}
UmiStatus UmiComfyNodeCatalogueCreate(const UmiProviderConnection *connection,
    uint64_t connection_revision,const void *bytes,size_t length,const UmiCancellationToken *cancel,UmiComfyNodeCatalogue **out)
{
    if(out==NULL || *out!=NULL || connection_revision==0U)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
    UmiStatus status=UmiComfyConnectionCheck(connection);
    if(status!=UMI_STATUS_OK)return status;
    UmiComfyNodeCatalogue *candidate=calloc(1U,sizeof *candidate);
    if(candidate==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    candidate->connection_revision=connection_revision;
    status=UmiProviderConnectionFingerprint(connection,candidate->connection_sha256);
    if(status==UMI_STATUS_OK)status=UmiComfyResponseOpen(bytes,length,UMI_COMFY_NODES_MAX_BYTES,cancel,&candidate->tree);
    if(status==UMI_STATUS_OK){
        candidate->count=UmiJsonTreeCount(candidate->tree,0);
        if(candidate->count>UMI_COMFY_NODES_LIMIT)status=UMI_STATUS_CAPACITY_EXCEEDED;
        else if(candidate->count){candidate->entries=calloc(candidate->count,sizeof *candidate->entries);if(!candidate->entries)status=UMI_STATUS_OUT_OF_MEMORY;}
    }
    size_t index=0U;
    for(int key=status==UMI_STATUS_OK?UmiJsonTreeFirst(candidate->tree,0):-1;key>=0 && status==UMI_STATUS_OK;){
        if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;break;}
        status=ReadNode(candidate->tree,key,cancel,&candidate->entries[index++]);
        key=UmiJsonTreeNext(candidate->tree,UmiJsonTreeNext(candidate->tree,key));
    }
    if(status==UMI_STATUS_OK){
        /* Bind downstream review to these exact bytes, independently of the
         * connection's configuration identity or the server's availability. */
        UmiSha256 digest; UmiSha256Init(&digest);
        status=UmiSha256Update(&digest,bytes,length);
        if(status==UMI_STATUS_OK)status=UmiSha256Final(&digest,candidate->response_sha256);
    }
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)*out=candidate;else UmiComfyNodeCatalogueDestroy(candidate);
    return status;
}
size_t UmiComfyNodeCatalogueCount(const UmiComfyNodeCatalogue *catalogue){return catalogue?catalogue->count:0U;}
UmiStatus UmiComfyNodeCatalogueAt(const UmiComfyNodeCatalogue *catalogue,size_t index,UmiComfyNodeInfo *out)
{
    if(!catalogue || !out)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=catalogue->count)return UMI_STATUS_NOT_FOUND;
    *out=catalogue->entries[index].info;return UMI_STATUS_OK;
}
UmiStatus UmiComfyNodeCatalogueFind(const UmiComfyNodeCatalogue *catalogue,const char *class_type,size_t *out_index)
{
    if(!catalogue || !class_type || !out_index)return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;while(length<129U && class_type[length]!='\0')++length;
    if(length==0U)return UMI_STATUS_INVALID_ARGUMENT;
    if(length==129U)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(!UmiUtf8Validate((const unsigned char *)class_type,length,NULL))return UMI_STATUS_INVALID_ARGUMENT;
    for(size_t i=0U;i<catalogue->count;++i)if(strcmp(class_type,catalogue->entries[i].info.class_type)==0){*out_index=i;return UMI_STATUS_OK;}
    return UMI_STATUS_NOT_FOUND;
}
/* The raw input lookup is centralized so typed numeric observations use the same owner, source order and index admission. The prior implementation is retained for review; its borrowed-span behavior remains available. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiComfyNodeCatalogueInputAt(const UmiComfyNodeCatalogue *catalogue,size_t node_index,
    UmiComfyInputGroup group,size_t input_index,UmiComfyNodeInput *out)
{
    if(!catalogue || !out || (group!=UMI_COMFY_INPUT_REQUIRED && group!=UMI_COMFY_INPUT_OPTIONAL))return UMI_STATUS_INVALID_ARGUMENT;
    if(node_index>=catalogue->count)return UMI_STATUS_NOT_FOUND;
    const NodeEntry *entry=&catalogue->entries[node_index];
    size_t count=group==UMI_COMFY_INPUT_REQUIRED?entry->info.required_count:entry->info.optional_count;
    if(input_index>=count)return UMI_STATUS_NOT_FOUND;
    int object=group==UMI_COMFY_INPUT_REQUIRED?entry->required:entry->optional;
    int key=UmiJsonTreeFirst(catalogue->tree,object);
    for(size_t i=0U;i<input_index;++i)key=UmiJsonTreeNext(catalogue->tree,UmiJsonTreeNext(catalogue->tree,key));
    UmiComfyNodeInput staged={0};
    UmiStatus status=UmiJsonTreeText(catalogue->tree,key,staged.name,sizeof staged.name);
    if(status==UMI_STATUS_OK)status=UmiJsonTreeSourceSpan(catalogue->tree,UmiJsonTreeNext(catalogue->tree,key),&staged.schema_json,&staged.schema_bytes);
    if(status==UMI_STATUS_OK)*out=staged;
    return status;
}

#endif
/* One lookup serves raw and typed observations without changing catalogue order
 * or admission precedence. The key and descriptor stay inside the existing
 * immutable JSON owner; no second parser or response allocation is introduced. */
static UmiStatus InputKey(const UmiComfyNodeCatalogue *catalogue, size_t node_index,
    UmiComfyInputGroup group, size_t input_index, int *out_key)
{
    if (!catalogue || !out_key ||
        (group != UMI_COMFY_INPUT_REQUIRED && group != UMI_COMFY_INPUT_OPTIONAL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (node_index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    const NodeEntry *entry = &catalogue->entries[node_index];
    size_t count = group == UMI_COMFY_INPUT_REQUIRED ?
        entry->info.required_count : entry->info.optional_count;
    if (input_index >= count)
        return UMI_STATUS_NOT_FOUND;
    int object = group == UMI_COMFY_INPUT_REQUIRED ? entry->required : entry->optional;
    int key = UmiJsonTreeFirst(catalogue->tree, object);
    for (size_t i = 0U; i < input_index; ++i)
        key = UmiJsonTreeNext(catalogue->tree, UmiJsonTreeNext(catalogue->tree, key));
    *out_key = key;
    return UMI_STATUS_OK;
}

UmiStatus UmiComfyNodeCatalogueInputAt(const UmiComfyNodeCatalogue *catalogue, size_t node_index,
    UmiComfyInputGroup group, size_t input_index, UmiComfyNodeInput *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    int key = -1;
    UmiStatus status = InputKey(catalogue, node_index, group, input_index, &key);
    if (status != UMI_STATUS_OK)
        return status;
    UmiComfyNodeInput staged = {0};
    status = UmiJsonTreeText(catalogue->tree, key, staged.name, sizeof(staged.name));
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(catalogue->tree, UmiJsonTreeNext(catalogue->tree, key),
            &staged.schema_json, &staged.schema_bytes);
    if (status == UMI_STATUS_OK)
        *out = staged;
    return status;
}

UmiStatus UmiComfyNodeCatalogueOutputAt(const UmiComfyNodeCatalogue *catalogue,size_t node_index,
    size_t output_index,char *out_type,size_t capacity)
{
    if(!catalogue || !out_type)return UMI_STATUS_INVALID_ARGUMENT;
    if(node_index>=catalogue->count)return UMI_STATUS_NOT_FOUND;
    const NodeEntry *entry=&catalogue->entries[node_index];
    if(output_index>=entry->info.output_count)return UMI_STATUS_NOT_FOUND;
    int value=UmiJsonTreeFirst(catalogue->tree,entry->outputs);
    for(size_t i=0U;i<output_index;++i)value=UmiJsonTreeNext(catalogue->tree,value);
    return UmiJsonTreeText(catalogue->tree,value,out_type,capacity);
}
UmiStatus UmiComfyNodeCatalogueSchema(const UmiComfyNodeCatalogue *catalogue,size_t node_index,
    const char **out_json,size_t *out_length)
{
    if(!catalogue || !out_json || !out_length)return UMI_STATUS_INVALID_ARGUMENT;
    if(node_index>=catalogue->count)return UMI_STATUS_NOT_FOUND;
    return UmiJsonTreeSourceSpan(catalogue->tree,catalogue->entries[node_index].object,out_json,out_length);
}
UmiStatus UmiComfyNodeCatalogueCheckConnection(const UmiComfyNodeCatalogue *catalogue,
    const UmiProviderConnection *current,uint64_t current_revision)
{
    if(!catalogue || !current)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiComfyConnectionCheck(current);
    if(status!=UMI_STATUS_OK)return status;
    if(current_revision!=catalogue->connection_revision)return UMI_STATUS_BUSY;
    unsigned char digest[UMI_SHA256_BYTES];status=UmiProviderConnectionFingerprint(current,digest);
    if(status==UMI_STATUS_OK && memcmp(digest,catalogue->connection_sha256,sizeof digest)!=0)status=UMI_STATUS_BUSY;
    return status;
}

UmiStatus UmiComfyNodeCatalogueInspect(const UmiComfyNodeCatalogue *catalogue,
    UmiComfyNodeCatalogueReview *out)
{
    if(!catalogue || !out)return UMI_STATUS_INVALID_ARGUMENT;
    UmiComfyNodeCatalogueReview staged={0};
    staged.connection_revision=catalogue->connection_revision;
    memcpy(staged.connection_sha256,catalogue->connection_sha256,sizeof staged.connection_sha256);
    memcpy(staged.response_sha256,catalogue->response_sha256,sizeof staged.response_sha256);
    *out=staged;return UMI_STATUS_OK;
}


static bool NumericInconsistent(const UmiComfyNumericInput *input)
{
    bool default_known = (input->known_values & (1U << UMI_COMFY_NUMBER_DEFAULT)) != 0U;
    bool minimum_known = (input->known_values & (1U << UMI_COMFY_NUMBER_MINIMUM)) != 0U;
    bool maximum_known = (input->known_values & (1U << UMI_COMFY_NUMBER_MAXIMUM)) != 0U;
    bool step_known = (input->known_values & (1U << UMI_COMFY_NUMBER_STEP)) != 0U;
    /* Keep integer comparisons exact; converting a seed bound to double here
     * would lose information before the host ever received the observation. */
    if (input->kind == UMI_COMFY_NUMBER_INTEGER)
    {
        const int64_t *v = input->values.integers;
        return (minimum_known && maximum_known && v[1] > v[2]) ||
            (default_known && minimum_known && v[0] < v[1]) ||
            (default_known && maximum_known && v[0] > v[2]) ||
            (step_known && v[3] <= 0);
    }
    const double *v = input->values.decimals;
    return (minimum_known && maximum_known && v[1] > v[2]) ||
        (default_known && minimum_known && v[0] < v[1]) ||
        (default_known && maximum_known && v[0] > v[2]) ||
        (step_known && v[3] <= 0.0);
}

UmiStatus UmiComfyNodeCatalogueNumericInput(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    UmiComfyNumericInput *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    int key = -1;
    UmiStatus status = InputKey(catalogue, node_index, group, input_index, &key);
    if (status != UMI_STATUS_OK)
        return status;
    const UmiJsonTree *tree = catalogue->tree;
    int descriptor = UmiJsonTreeNext(tree, key);
    if (UmiJsonTreeKind(tree, descriptor) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_NOT_IMPLEMENTED;
    size_t count = UmiJsonTreeCount(tree, descriptor);
    if (count == 0U)
        return UMI_STATUS_PARSE_ERROR;
    if (count > 2U)
        return UMI_STATUS_NOT_IMPLEMENTED;
    int type_node = UmiJsonTreeFirst(tree, descriptor);
    if (UmiJsonTreeKind(tree, type_node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UmiJsonTreeKind(tree, type_node) == UMI_LANGUAGE_RUNTIME_JSON_ARRAY ?
            UMI_STATUS_NOT_IMPLEMENTED : UMI_STATUS_PARSE_ERROR;
    char type[129];
    status = UmiJsonTreeText(tree, type_node, type, sizeof(type));
    if (status != UMI_STATUS_OK)
        return status;
    UmiComfyNumericInput staged = {0};
    if (strcmp(type, "INT") == 0)
        staged.kind = UMI_COMFY_NUMBER_INTEGER;
    else if (strcmp(type, "FLOAT") == 0)
        staged.kind = UMI_COMFY_NUMBER_DECIMAL;
    else
        return UMI_STATUS_NOT_IMPLEMENTED;
    status = UmiJsonTreeText(tree, key, staged.name, sizeof(staged.name));
    if (status != UMI_STATUS_OK)
        return status;
    if (count == 2U)
    {
        int options = UmiJsonTreeNext(tree, type_node);
        if (UmiJsonTreeKind(tree, options) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            return UMI_STATUS_PARSE_ERROR;
        static const char *const names[] = {"default", "min", "max", "step"};
        size_t interpreted = 0U;
        for (size_t i = 0U; i < 4U; ++i)
        {
            int value = -1;
            status = UmiJsonTreeMember(tree, options, names[i], &value);
            if (status == UMI_STATUS_NOT_FOUND)
                continue;
            if (status != UMI_STATUS_OK)
                return status;
            status = staged.kind == UMI_COMFY_NUMBER_INTEGER ?
                UmiJsonTreeInteger(tree, value, &staged.values.integers[i]) :
                UmiJsonTreeDouble(tree, value, &staged.values.decimals[i]);
            if (status != UMI_STATUS_OK)
                return status;
            staged.known_values |= 1U << i;
            ++interpreted;
        }
        int force = -1, enabled = 0;
        status = UmiJsonTreeMember(tree, options, "forceInput", &force);
        if (status == UMI_STATUS_OK)
        {
            status = UmiJsonTreeBoolean(tree, force, &enabled);
            if (status != UMI_STATUS_OK)
                return status;
            staged.known_flags |= UMI_COMFY_NUMBER_FORCE_INPUT;
            if (enabled)
                staged.enabled_flags |= UMI_COMFY_NUMBER_FORCE_INPUT;
            ++interpreted;
        }
        else if (status != UMI_STATUS_NOT_FOUND)
            return status;
        staged.uninterpreted_options = UmiJsonTreeCount(tree, options) - interpreted;
    }
    staged.inconsistent_values = NumericInconsistent(&staged);
    /* Only this final assignment publishes. Unsupported or malformed metadata
     * cannot replace an earlier host report or mutate the raw catalogue. */
    *out = staged;
    return UMI_STATUS_OK;
}


/* Validate the complete legacy string-list profile before publishing a count
 * or one label. A later custom value must not make an earlier prefix look like
 * a fully supported selector. Existing raw descriptors remain accessible. */
static UmiStatus ChoiceProfile(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    int *out_key, int *out_choices, size_t *out_options)
{
    int key = -1;
    UmiStatus status = InputKey(catalogue, node_index, group, input_index, &key);
    if (status != UMI_STATUS_OK)
        return status;
    const UmiJsonTree *tree = catalogue->tree;
    int descriptor = UmiJsonTreeNext(tree, key);
    if (UmiJsonTreeKind(tree, descriptor) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_NOT_IMPLEMENTED;
    size_t count = UmiJsonTreeCount(tree, descriptor);
    if (count == 0U)
        return UMI_STATUS_PARSE_ERROR;
    if (count > 2U)
        return UMI_STATUS_NOT_IMPLEMENTED;
    int choices = UmiJsonTreeFirst(tree, descriptor);
    UmiLanguageRuntimeJsonTokenType kind = UmiJsonTreeKind(tree, choices);
    if (kind != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return kind == UMI_LANGUAGE_RUNTIME_JSON_STRING ?
            UMI_STATUS_NOT_IMPLEMENTED : UMI_STATUS_PARSE_ERROR;
    size_t options_count = 0U;
    if (count == 2U)
    {
        int options = UmiJsonTreeNext(tree, choices);
        if (UmiJsonTreeKind(tree, options) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            return UMI_STATUS_PARSE_ERROR;
        options_count = UmiJsonTreeCount(tree, options);
    }
    for (int item = UmiJsonTreeFirst(tree, choices); item >= 0;
         item = UmiJsonTreeNext(tree, item))
        if (UmiJsonTreeKind(tree, item) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
            return UMI_STATUS_NOT_IMPLEMENTED;
    *out_key = key;
    *out_choices = choices;
    *out_options = options_count;
    return UMI_STATUS_OK;
}
UmiStatus UmiComfyNodeCatalogueChoiceInput(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    UmiComfyChoiceInput *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    int key = -1, choices = -1;
    size_t options = 0U;
    UmiStatus status = ChoiceProfile(catalogue, node_index, group, input_index,
        &key, &choices, &options);
    if (status != UMI_STATUS_OK)
        return status;
    UmiComfyChoiceInput staged = {0};
    status = UmiJsonTreeText(catalogue->tree, key, staged.name, sizeof(staged.name));
    if (status != UMI_STATUS_OK)
        return status;
    staged.choice_count = UmiJsonTreeCount(catalogue->tree, choices);
    staged.uninterpreted_options = options;
    *out = staged;
    return UMI_STATUS_OK;
}
UmiStatus UmiComfyNodeCatalogueChoiceAt(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    size_t choice_index, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    int key = -1, choices = -1;
    size_t options = 0U;
    UmiStatus status = ChoiceProfile(catalogue, node_index, group, input_index,
        &key, &choices, &options);
    if (status != UMI_STATUS_OK)
        return status;
    const UmiJsonTree *tree = catalogue->tree;
    if (choice_index >= UmiJsonTreeCount(tree, choices))
        return UMI_STATUS_NOT_FOUND;
    int selected = UmiJsonTreeFirst(tree, choices);
    for (size_t index = 0U; index < choice_index; ++index)
        selected = UmiJsonTreeNext(tree, selected);
    /* Shared text decoding preflights capacity before writing any byte. */
    return UmiJsonTreeText(tree, selected, out, capacity);
}
