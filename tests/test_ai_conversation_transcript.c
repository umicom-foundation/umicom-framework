/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_conversation_transcript.c
 *
 * PURPOSE:
 *   Verify literal transcript formatting, selected disclosure, Unicode admission and unchanged outputs on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "ai/conversation_archive_fixture.h"
#include "umicom/ai/conversation_transcript.h"

static void transcript_message(UmiAiConversationRecord *record,UmiAiRole role,
    const char *name,const char *text)
{
    UmiAiMessage message;
    assert(umi_ai_message_set(&message,role,name,text)==UMI_STATUS_OK);
    assert(umi_ai_conversation_add(&record->conversation,&message)==UMI_STATUS_OK);
}

static void unchanged_failure(const UmiAiConversationRecord *record,
    const UmiAiConversationTranscriptOptions *options,UmiStatus expected,char *output,
    const char *before,size_t capacity)
{
    size_t bytes=991U;
    assert(UmiAiConversationTranscriptMeasure(record,options,&bytes)==expected);
    assert(bytes==991U);
    assert(UmiAiConversationTranscriptMarkdown(record,options,output,capacity,&bytes)==expected);
    assert(bytes==991U);
    assert(memcmp(output,before,UMI_AI_TRANSCRIPT_MAX_BYTES+2U)==0);
}

int main(int argc,char **argv)
{
    assert(argc==2);
    const char *mode=argv[1];
    UmiAiConversationRecord *record=archive_record_fixture("private.session.identifier");
    UmiAiConversationRecord *before_record=malloc(sizeof(*before_record));
    const size_t allocation=UMI_AI_TRANSCRIPT_MAX_BYTES+2U;
    char *output=malloc(allocation),*before=malloc(allocation);
    assert(before_record!=NULL && output!=NULL && before!=NULL);
    memset(output,0xa5,allocation);memcpy(before,output,allocation);
    strcpy(record->title,"Review");record->classification=UMI_AI_DATA_CONFIDENTIAL;
    record->conversation.message_count=0U;
    transcript_message(record,UMI_AI_ROLE_SYSTEM,"hidden.system.name","PRIVATE SYSTEM CONTEXT");
    transcript_message(record,UMI_AI_ROLE_USER,"reader","Hello");
    transcript_message(record,UMI_AI_ROLE_ASSISTANT,"writer","World");
    transcript_message(record,UMI_AI_ROLE_TOOL,"hidden.tool.name","PRIVATE TOOL RESULT");
    UmiAiConversationTranscriptOptions options={0U,4U,UMI_AI_TRANSCRIPT_DIALOGUE,0};
    UmiStatus expected=UMI_STATUS_OK;
    int special=0;
    size_t capacity=allocation;
    if(strcmp(mode,"golden")==0) { options.first_message=1U;options.message_count=1U; }
    else if(strcmp(mode,"dialogue")==0) { }
    else if(strcmp(mode,"system-tool")==0) options.roles=UMI_AI_TRANSCRIPT_SYSTEM|UMI_AI_TRANSCRIPT_TOOL;
    else if(strcmp(mode,"range")==0) { options.first_message=2U;options.message_count=1U; }
    else if(strcmp(mode,"empty-range")==0) { options.first_message=4U;options.message_count=0U; }
    else if(strcmp(mode,"empty-text")==0) record->conversation.messages[1].text[0]='\0';
    else if(strcmp(mode,"names")==0) options.include_message_names=1;
    else if(strcmp(mode,"hidden-names")==0) {
        record->conversation.messages[1].name[0]=(char)0xff;record->conversation.messages[1].name[1]='\0';
    }
    else if(strcmp(mode,"title-fences")==0) strcpy(record->title,"Heading\n~~~\n# injected\n~~~~~");
    else if(strcmp(mode,"message-fences")==0) strcpy(record->conversation.messages[1].text,"~~~\n<script>text</script>\n~~~~~\n# heading");
    else if(strcmp(mode,"name-fences")==0) {
        options.include_message_names=1;strcpy(record->conversation.messages[1].name,"~~~\n~~~~~\nname");
    }
    else if(strcmp(mode,"unicode")==0) strcpy(record->conversation.messages[1].text,"caf\303\251 \360\237\214\215");
    else if(strcmp(mode,"line-endings")==0) strcpy(record->conversation.messages[1].text,"A\r\nB\n\tC\r");
    else if(strcmp(mode,"control-title")==0) { strcpy(record->title,"bad\033[31m");expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"control-message")==0) { strcpy(record->conversation.messages[1].text,"bad\177");expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"control-name")==0) { options.include_message_names=1;strcpy(record->conversation.messages[1].name,"bad\001");expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"unicode-title")==0) { strcpy(record->title,"\300\257");expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"unicode-message")==0) { strcpy(record->conversation.messages[1].text,"\355\240\200");expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"unicode-name")==0) { options.include_message_names=1;strcpy(record->conversation.messages[1].name,"\360\237");expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"excluded-encoding")==0) {
        strcpy(record->conversation.messages[0].text,"\377");strcpy(record->conversation.messages[3].text,"\033");
    }
    else if(strcmp(mode,"zero-mask")==0) { options.roles=0U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"unknown-mask")==0) { options.roles=UMI_AI_TRANSCRIPT_ALL|16U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"range-overflow")==0) { options.first_message=3U;options.message_count=2U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"start-overflow")==0) { options.first_message=SIZE_MAX;options.message_count=0U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"count-overflow")==0) { options.message_count=SIZE_MAX;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"unterminated")==0) { memset(record->title,'x',sizeof(record->title));expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"bad-role")==0) { record->conversation.messages[1].role=(UmiAiRole)0;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"bad-classification")==0) { record->classification=(UmiAiDataClassification)0;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"invalid-hidden-record")==0) {
        memset(record->conversation.messages[0].name,'x',sizeof(record->conversation.messages[0].name));expected=UMI_STATUS_INVALID_ARGUMENT;
    }
    else if(strcmp(mode,"null-record")==0 || strcmp(mode,"null-options")==0 ||
        strcmp(mode,"null-output")==0 || strcmp(mode,"null-size")==0 ||
        strcmp(mode,"short-buffer")==0 || strcmp(mode,"zero-capacity")==0) special=1;
    else if(strcmp(mode,"exact-capacity")==0) { }
    else if(strcmp(mode,"maximum")==0) {
        char text[UMI_AI_TEXT_CAPACITY],name[UMI_AI_ID_CAPACITY];
        memset(text,'~',sizeof(text)-1U);text[sizeof(text)-1U]='\0';
        memset(name,'~',sizeof(name)-1U);name[sizeof(name)-1U]='\0';
        memset(record->title,'~',sizeof(record->title)-1U);record->title[sizeof(record->title)-1U]='\0';
        record->conversation.message_count=0U;
        for(size_t i=0U;i<UMI_AI_MAX_MESSAGES;++i) transcript_message(record,UMI_AI_ROLE_USER,name,text);
        options.message_count=UMI_AI_MAX_MESSAGES;options.include_message_names=1;
    }
    else if(strcmp(mode,"tail-bytes")==0) {
        memset(record->conversation.messages[1].text+6,'Z',sizeof(record->conversation.messages[1].text)-6U);
        memset(&record->conversation.messages[4],0xff,sizeof(record->conversation.messages[4]));
    }
    else if(strcmp(mode,"metadata-omitted")==0) {
        record->remote_approved=1;record->persistence_approved=1;record->revision=UINT64_MAX;
    }
    else assert(!"unknown transcript case");
    memcpy(before_record,record,sizeof(*record));
    if(expected!=UMI_STATUS_OK) unchanged_failure(record,&options,expected,output,before,capacity);
    else if(special) {
        size_t bytes=991U;
        if(strcmp(mode,"null-record")==0) unchanged_failure(NULL,&options,UMI_STATUS_INVALID_ARGUMENT,output,before,capacity);
        else if(strcmp(mode,"null-options")==0) unchanged_failure(record,NULL,UMI_STATUS_INVALID_ARGUMENT,output,before,capacity);
        else if(strcmp(mode,"null-output")==0) {
            assert(UmiAiConversationTranscriptMarkdown(record,&options,NULL,capacity,&bytes)==UMI_STATUS_INVALID_ARGUMENT);
            assert(bytes==991U);
        } else if(strcmp(mode,"null-size")==0) {
            assert(UmiAiConversationTranscriptMeasure(record,&options,NULL)==UMI_STATUS_INVALID_ARGUMENT);
            assert(UmiAiConversationTranscriptMarkdown(record,&options,output,capacity,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        } else {
            size_t required=0U;
            assert(UmiAiConversationTranscriptMeasure(record,&options,&required)==UMI_STATUS_OK);
            capacity=strcmp(mode,"zero-capacity")==0?0U:required;
            assert(UmiAiConversationTranscriptMarkdown(record,&options,output,capacity,&bytes)==UMI_STATUS_CAPACITY_EXCEEDED);
            assert(bytes==991U);
        }
        assert(memcmp(output,before,allocation)==0);
    } else {
        size_t measured=0U,bytes=991U;
        assert(UmiAiConversationTranscriptMeasure(record,&options,&measured)==UMI_STATUS_OK);
        assert(measured>0U && measured<=UMI_AI_TRANSCRIPT_MAX_BYTES);
        if(strcmp(mode,"exact-capacity")==0) capacity=measured+1U;
        assert(UmiAiConversationTranscriptMarkdown(record,&options,output,capacity,&bytes)==UMI_STATUS_OK);
        assert(bytes==measured && strlen(output)==measured && output[measured]=='\0');
        assert(memcmp(output+measured+1U,before+measured+1U,allocation-measured-1U)==0);
        assert(strstr(output,"Classification: confidential\n")!=NULL);
        assert(strstr(output,"private.session.identifier")==NULL && strstr(output,"provider.local")==NULL);
        assert(strstr(output,"model.chat")==NULL && strstr(output,"workspace.fixture")==NULL);
        if(strcmp(mode,"golden")==0) {
            const char *golden="## Conversation transcript\n\nClassification: confidential\n\n### Title\n\n"
                "~~~text\nReview\n~~~\n\n### Message 2 - user\n\nText:\n\n~~~text\nHello\n~~~\n\n";
            assert(strcmp(output,golden)==0);
        }
        if(options.roles==UMI_AI_TRANSCRIPT_DIALOGUE) {
            assert(strstr(output,"PRIVATE SYSTEM CONTEXT")==NULL && strstr(output,"PRIVATE TOOL RESULT")==NULL);
            assert(strstr(output,"hidden.system.name")==NULL && strstr(output,"hidden.tool.name")==NULL);
        }
        if(strcmp(mode,"dialogue")==0) assert(strstr(output,"Message 2 - user")!=NULL && strstr(output,"Message 3 - assistant")!=NULL);
        if(strcmp(mode,"system-tool")==0) {
            assert(strstr(output,"Message 1 - system")!=NULL && strstr(output,"Message 4 - tool")!=NULL);
            assert(strstr(output,"Hello")==NULL && strstr(output,"World")==NULL);
        }
        if(strcmp(mode,"range")==0) assert(strstr(output,"Message 3 - assistant")!=NULL && strstr(output,"Message 2")==NULL);
        if(strcmp(mode,"empty-range")==0) assert(strstr(output,"### Message")==NULL);
        if(strcmp(mode,"empty-text")==0) assert(strstr(output,"Text:\n\n~~~text\n\n~~~\n")!=NULL);
        if(strcmp(mode,"names")==0) assert(strstr(output,"Name:\n\n~~~text\nreader\n~~~\n")!=NULL);
        if(strcmp(mode,"hidden-names")==0) assert(strstr(output,"Name:")==NULL);
        if(strstr(mode,"fences")!=NULL) assert(strstr(output,"~~~~~~text\n")!=NULL && strstr(output,"\n~~~~~~\n\n")!=NULL);
        if(strcmp(mode,"unicode")==0) assert(strstr(output,"caf\303\251 \360\237\214\215")!=NULL);
        if(strcmp(mode,"line-endings")==0) assert(strstr(output,"A\r\nB\n\tC\r\n~~~")!=NULL);
        if(strcmp(mode,"maximum")==0) assert(strstr(output,"Message 64 - user")!=NULL);
        if(strcmp(mode,"tail-bytes")==0) assert(strstr(output,"ZZZ")==NULL);
        if(strcmp(mode,"metadata-omitted")==0) assert(strstr(output,"approved")==NULL && strstr(output,"18446744073709551615")==NULL);
    }
    assert(memcmp(record,before_record,sizeof(*record))==0);
    free(before);free(output);free(before_record);free(record);
    return 0;
}
