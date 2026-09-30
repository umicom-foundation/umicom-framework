/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/os_image/test_buffers.c
 * PURPOSE:
 *   Public buffer contracts and negative cases. No subprocess or filesystem I/O.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Public buffer contracts and negative cases. No subprocess or filesystem I/O. */
#include "fixtures.h"
static int Replace(OiText *text,const char *from,const char *to) {
    char *found=strstr((char*)text->data,from);
    if(!found)return 0;
    size_t before=(size_t)(found-(char*)text->data);
    OiText t;
    OiTextInit(&t);
    OiAppend(&t,text->data,before);
    OiAppend(&t,to,strlen(to));
    OiAppend(&t,found+strlen(from),text->size-before-strlen(from));
    OiTextClear(text);
    *text=t;
    return t.status==UMI_STATUS_OK;
}
static int Archive(const char *name) {
    UmiOsImageEntry e[]= {
        {
            "etc",OI_DIR|0755U,0,0,NULL,0
        }
        , {
            "etc/message",OI_REG|0444U,0,0,(const unsigned char*)"Umicom Notes\n",13
        }
    }
    ;
    unsigned char *gzip=NULL;
    size_t size=0;
    UmiOsImageArchive *a=NULL;
    if(!strcmp(name,"archive_empty")) {
        CHECK(UmiOsImageArchiveBuild(e,0,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_duplicate")) {
        e[1].name="etc";
        CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_missing_parent")) {
        e[1].name="other/message";
        CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_file_parent")) {
        e[0].mode=OI_REG|0444U;
        CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_setid")) {
        e[1].mode|=04000U;
        CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_block_device")) {
        e[1].mode=0060600;
        e[1].size=0;
        CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_unknown_character_device")) {
        e[1].name="etc/device";
        e[1].mode=OI_CHR|0600U;
        e[1].size=0;
        e[1].major=5;
        e[1].minor=1;
        CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_bad_device_fields")) {
        e[1].major=1;
        CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_entry_limit")) {
        CHECK(UmiOsImageArchiveBuild(e,UMI_OS_IMAGE_MAX_ENTRIES+1U,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"archive_file_limit")) {
        e[1].size=UMI_OS_IMAGE_MAX_FILE+1U;
        CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)!=UMI_STATUS_OK);
        return 0;
    }
    unsigned char *large=NULL;
    if(!strcmp(name,"archive_multiblock")) {
        large=malloc(131073);
        CHECK(large);
        for(size_t i=0;i<131073U;++i)large[i]=(unsigned char)(i*31U);
        e[1].data=large;
        e[1].size=131073;
    }
    CHECK(UmiOsImageArchiveBuild(e,2,&gzip,&size)==UMI_STATUS_OK);
    if(!strcmp(name,"archive_crc"))gzip[size-8U]^=1;
    if(!strcmp(name,"archive_size_trailer"))gzip[size-4U]^=1;
    if(!strcmp(name,"archive_deflate_type"))gzip[10]=3;
    if(!strcmp(name,"archive_complement"))gzip[13]^=1;
    if(!strcmp(name,"archive_options"))gzip[3]=4;
    if(!strcmp(name,"archive_extra_member")) {
        unsigned char *p=realloc(gzip,size+1U);
        CHECK(p);
        gzip=p;
        gzip[size++]=0;
    }
    if(!strcmp(name,"archive_truncated")) {
        for(size_t i=0;i<size;++i) {
            CHECK(UmiOsImageArchiveOpen(gzip,i,&a)!=UMI_STATUS_OK);
            CHECK(!a);
        }
        free(gzip);
        return 0;
    }
    if(!strcmp(name,"archive_deterministic")) {
        UmiOsImageEntry reversed[]= {
            e[1],e[0]
        }
        ;
        unsigned char *second=NULL;
        size_t n=0;
        CHECK(UmiOsImageArchiveBuild(reversed,2,&second,&n)==UMI_STATUS_OK);
        CHECK(n==size&&!memcmp(second,gzip,size));
        free(second);
    }
    if(!strcmp(name,"archive_mutations")) {
        uint32_t seed=42;
        for(unsigned i=0;i<6000U;++i) {
            seed=seed*1664525U+1013904223U;
            size_t at=seed%size;
            unsigned char old=gzip[at];
            gzip[at]^=(unsigned char)(1U<<(i%8U));
            UmiStatus s=UmiOsImageArchiveOpen(gzip,size,&a);
            if(s==UMI_STATUS_OK)UmiOsImageArchiveDestroy(a);
            a=NULL;
            gzip[at]=old;
        }
        free(gzip);
        return 0;
    }
    int negative=!strcmp(name,"archive_crc")||!strcmp(name,"archive_size_trailer")||!strcmp(name,"archive_deflate_type")||!strcmp(name,"archive_complement")||!strcmp(name,"archive_options")||!strcmp(name,"archive_extra_member");
    UmiStatus s=UmiOsImageArchiveOpen(gzip,size,&a);
    if(negative) {
        CHECK(s!=UMI_STATUS_OK);
        CHECK(!a);
    }
    else {
        CHECK(s==UMI_STATUS_OK);
        CHECK(UmiOsImageArchiveCount(a)==2U);
        const UmiOsImageEntry *v=UmiOsImageArchiveAt(a,1);
        CHECK(v&&v->size==e[1].size&&!memcmp(v->data,e[1].data,v->size));
        CHECK(!UmiOsImageArchiveAt(a,2));
    }
    UmiOsImageArchiveDestroy(a);
    a=NULL;
    if(!strncmp(name,"newc_",5)) {
        size_t n=OiLe16(gzip+11);
        unsigned char *raw=malloc(n);
        CHECK(raw);
        memcpy(raw,gzip+15,n);
        if(!strcmp(name,"newc_uid"))raw[6+2*8+7]='1';
        if(!strcmp(name,"newc_links"))raw[6+4*8+7]='3';
        if(!strcmp(name,"newc_inode"))raw[6+7]='2';
        if(!strcmp(name,"newc_padding"))raw[114]=1;
        if(!strcmp(name,"newc_name_nul"))raw[113]='X';
        if(!strcmp(name,"newc_trailer_missing")) {
            char *tr=(char*)raw;
            for(size_t i=0;i+10U<n;++i)if(!memcmp(raw+i,"TRAILER!!!",10)) {
                tr=(char*)raw+i;
                break;
            }
            tr[0]='X';
        }
        if(!strcmp(name,"newc_duplicate")) {
            for(size_t i=0;i+11U<n;++i)if(!memcmp(raw+i,"etc/message",11)) {
                memcpy(raw+i,"etc\0message",11);
                break;
            }
        }
        s=UmiOsImageArchiveOpen(raw,n,&a);
        CHECK(!strcmp(name,"newc_roundtrip")?s==UMI_STATUS_OK:s!=UMI_STATUS_OK);
        UmiOsImageArchiveDestroy(a);
        free(raw);
    }
    free(large);
    free(gzip);
    return 0;
}
static int Formats(const char *name) {
    unsigned char elf[4096];
    TestElf(elf,UMI_OS_IMAGE_X86_64);
    UmiOsImageArch arch=UMI_OS_IMAGE_X86_64;
    size_t n=sizeof elf;
    if(!strcmp(name,"elf_riscv")) {
        TestElf(elf,UMI_OS_IMAGE_RISCV64);
        arch=UMI_OS_IMAGE_RISCV64;
    }
    if(!strcmp(name,"elf_wrong_arch"))arch=UMI_OS_IMAGE_RISCV64;
    if(!strcmp(name,"elf_truncated"))n=63;
    if(!strcmp(name,"elf_dynamic"))OiPut32(elf+120,2);
    if(!strcmp(name,"elf_interpreter"))OiPut32(elf+120,3);
    if(!strcmp(name,"elf_stack"))OiPut32(elf+124,7);
    if(!strcmp(name,"elf_entry"))TestPut64(elf+24,UINT64_C(0x401000));
    if(!strcmp(name,"elf_ph_overflow"))TestPut64(elf+32,UINT64_MAX);
    if(!strcmp(name,"elf_memory_overflow"))TestPut64(elf+80,UINT64_MAX-32U);
    if(!strcmp(name,"elf_file_overflow"))TestPut64(elf+72,4097);
    if(!strcmp(name,"elf_memory_small"))TestPut64(elf+104,1);
    if(!strcmp(name,"elf_alignment"))TestPut64(elf+112,3);
    if(!strcmp(name,"elf_big_endian"))elf[5]=2;
    if(!strcmp(name,"elf_pie"))TestPut16(elf+16,3);
    if(!strncmp(name,"elf_",4)) {
        UmiStatus s=UmiOsImageValidateElf(elf,n,arch);
        CHECK((!strcmp(name,"elf_x86")||!strcmp(name,"elf_riscv"))?s==UMI_STATUS_OK:s!=UMI_STATUS_OK);
        return 0;
    }
    unsigned char kernel[8192];
    arch=strstr(name,"riscv")?UMI_OS_IMAGE_RISCV64:UMI_OS_IMAGE_X86_64;
    TestKernel(kernel,arch);
    if(!strcmp(name,"kernel_x86_flags"))kernel[0x236]=0;
    if(!strcmp(name,"kernel_x86_protocol"))TestPut16(kernel+0x206,0x20b);
    if(!strcmp(name,"kernel_x86_setup"))kernel[0x1f1]=128;
    if(!strcmp(name,"kernel_riscv_magic"))kernel[56]='X';
    if(!strcmp(name,"kernel_riscv_size"))TestPut64(kernel+16,1);
    if(!strcmp(name,"kernel_riscv_endian"))kernel[24]=1;
    UmiStatus s=UmiOsImageValidateKernel(kernel,sizeof kernel,arch);
    CHECK((!strcmp(name,"kernel_x86")||!strcmp(name,"kernel_riscv"))?s==UMI_STATUS_OK:s!=UMI_STATUS_OK);
    return 0;
}
static int Boot(const char *name) {
    OiText t;
    OiTextInit(&t);
    int recovery=!strcmp(name,"boot_recovery");
    TestTranscript(&t,TestSource(),recovery);
    int code=0;
    if(!strcmp(name,"boot_exit"))code=1;
    if(!strcmp(name,"boot_wrong_source"))CHECK(Replace(&t,"source=0","source=f"));
    if(!strcmp(name,"boot_missing_end"))CHECK(Replace(&t,"UMICOM_SERVICE end=platform-check outcome=none exit=0 signal=0\n",""));
    if(!strcmp(name,"boot_service_failure"))CHECK(Replace(&t,"outcome=none exit=0","outcome=service-exit exit=1"));
    if(!strcmp(name,"boot_missing_shutdown"))CHECK(Replace(&t,"UMICOM_SHUTDOWN\n",""));
    if(!strcmp(name,"boot_double_init"))CHECK(Replace(&t,"UMICOM_INIT pid=1\n","UMICOM_INIT pid=1\nUMICOM_INIT pid=1\n"));
    if(!strcmp(name,"boot_duplicate_field"))CHECK(Replace(&t,"planned=2\n","planned=2\nplanned=2\n"));
    if(!strcmp(name,"boot_unknown_field"))CHECK(Replace(&t,"planned=2\n","planned=2\nunknown=2\n"));
    if(!strcmp(name,"boot_panic"))OiPrint(&t,"Kernel panic - not syncing\n");
    if(!strcmp(name,"boot_partial_line"))--t.size;
    if(!strcmp(name,"boot_service_order"))CHECK(Replace(&t,"UMICOM_SERVICE start=platform-check","UMICOM_SERVICE start=framework-probe"));
    if(!strcmp(name,"boot_service_after_report"))OiPrint(&t,"UMICOM_SERVICE start=platform-check\n");
    if(!strcmp(name,"boot_report_state"))CHECK(Replace(&t,"state=ready","state=starting"));
    if(!strcmp(name,"boot_incomplete_services"))CHECK(Replace(&t,"completed=2","completed=1"));
    if(!strcmp(name,"boot_marker_substring"))CHECK(Replace(&t,"UMICOM_INIT pid=1","noise UMICOM_INIT pid=1"));
    if(!strcmp(name,"boot_recovery_mismatch"))recovery=1;
    if(!strcmp(name,"boot_double_report"))TestTranscript(&t,TestSource(),0);
    if(!strcmp(name,"boot_embedded_nul"))t.data[5]=0;
    if(!strcmp(name,"boot_crlf")) {
        OiText c;
        OiTextInit(&c);
        for(size_t i=0;i<t.size;++i) {
            if(t.data[i]=='\n')OiAppend(&c,"\r",1);
            OiAppend(&c,t.data+i,1);
        }
        OiTextClear(&t);
        t=c;
    }
    UmiOsImageReport r;
    UmiStatus s=UmiOsImageAssessBoot(t.data,t.size,TestSource(),recovery?UMI_OS_IMAGE_RECOVERY:UMI_OS_IMAGE_NORMAL,code,&r);
    int valid=!strcmp(name,"boot_normal")||!strcmp(name,"boot_recovery")||!strcmp(name,"boot_crlf");
    CHECK(valid?s==UMI_STATUS_OK:s!=UMI_STATUS_OK);
    CHECK(!r.processLaunched);
    OiTextClear(&t);
    return 0;
}
static int Manifest(const char *name) {
    OiText text;
    OiTextInit(&text);
    OiPrint(&text,"UMICOM_OS_INPUTS\t1\narch\tx86_64\nbuildroot\t0123456789abcdef0123456789abcdef01234567\nlinux\t6.12.109\nlinux-sha256\t%s\nfile\t%s\t0\tframework/notice\n",TestSource(),TestSource());
    if(!strcmp(name,"manifest_duplicate"))OiPrint(&text,"file\t%s\t0\tframework/notice\n",TestSource());
    if(!strcmp(name,"manifest_escape"))CHECK(Replace(&text,"framework/notice","framework/../notice"));
    if(!strcmp(name,"manifest_unknown_owner"))CHECK(Replace(&text,"framework/notice","arbitrary/notice"));
    if(!strcmp(name,"manifest_leading_zero"))CHECK(Replace(&text,"\t0\tframework","\t00\tframework"));
    if(!strcmp(name,"manifest_version"))CHECK(Replace(&text,"6.12.109","6..109"));
    if(!strcmp(name,"manifest_unknown_field"))OiPrint(&text,"ignored\ttrue\n");
    OiPlan *p=calloc(1,sizeof *p);
    CHECK(p);
    UmiStatus s=OiPlanDecode(text.data,text.size,p);
    CHECK(!strcmp(name,"manifest_roundtrip")?s==UMI_STATUS_OK:s!=UMI_STATUS_OK);
    free(p);
    OiTextClear(&text);
    return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==2);
    const char *name=argv[1];
    if(!strncmp(name,"archive_",8)||!strncmp(name,"newc_",5))return Archive(name);
    if(!strncmp(name,"elf_",4)||!strncmp(name,"kernel_",7))return Formats(name);
    if(!strncmp(name,"boot_",5))return Boot(name);
    if(!strncmp(name,"artifact_",9)) {
        OiText evidence;
        OiTextInit(&evidence);
        char hash[65];
        CHECK(UmiNativeSha256Buffer("original",8,hash)==UMI_STATUS_OK);
        OiPrint(&evidence,"UMICOM_OS_BUILT\t1\nsource\t%s\nfile\t%s\t8\tbuild/target/init\n",TestSource(),hash);
        if(!strcmp(name,"artifact_duplicate"))OiPrint(&evidence,"file\t%s\t8\tbuild/target/init\n",hash);
        const char *data=!strcmp(name,"artifact_changed")?"modified":"original";
        const char *path=!strcmp(name,"artifact_wrong_path")?"build/target/other":"build/target/init";
        UmiStatus status=OiCapturedArtifact(&evidence,path,data,8);
        CHECK(!strcmp(name,"artifact_matching")?status==UMI_STATUS_OK:status!=UMI_STATUS_OK);
        OiTextClear(&evidence);
        return 0;
    }
    if(!strcmp(name,"manifest_adversarial")) {
        OiPlan *plan=calloc(1,sizeof *plan);
        CHECK(plan);
        const char *text="UMICOM_OS_INPUTS\t1\narch\tx86_64\nbuildroot\t0123456789abcdef0123456789abcdef01234567\nlinux\t6.12.109\nlinux-sha256\t0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef\nfile\t0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef\t0\tframework/notice\n";
        size_t n=strlen(text);
        char *copy=malloc(n+1);
        CHECK(copy);
        uint32_t seed=9;
        for(size_t i=0;i<n;++i)CHECK(OiPlanDecode(text,i,plan)!=UMI_STATUS_OK);
        for(unsigned i=0;i<5000;++i) {
            memcpy(copy,text,n+1);
            seed=seed*1664525U+1013904223U;
            copy[seed%n]=(char)(i%128U);
            (void)OiPlanDecode(copy,n,plan);
        }
        free(copy);
        free(plan);
        return 0;
    }
    if(!strncmp(name,"manifest_",9))return Manifest(name);
    if(!strcmp(name,"names")) {
        const char *bad[]= {
            "","/abs",".","..","x/..","x//y","x/","x\\y","x:y","TRAILER!!!","x\ty","x/./y"
        }
        ;
        for(size_t i=0;i<sizeof bad/sizeof bad[0];++i)CHECK(UmiOsImageValidateName(bad[i])!=UMI_STATUS_OK);
        CHECK(UmiOsImageValidateName("etc/umicom/boot.conf")==UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"crc32")) {
        CHECK(OiCrc32("123456789",9)==UINT32_C(0xcbf43926));
        CHECK(OiCrc32(NULL,0)==0);
        return 0;
    }
    if(!strcmp(name,"invalid_arguments")) {
        CHECK(UmiOsImageArchiveOpen(NULL,0,NULL)!=UMI_STATUS_OK);
        CHECK(UmiOsImageArchiveBuild(NULL,0,NULL,NULL)!=UMI_STATUS_OK);
        CHECK(UmiOsImageAssessBoot(NULL,0,NULL,UMI_OS_IMAGE_NORMAL,0,NULL)!=UMI_STATUS_OK);
        return 0;
    }
    return 2;
}
