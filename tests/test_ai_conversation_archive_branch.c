/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_conversation_archive_branch.c
 *
 * PURPOSE:
 *   Verify native conversation branches preserve source identity, reject stale publication and discard inherited approvals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "ai/conversation_archive_fixture.h"
/* Inject allocation failures and exhausted owner counters without changing the
 * production opaque API. All test setup allocations occur before arming a fault. */
static unsigned allocation_failure;
static void *branch_calloc(size_t count,size_t size)
{
    if(allocation_failure==1U) { allocation_failure=0U;return NULL; }
    return calloc(count,size);
}
static void *branch_malloc(size_t size)
{
    if(allocation_failure==2U) { allocation_failure=0U;return NULL; }
    return malloc(size);
}
#define calloc branch_calloc
#define malloc branch_malloc
#include "../src/ai/conversation_archive.c"
#undef calloc
#undef malloc
#define CASE(name) (strcmp(mode,name)==0)
int main(int argc,char **argv)
{
    assert(argc==2);const char *mode=argv[1];
    static const char *const modes[]={
        "empty","prefix","full","roles","approval","unused-bytes","unrelated-edit","stale-source",
        "removed-source","same-id","existing-destination","zero-revision","range","capacity",
        "last-revision","exhausted-revision","allocation-record","allocation-constructor","invalid-owner",
        "bad-source-id","bad-destination-id","bad-title","empty-title","null-output","null-archive",
        "borrowed-inputs","max-messages","independent-branches"
    };
    int known=0;for(size_t i=0U;i<sizeof(modes)/sizeof(modes[0]);++i) if(CASE(modes[i])) known=1;assert(known);
    UmiAiConversationArchive *archive=NULL;assert(umi_ai_conversation_archive_create(&archive)==UMI_STATUS_OK);
    UmiAiConversationRecord *source=archive_record_fixture("session.source");
    UmiAiConversationRecord *output=calloc(1U,sizeof(*output));assert(output!=NULL);
    UmiAiConversationArchive *before=malloc(sizeof(*before));assert(before!=NULL);
    UmiAiMessage message;
    const UmiAiRole roles[]={UMI_AI_ROLE_ASSISTANT,UMI_AI_ROLE_TOOL,UMI_AI_ROLE_SYSTEM};
    for(size_t i=0U;i<3U;++i) {
        assert(umi_ai_message_set(&message,roles[i],"historical","Historical text; not proof of execution")==UMI_STATUS_OK);
        assert(umi_ai_conversation_add(&source->conversation,&message)==UMI_STATUS_OK);
    }
    source->classification=UMI_AI_DATA_RESTRICTED;source->remote_approved=7;source->persistence_approved=-3;
    if(CASE("unused-bytes")) {
        size_t used=strlen(source->conversation.messages[0].text)+1U;
        memset(source->conversation.messages[0].text+used,'x',sizeof(source->conversation.messages[0].text)-used);
        memset(&source->conversation.messages[4],0xa5,sizeof(source->conversation.messages[4]));
    }
    if(CASE("max-messages")) while(source->conversation.message_count<UMI_AI_MAX_MESSAGES)
        assert(umi_ai_conversation_add(&source->conversation,&message)==UMI_STATUS_OK);
    assert(UmiAiConversationArchiveUpsertIfRevision(archive,source,0U)==UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_find(archive,"session.source",source)==UMI_STATUS_OK);
    uint64_t expected=source->revision;
    const char *source_id="session.source",*new_id="session.branch",*title="Alternative explanation";
    size_t count=2U;UmiStatus wanted=UMI_STATUS_OK;
    if(CASE("empty")) count=0U;
    if(CASE("full") || CASE("roles") || CASE("max-messages")) count=source->conversation.message_count;
    if(CASE("unrelated-edit") || CASE("existing-destination")) {
        UmiAiConversationRecord *other=archive_record_fixture("session.other");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive,other,0U)==UMI_STATUS_OK);
        if(CASE("existing-destination")) { new_id="session.other";wanted=UMI_STATUS_ALREADY_EXISTS; }
        free(other);
    }
    if(CASE("stale-source")) {
        strcpy(source->title,"A newer accepted source");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive,source,expected)==UMI_STATUS_OK);
        wanted=UMI_STATUS_BUSY;
    }
    if(CASE("removed-source")) {
        assert(UmiAiConversationArchiveRemoveIfRevision(archive,source_id,expected)==UMI_STATUS_OK);
        wanted=UMI_STATUS_NOT_FOUND;
    }
    if(CASE("same-id")) { new_id=source_id;wanted=UMI_STATUS_ALREADY_EXISTS; }
    if(CASE("zero-revision")) { expected=0U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    if(CASE("range")) { count=source->conversation.message_count+1U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    if(CASE("capacity")) {
        UmiAiConversationRecord *other=archive_record_fixture("temporary");
        for(size_t i=1U;i<UMI_AI_CONVERSATION_ARCHIVE_CAPACITY;++i) {
            (void)snprintf(other->session.session_id,sizeof(other->session.session_id),"session.%zu",i);
            assert(UmiAiConversationArchiveUpsertIfRevision(archive,other,0U)==UMI_STATUS_OK);
        }
        free(other);wanted=UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if(CASE("last-revision")) archive->revision=UINT64_MAX-1U;
    if(CASE("exhausted-revision")) { archive->revision=UINT64_MAX;wanted=UMI_STATUS_CAPACITY_EXCEEDED; }
    if(CASE("invalid-owner")) { archive->count=UMI_AI_CONVERSATION_ARCHIVE_CAPACITY+1U;wanted=UMI_STATUS_INVALID_STATE; }
    char bad_id[UMI_AI_ID_CAPACITY],bad_title[UMI_AI_SMALL_TEXT_CAPACITY];
    memset(bad_id,'x',sizeof(bad_id));memset(bad_title,'x',sizeof(bad_title));
    if(CASE("bad-source-id")) { source_id=bad_id;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    if(CASE("bad-destination-id")) { new_id=bad_id;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    if(CASE("bad-title")) { title=bad_title;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    if(CASE("empty-title")) { title="";wanted=UMI_STATUS_INVALID_ARGUMENT; }
    if(CASE("null-output") || CASE("null-archive")) wanted=UMI_STATUS_INVALID_ARGUMENT;
    UmiAiConversationBranchReceipt receipt,receipt_before;memset(&receipt,0xa5,sizeof(receipt));
    if(CASE("borrowed-inputs")) {
        strcpy(receipt.source_session_id,source_id);strcpy(receipt.destination_session_id,new_id);
        source_id=receipt.source_session_id;new_id=receipt.destination_session_id;
    }
/* Copy every receipt byte for failure assertions, including implementation padding; structure assignment guarantees member values but need not preserve padding bytes. The former implementation is retained for owner review. */
#if 0
    receipt_before=receipt;memcpy(before,archive,sizeof(*before));
#endif
    memcpy(&receipt_before,&receipt,sizeof(receipt));memcpy(before,archive,sizeof(*before));
    if(CASE("allocation-record")) { allocation_failure=1U;wanted=UMI_STATUS_OUT_OF_MEMORY; }
    if(CASE("allocation-constructor")) { allocation_failure=2U;wanted=UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status=UmiAiConversationArchiveBranch(CASE("null-archive")?NULL:archive,source_id,expected,
        new_id,title,count,99U,CASE("null-output")?NULL:&receipt);
    assert(status==wanted);
    if(wanted!=UMI_STATUS_OK) {
        assert(!memcmp(archive,before,sizeof(*before)));
        assert(!memcmp(&receipt,&receipt_before,sizeof(receipt)));
        if(CASE("allocation-record") || CASE("allocation-constructor")) assert(allocation_failure==0U);
    } else {
        assert(UmiAiConversationArchiveValidate(archive)==UMI_STATUS_OK);
        assert(archive->count==before->count+1U && archive->revision==before->revision+1U);
        for(size_t i=0U;i<before->count;++i) assert(!memcmp(&archive->records[i],&before->records[i],sizeof(*source)));
        assert(!strcmp(receipt.source_session_id,"session.source") && !strcmp(receipt.destination_session_id,"session.branch"));
        assert(receipt.source_revision==expected && receipt.destination_revision==archive->revision && receipt.message_count==count);
        assert(umi_ai_conversation_archive_find(archive,"session.branch",output)==UMI_STATUS_OK);
        assert(!strcmp(output->conversation.conversation_id,"session.branch"));
        assert(!strcmp(output->session.provider_id,source->session.provider_id));
        assert(!strcmp(output->session.model_id,source->session.model_id));
        assert(!strcmp(output->workspace_id,source->workspace_id));
        assert(!strcmp(output->title,title) && output->classification==UMI_AI_DATA_RESTRICTED);
        assert(output->remote_approved==0 && output->persistence_approved==0);
        assert(output->session.created_at_ns==99U && output->updated_at_ns==99U && output->revision==archive->revision);
        assert(output->conversation.message_count==count);
        for(size_t i=0U;i<count;++i) {
            const UmiAiMessage *original=&source->conversation.messages[i],*copied=&output->conversation.messages[i];
            assert(copied->role==original->role && !strcmp(copied->name,original->name) && !strcmp(copied->text,original->text));
            for(size_t j=strlen(copied->text)+1U;j<sizeof(copied->text);++j) assert(copied->text[j]==0);
        }
        UmiAiMessage empty={0};
        for(size_t i=count;i<UMI_AI_MAX_MESSAGES;++i) assert(!memcmp(&output->conversation.messages[i],&empty,sizeof(empty)));
        if(CASE("independent-branches")) {
            UmiAiConversationBranchReceipt second;
            assert(UmiAiConversationArchiveBranch(archive,"session.source",expected,"session.second-branch",
                "Second experiment",1U,100U,&second)==UMI_STATUS_OK);
            assert(second.source_revision==receipt.source_revision && second.destination_revision>receipt.destination_revision);
            assert(umi_ai_conversation_archive_find(archive,"session.branch",source)==UMI_STATUS_OK);
            assert(!memcmp(source,output,sizeof(*source)));
        }
    }
    allocation_failure=0U;free(before);free(source);free(output);umi_ai_conversation_archive_destroy(archive);return 0;
}
#undef CASE
