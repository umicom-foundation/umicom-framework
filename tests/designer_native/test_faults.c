/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Linker fault injection is confined to this Linux test executable. */
#define _POSIX_C_SOURCE 200809L
#include "umicom/designer/native_project.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while(0)
void *__real_malloc(size_t n); void *__real_calloc(size_t n,size_t s); void *__real_realloc(void *p,size_t n);
ssize_t __real_write(int fd,const void *p,size_t n); int __real_fsync(int fd);
static size_t allocation,failAllocation,ioCount,failIo;
static int ioMode;
static int Fail(void) { return failAllocation!=0U && ++allocation==failAllocation; }
void *__wrap_malloc(size_t n) { return Fail()?NULL:__real_malloc(n); }
void *__wrap_calloc(size_t n,size_t s) { return Fail()?NULL:__real_calloc(n,s); }
void *__wrap_realloc(void *p,size_t n) { return Fail()?NULL:__real_realloc(p,n); }
ssize_t __wrap_write(int fd,const void *p,size_t n)
{
    if(ioMode==1 && ++ioCount==failIo) { errno=ENOSPC; return -1; }
    if(ioMode==3) {
        ++ioCount;
        if(ioCount==1U) { errno=EINTR; return -1; }
        if(n>7U) n=7U;
    }
    return __real_write(fd,p,n);
}
int __wrap_fsync(int fd)
{
    if(ioMode==2 && ++ioCount==failIo) { errno=EIO; return -1; }
    return __real_fsync(fd);
}
static int DescriptorCount(void)
{
    DIR *dir=opendir("/proc/self/fd");
    if(dir==NULL) return -1;
    int count=0; while(readdir(dir)!=NULL) ++count;
    closedir(dir); return count;
}
int main(int argc,char **argv)
{
    if(argc<2) return 2;
    UmiDeclDocument *doc=NULL;
    CHECK(UmiDesignerNativeNotesDocument(&doc)==UMI_STATUS_OK);
    if(strncmp(argv[1],"allocation",10U)==0) {
        size_t failures=0U;
        for(size_t ordinal=1U;ordinal<256U;++ordinal) {
            UmiDesignerNativeProject *plan=NULL;
            UmiDeclDocument *clone=NULL;
            allocation=0U; failAllocation=ordinal;
            UmiStatus status;
            if(strcmp(argv[1],"allocation_clone")==0) status=umi_decl_document_clone(doc,&clone);
            else if(strcmp(argv[1],"allocation_notes")==0) status=UmiDesignerNativeNotesDocument(&clone);
            else status=UmiDesignerNativeProjectCreate(doc,"umicom_notes",&plan,NULL,0U);
            failAllocation=0U;
            if(status==UMI_STATUS_OK) {
                UmiDesignerNativeProjectDestroy(plan); umi_decl_document_destroy(clone);
                CHECK(failures>0U); umi_decl_document_destroy(doc);
                printf("%zu allocation failure points rejected without a published result.\n",failures);
                return 0;
            }
            CHECK(status==UMI_STATUS_OUT_OF_MEMORY && plan==NULL && clone==NULL);
            CHECK(umi_decl_document_node_count(doc)==7U); ++failures;
        }
        return 1;
    }
    if(argc!=3) return 2;
    UmiDesignerNativeProject *plan=NULL;
    CHECK(UmiDesignerNativeProjectCreate(doc,"umicom_notes",&plan,NULL,0U)==UMI_STATUS_OK);
    if(strcmp(argv[1],"short_write")==0) ioMode=3;
    else if(strcmp(argv[1],"write_failure")==0) { ioMode=1; failIo=1U; }
    else if(strcmp(argv[1],"flush_failure")==0) { ioMode=2; failIo=3U; }
    else if(strcmp(argv[1],"directory_flush_failure")==0) { ioMode=2; failIo=7U; }
    else return 2;
    UmiDesignerNativePublishResult result;
    int before=DescriptorCount();
    UmiStatus status=UmiDesignerNativeProjectPublish(plan,argv[2],&result);
    ioMode=0;
    CHECK(before<0 || DescriptorCount()==before);
    if(strcmp(argv[1],"short_write")==0) CHECK(status==UMI_STATUS_OK && result.complete && result.filesWritten==6U);
    else {
        CHECK(status==UMI_STATUS_IO_ERROR && result.directoryCreated && !result.complete);
        CHECK(result.filesWritten==(strcmp(argv[1],"write_failure")==0?0U:strcmp(argv[1],"flush_failure")==0?2U:6U));
        CHECK(UmiDesignerNativeProjectPublish(plan,argv[2],&result)==UMI_STATUS_ALREADY_EXISTS);
    }
    UmiDesignerNativeProjectDestroy(plan); umi_decl_document_destroy(doc); return 0;
}
