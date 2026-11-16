/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/conversation_archive/main.c
 *
 * PURPOSE:
 *   Demonstrate offline native conversation branching and selected transcript output through public SDK contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/conversation_transcript.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus add_message(UmiAiConversationRecord *record,UmiAiRole role,const char *text)
{
    UmiAiMessage message;
    UmiStatus status=umi_ai_message_set(&message,role,"",text);
    return status==UMI_STATUS_OK?umi_ai_conversation_add(&record->conversation,&message):status;
}

int main(void)
{
    UmiAiConversationArchive *archive=NULL;
    UmiAiConversationRecord *source=calloc(1U,sizeof(*source));
    UmiAiConversationRecord *branch=calloc(1U,sizeof(*branch));
    char *text=NULL;
    UmiStatus status=UMI_STATUS_OUT_OF_MEMORY;
    const char *step="Allocate owned records";
    if(source==NULL || branch==NULL) goto cleanup;
    step="Construct a synthetic source";
    status=umi_ai_conversation_record_init(source,"session.original","provider.example",
        "model.example","workspace.example","Original explanation",1U);
    if(status!=UMI_STATUS_OK) goto cleanup;
    source->remote_approved=1;source->persistence_approved=1;
    /* This is historical fixture text. It never configures or invokes a model. */
    status=add_message(source,UMI_AI_ROLE_SYSTEM,"SYSTEM CONTEXT OMITTED FROM TRANSCRIPT");
    if(status==UMI_STATUS_OK) status=add_message(source,UMI_AI_ROLE_USER,"Who owns a buffer allocated by malloc?");
    if(status==UMI_STATUS_OK) status=add_message(source,UMI_AI_ROLE_ASSISTANT,
        "The owner releases it with free after all borrowers finish using it.");
    if(status!=UMI_STATUS_OK) goto cleanup;
    step="Create the serialized in-memory archive";
    status=umi_ai_conversation_archive_create(&archive);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=UmiAiConversationArchiveUpsertIfRevision(archive,source,0U);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=umi_ai_conversation_archive_find(archive,"session.original",source);
    if(status!=UMI_STATUS_OK) goto cleanup;
    uint64_t selected_revision=source->revision;
    UmiAiConversationBranchReceipt receipt;
    step="Branch the selected source revision";
    status=UmiAiConversationArchiveBranch(archive,source->session.session_id,selected_revision,
        "session.alternative","Alternative explanation",3U,2U,&receipt);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=umi_ai_conversation_archive_find(archive,receipt.destination_session_id,branch);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=umi_ai_conversation_archive_find(archive,receipt.source_session_id,source);
    if(status!=UMI_STATUS_OK) goto cleanup;
    step="Check independent identity and reset permissions";
    if(source->revision!=selected_revision || source->remote_approved!=1 ||
        source->persistence_approved!=1 || branch->remote_approved!=0 || branch->persistence_approved!=0 ||
        branch->classification!=source->classification || branch->revision!=receipt.destination_revision ||
        strcmp(branch->session.session_id,"session.alternative")!=0 ||
        umi_ai_conversation_archive_count(archive)!=2U) {
        status=UMI_STATUS_INVALID_STATE;goto cleanup;
    }
    /* The transcript is a selected reading view; it is not a saved archive,
     * fresh consent, a provider response or a complete branded document. */
    UmiAiConversationTranscriptOptions selection={0U,branch->conversation.message_count,UMI_AI_TRANSCRIPT_DIALOGUE,0};
    size_t bytes=0U;
    step="Measure selected messages";
    status=UmiAiConversationTranscriptMeasure(branch,&selection,&bytes);
    if(status!=UMI_STATUS_OK) goto cleanup;
    text=malloc(bytes+1U);
    if(text==NULL) { status=UMI_STATUS_OUT_OF_MEMORY;goto cleanup; }
    step="Render the complete literal transcript";
    status=UmiAiConversationTranscriptMarkdown(branch,&selection,text,bytes+1U,&bytes);
    if(status!=UMI_STATUS_OK) goto cleanup;
    if(strstr(text,"SYSTEM CONTEXT")!=NULL || strstr(text,"### Message 2 - user")==NULL ||
        strstr(text,"### Message 3 - assistant")==NULL) {
        status=UMI_STATUS_INVALID_STATE;goto cleanup;
    }
    step="Print synthetic content";
    if(fwrite(text,1U,bytes,stdout)!=bytes || fflush(stdout)!=0) status=UMI_STATUS_IO_ERROR;
cleanup:
    /* The archive owns copies. Keep each heap record and output buffer under
     * one explicit owner, then release them independently on every exit. */
    free(text);free(branch);free(source);umi_ai_conversation_archive_destroy(archive);
    if(status!=UMI_STATUS_OK) fprintf(stderr,"%s: %s\n",step,umi_status_text(status));
    return status==UMI_STATUS_OK?0:1;
}
