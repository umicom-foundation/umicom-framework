/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/task_store.c
 *
 * PURPOSE:
 *   Commit provider task transitions atomically while retaining uncertain and malformed recovery evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "internal.h"
#include "umicom/provider_connections/task_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiProviderTasks {
    UmiDataServer *server;
    UmiMutex *mutex;
    char application_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char profile_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char prefix[128];
    bool poisoned;
};
static bool task_scope(const UmiProviderTasks *store,const UmiProviderTaskReceipt *r)
{
    return strcmp(store->application_id,r->context.application_id)==0 &&
        strcmp(store->profile_id,r->context.profile_id)==0;
}
static UmiStatus task_key(const UmiProviderTasks *store,const char *id,char *key,size_t capacity)
{
    int length=snprintf(key,capacity,"%s%s",store->prefix,id);
    return length>=0 && (size_t)length<capacity?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED;
}
static UmiStatus task_load(UmiProviderTasks *store,const char *id,UmiProviderTaskReceipt *out)
{
    char key[UMI_PROVIDER_CONNECTION_KEY_CAPACITY],wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];
    UmiStatus status=task_key(store,id,key,sizeof(key));
    if(status==UMI_STATUS_OK) status=umi_data_server_get(store->server,key,wire,sizeof(wire));
    if(status==UMI_STATUS_OK) status=UmiProviderTaskDecode(wire,strlen(wire),out);
    if(status==UMI_STATUS_OK && (!task_scope(store,out) || strcmp(id,out->context.local_id)!=0))
        status=UMI_STATUS_PARSE_ERROR;
    return status;
}
static UmiStatus task_save(UmiProviderTasks *store,const UmiProviderTaskReceipt *r)
{
    char key[UMI_PROVIDER_CONNECTION_KEY_CAPACITY],wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];size_t length;
    UmiStatus status=task_key(store,r->context.local_id,key,sizeof(key));
    if(status==UMI_STATUS_OK) status=UmiProviderTaskEncode(r,wire,sizeof(wire),&length);
    if(status==UMI_STATUS_OK) status=umi_data_server_set(store->server,key,wire);
    return status;
}
UmiStatus UmiProviderTasksOpen(UmiDataServer *server,const char *application_id,
    const char *profile_id,UmiProviderTasks **out_store)
{
    if(server==NULL || out_store==NULL ||
        !UmiProviderConnectionIdValid(application_id,UMI_PROVIDER_CONNECTION_ID_CAPACITY) ||
        !UmiProviderConnectionIdValid(profile_id,UMI_PROVIDER_CONNECTION_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderTasks *store=calloc(1U,sizeof(*store));if(store==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    int length=snprintf(store->prefix,sizeof(store->prefix),"provider.tasks/%s/%s/",application_id,profile_id);
    UmiStatus status=length>=0 && (size_t)length<sizeof(store->prefix)?
        umi_mutex_create(&store->mutex):UMI_STATUS_CAPACITY_EXCEEDED;
    if(status!=UMI_STATUS_OK) { free(store);return status; }
    store->server=server;strcpy(store->application_id,application_id);strcpy(store->profile_id,profile_id);
    *out_store=store;return UMI_STATUS_OK;
}
void UmiProviderTasksDestroy(UmiProviderTasks *store)
{
    if(store==NULL) return;
    umi_mutex_destroy(store->mutex);free(store);
}
UmiStatus UmiProviderTasksCreate(UmiProviderTasks *store,const UmiProviderTaskReceipt *prepared)
{
    if(store==NULL || UmiProviderTaskValidate(prepared)!=UMI_STATUS_OK ||
        prepared->phase!=UMI_PROVIDER_TASK_PREPARED || prepared->revision!=1U ||
        !task_scope(store,prepared)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_mutex_lock(store->mutex);if(status!=UMI_STATUS_OK) return status;
    status=UmiProviderStorageBegin(store->server,store->poisoned);
    if(status==UMI_STATUS_OK) {
        char key[UMI_PROVIDER_CONNECTION_KEY_CAPACITY],wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];
        status=task_key(store,prepared->context.local_id,key,sizeof(key));
        if(status==UMI_STATUS_OK) {
            status=umi_data_server_get(store->server,key,wire,sizeof(wire));
            /* Any existing bytes reserve the local ID, even if their schema is
             * unknown or damaged. Absence is the only admission to creation. */
            if(status==UMI_STATUS_OK) status=UMI_STATUS_ALREADY_EXISTS;
            else if(status==UMI_STATUS_NOT_FOUND) status=task_save(store,prepared);
        }
        status=UmiProviderStorageFinish(store->server,&store->poisoned,status);
    }
    (void)umi_mutex_unlock(store->mutex);return status;
}
UmiStatus UmiProviderTasksRead(UmiProviderTasks *store,const char *id,UmiProviderTaskReceipt *out)
{
    if(store==NULL || out==NULL || !UmiProviderConnectionIdValid(id,UMI_PROVIDER_CONNECTION_ID_CAPACITY))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderTaskReceipt candidate;UmiStatus status=umi_mutex_lock(store->mutex);
    if(status!=UMI_STATUS_OK) return status;
    status=UmiProviderStorageBegin(store->server,store->poisoned);
    if(status==UMI_STATUS_OK) status=UmiProviderStorageFinish(store->server,&store->poisoned,task_load(store,id,&candidate));
    if(status==UMI_STATUS_OK) *out=candidate;
    (void)umi_mutex_unlock(store->mutex);return status;
}
typedef enum TaskEditKind { TASK_EDIT_ADVANCE,TASK_EDIT_CONFIRM,TASK_EDIT_OUTPUT } TaskEditKind;
/* Both storage and pure transition owners keep their existing responsibilities.
 * Holding the transaction across load, revision comparison and save prevents a
 * second store from accepting the same old revision. No effect runs under it. */
static UmiStatus task_edit(UmiProviderTasks *store,const char *id,uint64_t expected,
    TaskEditKind kind,UmiProviderTaskEvent event,const char *remote_id,uint64_t bytes,
    const unsigned char *digest,UmiProviderTaskReceipt *out)
{
    if(store==NULL || out==NULL || !UmiProviderConnectionIdValid(id,UMI_PROVIDER_CONNECTION_ID_CAPACITY))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderTaskReceipt before,candidate;UmiStatus status=umi_mutex_lock(store->mutex);
    if(status!=UMI_STATUS_OK) return status;
    status=UmiProviderStorageBegin(store->server,store->poisoned);
    if(status==UMI_STATUS_OK) {
        status=task_load(store,id,&before);
        if(status==UMI_STATUS_OK) {
            if(kind==TASK_EDIT_ADVANCE) status=UmiProviderTaskAdvance(&before,expected,event,&candidate);
            else if(kind==TASK_EDIT_CONFIRM) status=UmiProviderTaskConfirm(&before,expected,remote_id,&candidate);
            else status=UmiProviderTaskRecordOutput(&before,expected,bytes,digest,&candidate);
        }
        if(status==UMI_STATUS_OK && candidate.revision!=before.revision) status=task_save(store,&candidate);
        status=UmiProviderStorageFinish(store->server,&store->poisoned,status);
    }
    if(status==UMI_STATUS_OK) *out=candidate;
    (void)umi_mutex_unlock(store->mutex);return status;
}
UmiStatus UmiProviderTasksAdvance(UmiProviderTasks *store,const char *id,uint64_t expected,
    UmiProviderTaskEvent event,UmiProviderTaskReceipt *out)
{ return task_edit(store,id,expected,TASK_EDIT_ADVANCE,event,NULL,0U,NULL,out); }
UmiStatus UmiProviderTasksConfirm(UmiProviderTasks *store,const char *id,uint64_t expected,
    const char *remote_id,UmiProviderTaskReceipt *out)
{ return task_edit(store,id,expected,TASK_EDIT_CONFIRM,(UmiProviderTaskEvent)0,remote_id,0U,NULL,out); }
UmiStatus UmiProviderTasksRecordOutput(UmiProviderTasks *store,const char *id,uint64_t expected,
    uint64_t bytes,const unsigned char digest[UMI_SHA256_BYTES],UmiProviderTaskReceipt *out)
{ return task_edit(store,id,expected,TASK_EDIT_OUTPUT,(UmiProviderTaskEvent)0,NULL,bytes,digest,out); }

/* Task discovery follows the existing bounded catalogue pattern while keeping
 * receipt keys and validation in their provider owner. Never call back into
 * Data Server from its visitor; all work here uses borrowed strings and pure
 * codecs. Publication waits for both a complete scan and transaction commit. */
typedef struct TaskCatalogue {
    UmiProviderTasks *store;
    size_t scanned,count;
    UmiProviderTaskReceipt items[UMI_PROVIDER_TASK_CATALOGUE_LIMIT];
} TaskCatalogue;
static UmiStatus task_catalogue_visit(const char *key,const char *wire,void *context)
{
    TaskCatalogue *catalogue=context;
    if(catalogue->scanned==UMI_PROVIDER_TASK_SCAN_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++catalogue->scanned;
    size_t prefix=strlen(catalogue->store->prefix);
    if(strncmp(key,catalogue->store->prefix,prefix)!=0) return UMI_STATUS_OK;
    const char *id=key+prefix;
    if(!UmiProviderConnectionIdValid(id,UMI_PROVIDER_CONNECTION_ID_CAPACITY)) return UMI_STATUS_PARSE_ERROR;
    if(catalogue->count==UMI_PROVIDER_TASK_CATALOGUE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t length=0U;while(length<UMI_PROVIDER_TASK_WIRE_CAPACITY && wire[length]!='\0') ++length;
    if(length==UMI_PROVIDER_TASK_WIRE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiProviderTaskReceipt *r=&catalogue->items[catalogue->count];
    UmiStatus status=UmiProviderTaskDecode(wire,length,r);
    if(status==UMI_STATUS_OK && (!task_scope(catalogue->store,r) || strcmp(r->context.local_id,id)!=0))
        status=UMI_STATUS_PARSE_ERROR;
    if(status==UMI_STATUS_OK) ++catalogue->count;
    return status;
}
static int task_catalogue_compare(const void *left,const void *right)
{
    const UmiProviderTaskReceipt *a=left,*b=right;
    return strcmp(a->context.local_id,b->context.local_id);
}
static bool task_snapshot_valid(const char *text)
{
    for(size_t i=0U;i<64U;++i) if(!((text[i]>='0' && text[i]<='9') || (text[i]>='a' && text[i]<='f'))) return false;
    return text[64]=='\0';
}
static UmiStatus task_catalogue_snapshot(const TaskCatalogue *catalogue,UmiProviderTaskPhase phase,char *out)
{
    static const char domain[]="Umicom provider task catalogue v1\n";
    UmiSha256 hash;UmiSha256Init(&hash);
    UmiStatus status=UmiSha256Update(&hash,domain,sizeof(domain)-1U);
    char scope[128];int count=snprintf(scope,sizeof(scope),"%s\n%s\n%u\n",
        catalogue->store->application_id,catalogue->store->profile_id,(unsigned)phase);
    if(count<0 || (size_t)count>=sizeof(scope)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(status==UMI_STATUS_OK) status=UmiSha256Update(&hash,scope,(size_t)count);
    for(size_t i=0U;status==UMI_STATUS_OK && i<catalogue->count;++i) {
        char wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];size_t length;
        status=UmiProviderTaskEncode(&catalogue->items[i],wire,sizeof(wire),&length);
        if(status==UMI_STATUS_OK) status=UmiSha256Update(&hash,wire,length);
    }
    unsigned char digest[UMI_SHA256_BYTES];
    if(status==UMI_STATUS_OK) status=UmiSha256Final(&hash,digest);
    if(status==UMI_STATUS_OK) UmiSha256Hex(digest,out);
    return status;
}
static UmiStatus task_catalogue_page(const TaskCatalogue *catalogue,const UmiProviderTaskQuery *query,
    UmiProviderTaskPage *page)
{
    strcpy(page->application_id,catalogue->store->application_id);
    strcpy(page->profile_id,catalogue->store->profile_id);page->phase=query->phase;page->scope_count=catalogue->count;
    UmiStatus status=task_catalogue_snapshot(catalogue,query->phase,page->snapshot);
    if(status!=UMI_STATUS_OK) return status;
    if(query->expected_snapshot[0]!='\0' && strcmp(query->expected_snapshot,page->snapshot)!=0) return UMI_STATUS_BUSY;
    bool cursor_found=query->after_id[0]=='\0',more=false;
    for(size_t i=0U;i<catalogue->count;++i) {
        const UmiProviderTaskReceipt *r=&catalogue->items[i];
        if(query->phase!=0 && query->phase!=r->phase) continue;
        ++page->matching_count;
        if(!cursor_found) { if(strcmp(r->context.local_id,query->after_id)==0) cursor_found=true;continue; }
        if(page->count<query->limit) page->items[page->count++]=*r;else more=true;
    }
    if(!cursor_found) return UMI_STATUS_INVALID_STATE;
    if(more) strcpy(page->next_after_id,page->items[page->count-1U].context.local_id);
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderTasksReadPage(UmiProviderTasks *store,const UmiProviderTaskQuery *query,UmiProviderTaskPage *out)
{
    if(store==NULL || query==NULL || out==NULL || (unsigned)query->phase>UMI_PROVIDER_TASK_OUTPUT_STORED ||
        query->limit==0U || query->limit>UMI_PROVIDER_TASK_PAGE_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    bool continuation=query->after_id[0]!='\0';
    if(continuation!= (query->expected_snapshot[0]!='\0') ||
        (continuation && (!UmiProviderConnectionIdValid(query->after_id,sizeof(query->after_id)) ||
        !task_snapshot_valid(query->expected_snapshot)))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Capture the query before any output write; a caller may retain cursor
     * bytes beside its previous page. Failed allocation never opens a transaction. */
    UmiProviderTaskQuery selected=*query;
    TaskCatalogue *catalogue=calloc(1U,sizeof(*catalogue));UmiProviderTaskPage *page=calloc(1U,sizeof(*page));
    if(catalogue==NULL || page==NULL) { free(catalogue);free(page);return UMI_STATUS_OUT_OF_MEMORY; }
    catalogue->store=store;
    UmiStatus status=umi_mutex_lock(store->mutex);
    if(status==UMI_STATUS_OK) {
        status=UmiProviderStorageBegin(store->server,store->poisoned);
        if(status==UMI_STATUS_OK) {
            status=umi_data_server_visit(store->server,task_catalogue_visit,catalogue);
            if(status==UMI_STATUS_OK) {
                qsort(catalogue->items,catalogue->count,sizeof(catalogue->items[0]),task_catalogue_compare);
                status=task_catalogue_page(catalogue,&selected,page);
            }
            status=UmiProviderStorageFinish(store->server,&store->poisoned,status);
        }
        if(status==UMI_STATUS_OK) *out=*page;
        (void)umi_mutex_unlock(store->mutex);
    }
    free(page);free(catalogue);return status;
}
