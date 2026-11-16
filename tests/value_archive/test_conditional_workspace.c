/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_conditional_workspace.c
 *
 * PURPOSE:
 *   Exercise allocation-free conditional restore against independent bytes, stale state, scratch aliasing and large records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../src/base/conditional_archive_workspace_internal.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct ReviewRecord { uint64_t serial; char label[16]; } ReviewRecord;
static uint64_t ReviewSchema(void) { return UINT64_C(0x7538436f6e647731); }
static size_t ReviewBound(void) { return UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U + 8U + 15U; }
static void ReviewWrite(UmiArchiveWriter *writer, const ReviewRecord *value)
{
    UmiArchiveWriteUnsigned(writer, value->serial);
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
}
static void ReviewRead(UmiArchiveReader *reader, ReviewRecord *value)
{
    value->serial = UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
}
static UmiStatus ReviewValidate(const ReviewRecord *value)
{
    return value != NULL && value->serial != 0U &&
        memchr(value->label, '\0', sizeof(value->label)) != NULL &&
        value->label[0] != '\0' ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus ReviewEncode(const ReviewRecord *, void *, size_t, size_t *);
UmiStatus ReviewDecode(const void *, size_t, ReviewRecord *);
UmiStatus ReviewDecodeWorkspace(const void *, size_t, ReviewRecord *, ReviewRecord *);
UmiStatus ReviewRestore(const void *, size_t, const ReviewRecord *, ReviewRecord *, void *, size_t, ReviewRecord *);

/* Hidden heap use must fail compilation. Encoders and readers remain the same
 * established contracts; only the newly generated restore entry is guarded. */
UMI_DEFINE_VALUE_ARCHIVE(ReviewEncode, ReviewDecode, ReviewRecord,
    ReviewSchema, ReviewBound, ReviewWrite, ReviewRead, ReviewValidate)
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(ReviewDecodeWorkspace, ReviewRecord,
    ReviewSchema, ReviewBound, ReviewRead, ReviewValidate)
#define malloc UMICOM_CONDITIONAL_WORKSPACE_MUST_NOT_MALLOC
#define calloc UMICOM_CONDITIONAL_WORKSPACE_MUST_NOT_CALLOC
#define realloc UMICOM_CONDITIONAL_WORKSPACE_MUST_NOT_REALLOC
#define free UMICOM_CONDITIONAL_WORKSPACE_MUST_NOT_FREE
UMI_DEFINE_CONDITIONAL_ARCHIVE_WORKSPACE(ReviewRestore, ReviewEncode,
    ReviewDecodeWorkspace, ReviewRecord, ReviewSchema, ReviewBound)
#undef free
#undef realloc
#undef calloc
#undef malloc
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static int SmallCases(const char *name)
{
    ReviewRecord expected = {7U, "old"}, value = {7U, "old"};
    ReviewRecord incoming = {9U, "new"}, workspace;
    unsigned char archive[64], comparison[128], before[sizeof(value)];
    size_t count = 0U, measured = 0U;
    CHECK(ReviewEncode(&incoming, archive, sizeof(archive), &count) == UMI_STATUS_OK);
    CHECK(ReviewEncode(&expected, NULL, 0U, &measured) == UMI_STATUS_OK);
    memset(&workspace, 0xa5, sizeof(workspace));
    memset(comparison, 0xc3, sizeof(comparison));
    if (strcmp(name, "padding") == 0) {
        memset(&value, 0xe5, sizeof(value));
        value.serial = 7U;
        memcpy(value.label, "old", sizeof("old"));
    }
    if (strcmp(name, "stale") == 0) value.serial = 8U;
    if (strcmp(name, "size-mismatch") == 0) memcpy(value.label, "longer", sizeof("longer"));
    if (strcmp(name, "invalid-expected") == 0) memset(expected.label, 'x', sizeof(expected.label));
    if (strcmp(name, "invalid-current") == 0) value.serial = 0U;
    if (strcmp(name, "checksum") == 0) archive[count-1U] ^= 1U;
    if (strcmp(name, "schema") == 0) UmiArchiveFinish(archive, count, ReviewSchema()+1U);
    if (strcmp(name, "domain") == 0) {
        UmiArchiveStoreInteger(archive+UMI_VALUE_ARCHIVE_HEADER_SIZE, 0U, 8U);
        UmiArchiveFinish(archive, count, ReviewSchema());
    }
    if (strcmp(name, "trailing") == 0) { archive[count++]=0U; UmiArchiveFinish(archive,count,ReviewSchema()); }
    memcpy(before, &value, sizeof(value));
    UmiStatus status;
    if (strcmp(name, "nulls") == 0) {
        CHECK(ReviewRestore(NULL,count,&expected,&workspace,comparison,sizeof(comparison),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,NULL,&workspace,comparison,sizeof(comparison),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,NULL,comparison,sizeof(comparison),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,&workspace,NULL,0U,&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,&workspace,comparison,sizeof(comparison),NULL)==UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "aliases") == 0) {
        /* Every writable/input conflict is refused before scratch is modified. */
        CHECK(ReviewRestore(archive,count,&expected,&value,comparison,sizeof(comparison),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,&expected,comparison,sizeof(comparison),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,&workspace,&expected,sizeof(expected),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,&workspace,&value,sizeof(value),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,&workspace,&workspace,sizeof(workspace),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,&workspace,archive,sizeof(archive),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore((unsigned char *)&value+1U,1U,&expected,&workspace,comparison,sizeof(comparison),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore((unsigned char *)&expected+1U,1U,&expected,&workspace,comparison,sizeof(comparison),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore((unsigned char *)&workspace+1U,1U,&expected,&workspace,comparison,sizeof(comparison),&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before,&value,sizeof(value))==0);
        return 0;
    }
    if (strcmp(name, "partial-alias") == 0) {
        _Alignas(ReviewRecord) unsigned char arena[2U*sizeof(ReviewRecord)];
        memset(arena,0xa5,sizeof(arena));
        ReviewRecord *first=(ReviewRecord *)(void *)arena;
        ReviewRecord *second=(ReviewRecord *)(void *)(arena+_Alignof(ReviewRecord));
        CHECK(ReviewRestore(archive,count,first,&workspace,comparison,sizeof(comparison),second)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ReviewRestore(archive,count,&expected,first,comparison,sizeof(comparison),second)==UMI_STATUS_INVALID_ARGUMENT);
        for(size_t i=0U;i<sizeof(arena);++i)CHECK(arena[i]==0xa5U);
        return 0;
    }
    size_t capacity=strcmp(name,"capacity")==0?measured*2U-1U:measured*2U;
    const ReviewRecord *review=strcmp(name,"self")==0?&value:&expected;
    status=ReviewRestore(archive,count,review,&workspace,comparison,capacity,&value);
    if(strcmp(name,"success")==0||strcmp(name,"padding")==0||strcmp(name,"self")==0){
        CHECK(status==UMI_STATUS_OK);
        CHECK(value.serial==9U&&strcmp(value.label,"new")==0);
        value.serial=19U;
        CHECK(workspace.serial==9U);
        return 0;
    }
    if(strcmp(name,"stale")==0||strcmp(name,"size-mismatch")==0)CHECK(status==UMI_STATUS_INVALID_STATE);
    else if(strcmp(name,"capacity")==0) {
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        for(size_t i=0U;i<sizeof(comparison);++i)CHECK(comparison[i]==0xc3U);
    }
    else if(strcmp(name,"invalid-expected")==0||strcmp(name,"invalid-current")==0||strcmp(name,"domain")==0)CHECK(status==UMI_STATUS_INVALID_ARGUMENT);
    else if(strcmp(name,"schema")==0)CHECK(status==UMI_STATUS_UNAVAILABLE);
    else if(strcmp(name,"checksum")==0||strcmp(name,"trailing")==0)CHECK(status==UMI_STATUS_PARSE_ERROR);
    else return 2;
    CHECK(memcmp(before,&value,sizeof(value))==0);
    return 0;
}

/* A multi-megabyte inline record exposes accidental automatic candidate copies.
 * Explicit fixture allocations are outside the guarded implementation. */
typedef struct LargeReview { uint64_t serial; unsigned char bytes[2U*1024U*1024U]; } LargeReview;
static uint64_t LargeSchema(void){return UINT64_C(0x7538436f6e644c67);}
static size_t LargeBound(void){return UMI_VALUE_ARCHIVE_HEADER_SIZE+8U+sizeof(((LargeReview *)0)->bytes);}
static UmiStatus LargeValidate(const LargeReview *v){return v->serial!=0U?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
static void LargeRead(UmiArchiveReader *r,LargeReview *v){
    v->serial=UmiArchiveReadUnsigned(r,UINT64_MAX);
    const void *p=UmiArchiveReadBytes(r,sizeof(v->bytes));
    if(p!=NULL)memcpy(v->bytes,p,sizeof(v->bytes));
}
static UmiStatus LargeEncode(const LargeReview *v,void *bytes,size_t capacity,size_t *count){
    if(v==NULL||count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(v->serial==0U)return UMI_STATUS_INVALID_ARGUMENT;
    *count=LargeBound();
    if(bytes==NULL)return capacity==0U?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
    if(capacity<*count)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiArchiveWriter w={(unsigned char *)bytes,capacity,UMI_VALUE_ARCHIVE_HEADER_SIZE,UMI_STATUS_OK};
    UmiArchiveWriteUnsigned(&w,v->serial);
    UmiArchiveWriteBytes(&w,v->bytes,sizeof(v->bytes));
    UmiArchiveFinish(w.bytes,w.offset,LargeSchema());
    return w.status;
}
UmiStatus LargeDecodeWorkspace(const void *,size_t,LargeReview *,LargeReview *);
UmiStatus LargeRestore(const void *,size_t,const LargeReview *,LargeReview *,void *,size_t,LargeReview *);
#define malloc UMICOM_CONDITIONAL_WORKSPACE_MUST_NOT_MALLOC
#define calloc UMICOM_CONDITIONAL_WORKSPACE_MUST_NOT_CALLOC
#define realloc UMICOM_CONDITIONAL_WORKSPACE_MUST_NOT_REALLOC
#define free UMICOM_CONDITIONAL_WORKSPACE_MUST_NOT_FREE
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(LargeDecodeWorkspace,LargeReview,LargeSchema,LargeBound,LargeRead,LargeValidate)
UMI_DEFINE_CONDITIONAL_ARCHIVE_WORKSPACE(LargeRestore,LargeEncode,LargeDecodeWorkspace,LargeReview,LargeSchema,LargeBound)
#undef free
#undef realloc
#undef calloc
#undef malloc
static int LargeCase(void){
    int result=1;
    size_t n=LargeBound(),written=0U;
    LargeReview *value=calloc(1,sizeof(*value)),*expected=calloc(1,sizeof(*expected)),*workspace=malloc(sizeof(*workspace));
    unsigned char *bytes=malloc(n),*comparison=malloc(n*2U);
    if(!value||!expected||!workspace||!bytes||!comparison)goto done;
    value->serial=expected->serial=1U;
    if(LargeEncode(value,bytes,n,&written)!=UMI_STATUS_OK)goto done;
    UmiArchiveStoreInteger(bytes+UMI_VALUE_ARCHIVE_HEADER_SIZE,2U,8U);
    bytes[n-1U]=0x7bU;
    UmiArchiveFinish(bytes,n,LargeSchema());
    if(LargeRestore(bytes,n,expected,workspace,comparison,n*2U,value)!=UMI_STATUS_OK)goto done;
    if(value->serial!=2U||value->bytes[sizeof(value->bytes)-1U]!=0x7bU||expected->serial!=1U)goto done;
    result=0;
done:
    free(comparison);free(bytes);free(workspace);free(expected);free(value);
    return result;
}
int main(int argc,char **argv){
    if(argc!=2)return 2;
    return strcmp(argv[1],"large")==0?LargeCase():SmallCases(argv[1]);
}
