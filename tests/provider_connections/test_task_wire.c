/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_task_wire.c
 *
 * PURPOSE:
 *   Verify canonical provider receipt bytes, strict decoding bounds and failure output preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "../../src/provider_connections/internal.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Independent byte oracle: fixed logical fields and a separately computed
 * connection fingerprint, never produced by the native codec under test. */
static const char oracle[]=
    "provider-task\n"
    "1\n"
    "1\n"
    "7\n"
    "1\n"
    "0\n"
    "6d65646961\n"
    "6f776e6572\n"
    "7363656e652d6f6e65\n"
    "766964656f2e67656e6572617465\n"
    "766964656f\n"
    "6578616d706c65\n"
    "\n"
    "2a00000000000000000000000000000000000000000000000000000000000000\n"
    "d5448990dd1dc614d1945a9ae1249afd4a7a7051265ec02e9f2d2aa372866173\n"
    "0000000000000000000000000000000000000000000000000000000000000000\n";

static UmiProviderTaskReceipt fixture(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"video");strcpy(c.provider_id,"example");
    strcpy(c.label,"Example video");strcpy(c.endpoint,"https://api.example.invalid/tasks");
    strcpy(c.model,"video-model");strcpy(c.secret_reference,"vault:video-key");
    c.route=UMI_PROVIDER_CONNECTION_HTTPS;c.timeout_ms=30000U;c.enabled=true;
    UmiProviderTaskContext context={0};strcpy(context.application_id,"media");
    strcpy(context.profile_id,"owner");strcpy(context.local_id,"scene-one");
    strcpy(context.operation_id,"video.generate");context.request_sha256[0]=42U;
    UmiProviderTaskReceipt r;assert(UmiProviderTaskPrepare(&context,&c,7U,&r)==UMI_STATUS_OK);return r;
}
static void decode_failure(const char *wire,size_t length,UmiStatus expected)
{
    UmiProviderTaskReceipt out;memset(&out,0xa5,sizeof(out));
    unsigned char before[sizeof(out)];memcpy(before,&out,sizeof(out));
    assert(UmiProviderTaskDecode(wire,length,&out)==expected);
    assert(memcmp(before,&out,sizeof(out))==0);
}
/* Alter one fixture line, not a copy of the production parser. Line zero is
 * the tag; lines one through five are schema, revisions, phase and bytes. */
static void replace_line(char *wire,size_t capacity,unsigned line,const char *text)
{
    char *start=wire;
    for(unsigned i=0U;i<line;++i) { start=strchr(start,'\n');assert(start!=NULL);++start; }
    char *end=strchr(start,'\n');assert(end!=NULL);
    size_t prefix=(size_t)(start-wire),tail=strlen(end)+1U,replacement=strlen(text);
    assert(prefix+replacement+tail<=capacity);
    memmove(start+replacement,end,tail);memcpy(start,text,replacement);
}
static void roundtrip(const UmiProviderTaskReceipt *r)
{
    char wire[UMI_PROVIDER_TASK_WIRE_CAPACITY],again[UMI_PROVIDER_TASK_WIRE_CAPACITY];
    size_t length=0U,other=0U;UmiProviderTaskReceipt decoded;
    assert(UmiProviderTaskEncode(r,wire,sizeof(wire),&length)==UMI_STATUS_OK);
    assert(length==strlen(wire));
    assert(UmiProviderTaskDecode(wire,length,&decoded)==UMI_STATUS_OK);
    assert(decoded.revision==r->revision && decoded.connection_revision==r->connection_revision);
    assert(decoded.phase==r->phase && decoded.output_bytes==r->output_bytes);
    assert(UmiProviderTaskEncode(&decoded,again,sizeof(again),&other)==UMI_STATUS_OK);
    assert(other==length && memcmp(wire,again,length+1U)==0);
}
int main(int argc,char **argv)
{
    assert(argc==2);const char *mode=argv[1];UmiProviderTaskReceipt r=fixture(),decoded;
    char wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];size_t length=0U;
    if(strcmp(mode,"oracle")==0) {
        assert(UmiProviderTaskEncode(&r,wire,sizeof(wire),&length)==UMI_STATUS_OK);
        assert(length==sizeof(oracle)-1U && memcmp(wire,oracle,sizeof(oracle))==0);
        assert(UmiProviderTaskDecode(oracle,sizeof(oracle)-1U,&decoded)==UMI_STATUS_OK);
        assert(strcmp(decoded.context.operation_id,"video.generate")==0);
        assert(decoded.context.request_sha256[0]==42U);return 0;
    }
    if(strcmp(mode,"phases")==0) {
        for(unsigned phase=1U;phase<=7U;++phase) {
            r=fixture();r.phase=(UmiProviderTaskPhase)phase;r.revision=phase;
            if(phase>=3U) strcpy(r.remote_id,"Remote-123");
            if(phase==7U) { r.output_bytes=42U;r.output_sha256[0]=9U; }
            roundtrip(&r);
        }return 0;
    }
    if(strcmp(mode,"wide-integers")==0) {
        r.revision=UINT64_MAX;r.connection_revision=UINT64_MAX;r.output_bytes=UINT64_MAX;
        r.phase=UMI_PROVIDER_TASK_OUTPUT_STORED;strcpy(r.remote_id,"Remote-123");
        for(size_t i=0U;i<UMI_SHA256_BYTES;++i) r.output_sha256[i]=(unsigned char)(i*8U);
        roundtrip(&r);return 0;
    }
    if(strcmp(mode,"truncation")==0) {
        for(size_t i=0U;i<sizeof(oracle)-1U;++i) decode_failure(oracle,i,UMI_STATUS_PARSE_ERROR);
        return 0;
    }
    if(strcmp(mode,"schema")==0) {
        const char *schemas[]={"0","2","18446744073709551615"};
        for(size_t i=0U;i<sizeof(schemas)/sizeof(schemas[0]);++i) {
            strcpy(wire,oracle);replace_line(wire,sizeof(wire),1U,schemas[i]);
            decode_failure(wire,strlen(wire),UMI_STATUS_NOT_IMPLEMENTED);
        }return 0;
    }
    if(strcmp(mode,"numbers")==0) {
        const char *bad[]={"","01","+1","-1"," 1","1 ","1.0","1e0","18446744073709551616"};
        for(unsigned line=1U;line<=5U;++line) for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) {
            strcpy(wire,oracle);replace_line(wire,sizeof(wire),line,bad[i]);
            decode_failure(wire,strlen(wire),UMI_STATUS_PARSE_ERROR);
        }return 0;
    }
    if(strcmp(mode,"extent")==0) {
        strcpy(wire,oracle);wire[30]='\0';decode_failure(wire,sizeof(oracle)-1U,UMI_STATUS_PARSE_ERROR);
        strcpy(wire,oracle);strcat(wire,"extra\n");decode_failure(wire,strlen(wire),UMI_STATUS_PARSE_ERROR);
        decode_failure(oracle,sizeof(oracle),UMI_STATUS_PARSE_ERROR);
        memset(wire,'x',sizeof(wire));decode_failure(wire,sizeof(wire),UMI_STATUS_CAPACITY_EXCEEDED);
        strcpy(wire,oracle);wire[0]='P';decode_failure(wire,strlen(wire),UMI_STATUS_PARSE_ERROR);return 0;
    }
    if(strcmp(mode,"digests")==0) {
        char digest[65];memset(digest,'0',64U);digest[64]='\0';
        for(unsigned line=13U;line<=15U;++line) {
            for(unsigned variant=0U;variant<4U;++variant) {
                digest[0]='0';digest[63]='0';digest[64]='\0';
                if(variant==0U) digest[0]='A';
                if(variant==1U) digest[0]='g';
                if(variant==2U) digest[63]='\0';
                if(variant==3U) digest[0]=' ';
                strcpy(wire,oracle);replace_line(wire,sizeof(wire),line,digest);
                decode_failure(wire,strlen(wire),UMI_STATUS_PARSE_ERROR);
            }
        }return 0;
    }
    if(strcmp(mode,"text")==0) {
        const char *bad[]={"00","6D65646961","0","zz","","612f62"};
        for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) {
            strcpy(wire,oracle);replace_line(wire,sizeof(wire),6U,bad[i]);
            decode_failure(wire,strlen(wire),UMI_STATUS_PARSE_ERROR);
        }
        char too_long[99];memset(too_long,'6',98U);too_long[98]='\0';
        strcpy(wire,oracle);replace_line(wire,sizeof(wire),6U,too_long);
        decode_failure(wire,strlen(wire),UMI_STATUS_PARSE_ERROR);return 0;
    }
    if(strcmp(mode,"invariants")==0) {
        const unsigned lines[]={2U,3U,4U,4U,4U,5U,12U,15U};
        const char *values[]={"0","0","0","8","3","1","52656d6f7465",
            "0100000000000000000000000000000000000000000000000000000000000000"};
        for(size_t i=0U;i<sizeof(lines)/sizeof(lines[0]);++i) {
            strcpy(wire,oracle);replace_line(wire,sizeof(wire),lines[i],values[i]);
            decode_failure(wire,strlen(wire),UMI_STATUS_PARSE_ERROR);
        }return 0;
    }
    if(strcmp(mode,"capacity")==0) {
        char before[sizeof(wire)];memset(wire,0xa5,sizeof(wire));memcpy(before,wire,sizeof(wire));
        length=999U;
        for(size_t capacity=1U;capacity<=sizeof(oracle)-1U;++capacity) {
            assert(UmiProviderTaskEncode(&r,wire,capacity,&length)==UMI_STATUS_CAPACITY_EXCEEDED);
            assert(length==999U && memcmp(wire,before,sizeof(wire))==0);
        }
        assert(UmiProviderTaskEncode(&r,wire,sizeof(oracle),&length)==UMI_STATUS_OK);
        assert(length==sizeof(oracle)-1U && memcmp(wire,oracle,sizeof(oracle))==0);
        assert(memcmp(wire+sizeof(oracle),before+sizeof(oracle),sizeof(wire)-sizeof(oracle))==0);return 0;
    }
    if(strcmp(mode,"padding")==0) {
        UmiProviderTaskReceipt dirty;memset(&dirty,0xcc,sizeof(dirty));
        dirty.schema=r.schema;dirty.revision=r.revision;dirty.connection_revision=r.connection_revision;
        dirty.phase=r.phase;dirty.output_bytes=r.output_bytes;
        strcpy(dirty.context.application_id,r.context.application_id);strcpy(dirty.context.profile_id,r.context.profile_id);
        strcpy(dirty.context.local_id,r.context.local_id);strcpy(dirty.context.operation_id,r.context.operation_id);
        strcpy(dirty.connection_id,r.connection_id);strcpy(dirty.provider_id,r.provider_id);strcpy(dirty.remote_id,r.remote_id);
        memcpy(dirty.context.request_sha256,r.context.request_sha256,UMI_SHA256_BYTES);
        memcpy(dirty.connection_fingerprint,r.connection_fingerprint,UMI_SHA256_BYTES);
        memcpy(dirty.output_sha256,r.output_sha256,UMI_SHA256_BYTES);
        assert(UmiProviderTaskEncode(&dirty,wire,sizeof(wire),&length)==UMI_STATUS_OK);
        assert(length==sizeof(oracle)-1U && memcmp(wire,oracle,sizeof(oracle))==0);return 0;
    }
    if(strcmp(mode,"invalid-input")==0) {
        char before[sizeof(wire)];memset(wire,0xa5,sizeof(wire));memcpy(before,wire,sizeof(wire));length=999U;
        r.schema=2U;assert(UmiProviderTaskEncode(&r,wire,sizeof(wire),&length)==UMI_STATUS_INVALID_ARGUMENT);
        assert(length==999U && memcmp(wire,before,sizeof(wire))==0);
        r=fixture();assert(UmiProviderTaskEncode(NULL,wire,sizeof(wire),&length)==UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiProviderTaskEncode(&r,NULL,sizeof(wire),&length)==UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiProviderTaskEncode(&r,wire,0U,&length)==UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiProviderTaskEncode(&r,wire,sizeof(wire),NULL)==UMI_STATUS_INVALID_ARGUMENT);
        assert(length==999U && memcmp(wire,before,sizeof(wire))==0);
        decode_failure(NULL,0U,UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiProviderTaskDecode(oracle,sizeof(oracle)-1U,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    if(strcmp(mode,"not-authentication")==0) {
/* The prior changed digest contained 66 hexadecimal digits; retain strict 64-digit decoding and correct the fixture. The former implementation is retained for owner review. */
#if 0
        strcpy(wire,oracle);replace_line(wire,sizeof(wire),13U,
            "2b0000000000000000000000000000000000000000000000000000000000000000");
#endif
        /* Change one byte without changing the independently specified digest
         * width. The codec must accept a different digest, not malformed hex. */
        static const char changed_digest[] = "2b"
            "00000000000000000000000000000000000000000000000000000000000000";
        _Static_assert(sizeof changed_digest == UMI_SHA256_HEX_CAPACITY, "SHA-256 hex width");
        strcpy(wire,oracle);replace_line(wire,sizeof(wire),13U,changed_digest);
        /* A consistent changed digest is still parseable. Authentication and
         * trusted storage ownership cannot be inferred from syntax checks. */
        assert(UmiProviderTaskDecode(wire,strlen(wire),&decoded)==UMI_STATUS_OK);
        assert(decoded.context.request_sha256[0]==43U);return 0;
    }
    return 2;
}
