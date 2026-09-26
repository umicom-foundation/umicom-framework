/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Closed-schema validation and an independently written Exec decoder. */
#include "test_support.h"
int TestManifestRoundtrip(void)
{
    UmiNativeStageManifest m=TestManifest(),read;char text[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t n;
    CHECK(UmiNativeStageManifestWrite(&m,text,sizeof text,&n)==UMI_STATUS_OK);
    CHECK(UmiNativeStageManifestRead(text,n,&read)==UMI_STATUS_OK);CHECK(memcmp(&m,&read,sizeof m)==0);return 0;
}
static int Mutated(const char *from,const char *to)
{
    UmiNativeStageManifest m=TestManifest(),read;char text[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t n;
    CHECK(UmiNativeStageManifestWrite(&m,text,sizeof text,&n)==UMI_STATUS_OK);CHECK(TestReplace(text,sizeof text,from,to));
    CHECK(UmiNativeStageManifestRead(text,strlen(text),&read)!=UMI_STATUS_OK);
    const UmiNativeStageManifest zero={0};CHECK(memcmp(&read,&zero,sizeof read)==0);return 0;
}
int TestManifestDuplicateRoot(void){return Mutated("\"root\":","\"root\":\"/home/user/review\",\"root\":");}
int TestManifestEscapedDuplicate(void){return Mutated("\"root\":","\"ro\\u006ft\":\"/home/user/review\",\"root\":");}
int TestManifestDuplicateBinary(void){return Mutated("\"bytes\":4096","\"bytes\":4096,\"bytes\":4096");}
int TestManifestUnknown(void){return Mutated("\"root\":","\"unknown\":0,\"root\":");}
int TestManifestMissing(void){return Mutated("\"root\":\"/home/user/review\",","");}
int TestManifestNegative(void){return Mutated("\"bytes\":4096","\"bytes\":-1");}
int TestManifestLeadingZero(void){return Mutated("\"bytes\":4096","\"bytes\":04096");}
int TestManifestOverflow(void){return Mutated("\"bytes\":4096","\"bytes\":18446744073709551616");}
int TestManifestFloat(void){return Mutated("\"bytes\":4096","\"bytes\":4096.0");}
int TestManifestBoolean(void){return Mutated("\"bytes\":4096","\"bytes\":true");}
int TestManifestTrailer(void){return Mutated("\"schema\":","\"schema\":{},\"schema\":");}
int TestManifestSurrogate(void){return Mutated("/home/user/review","/home/user/\\ud800");}
int TestManifestNul(void){return Mutated("/home/user/review","/home/user/\\u0000review");}
int TestManifestArchitecture(void){return Mutated("\"machine\":62","\"machine\":65536");}
int TestManifestScope(void){return Mutated(UMI_NATIVE_STAGE_SCOPE,"A different executable action");}
int TestManifestBadHash(void){return Mutated("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz");}
int TestManifestTrailingComma(void){return Mutated("\"machine\":62}","\"machine\":62,}");}
int TestManifestUtf8(void)
{
    UmiNativeStageManifest m=TestManifest(),read;char text[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t n;
    strcpy(m.root,"/home/user/caf\xc3\xa9-\xf0\x9f\x93\x9d");
    CHECK(UmiNativeStageManifestWrite(&m,text,sizeof text,&n)==UMI_STATUS_OK);
    CHECK(TestReplace(text,sizeof text,m.root,"/home/user/caf\\u00e9-\\ud83d\\udcdd"));
    CHECK(UmiNativeStageManifestRead(text,strlen(text),&read)==UMI_STATUS_OK);CHECK(strcmp(m.root,read.root)==0);
    CHECK(TestReplace(text,sizeof text,"/home/user/caf","/home/user/\xc0\x80"));
    CHECK(UmiNativeStageManifestRead(text,strlen(text),&read)==UMI_STATUS_PARSE_ERROR);return 0;
}
int TestManifestTruncation(void)
{
    UmiNativeStageManifest m=TestManifest(),read;char text[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t n;
    CHECK(UmiNativeStageManifestWrite(&m,text,sizeof text,&n)==UMI_STATUS_OK);
    /* The last newline is optional JSON whitespace; every earlier truncation
     * before the final closing brace is incomplete. */
    for(size_t i=0U;i<n-2U;++i)CHECK(UmiNativeStageManifestRead(text,i,&read)!=UMI_STATUS_OK);
    text[n++]='x';CHECK(UmiNativeStageManifestRead(text,n,&read)==UMI_STATUS_PARSE_ERROR);return 0;
}
int TestManifestCapacity(void)
{
    UmiNativeStageManifest m=TestManifest();char text[16]="not-empty";size_t n=77U;
    CHECK(UmiNativeStageManifestWrite(&m,text,sizeof text,&n)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(text[0]==0&&n==0U);
    CHECK(UmiNativeStageRenderEntry(&m,text,sizeof text,&n)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(text[0]==0&&n==0U);
    CHECK(UmiNativeStageManifestWrite(NULL,text,sizeof text,&n)==UMI_STATUS_INVALID_ARGUMENT);return 0;
}
int TestManifestAdversarial(void)
{
    UmiNativeStageManifest m=TestManifest(),read;char valid[UMI_NATIVE_STAGE_TEXT_CAPACITY],text[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t n;
    CHECK(UmiNativeStageManifestWrite(&m,valid,sizeof valid,&n)==UMI_STATUS_OK);uint32_t random=0x654321U;
    for(unsigned i=0U;i<10000U;++i){memcpy(text,valid,n);random=random*1664525U+1013904223U;size_t at=(size_t)random%n;random=random*1664525U+1013904223U;text[at]=(char)(random>>24U);
        UmiStatus status=UmiNativeStageManifestRead(text,n,&read);
        if(status==UMI_STATUS_OK){char again[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t count;UmiNativeStageManifest final;
            CHECK(UmiNativeStageManifestWrite(&read,again,sizeof again,&count)==UMI_STATUS_OK);CHECK(UmiNativeStageManifestRead(again,count,&final)==UMI_STATUS_OK);CHECK(memcmp(&read,&final,sizeof read)==0);}}
    return 0;
}
int TestPaths(void)
{
    const char *bad[]={"relative","/","/a/../b","/a/./b","//tmp/a","/a//b","/a/","/a/%f","/a/\n","/a/\x7f","/a/\xed\xa0\x80"};
    for(size_t i=0U;i<sizeof bad/sizeof bad[0];++i)CHECK(UmiNativeStageValidatePath(bad[i],0)!=UMI_STATUS_OK);
    CHECK(UmiNativeStageValidatePath("/a/valid space;\"$`\\-\xc3\xa9",0)==UMI_STATUS_OK);
    CHECK(UmiNativeStageValidatePath("/a/program ",1)!=UMI_STATUS_OK);
    CHECK(UmiNativeStageValidatePath("/a/x=y",1)!=UMI_STATUS_OK);CHECK(UmiNativeStageValidatePath("/a/x=y",0)==UMI_STATUS_OK);return 0;
}
/* Decode key-value backslash pairs, then quoted Exec tokens. Deliberately
 * separate from the production encoders; no shell or external runner is used. */
static int DecodeExec(const char *line,char tokens[4][UMI_NATIVE_STAGE_PATH_CAPACITY])
{
    char raw[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t n=0U;
    while(*line && *line!='\n'){char c=*line++;if(c=='\\'){if(*line!='\\')return 0;c=*line++;}raw[n++]=c;}raw[n]=0;
    const char *p=raw;unsigned index=0U;
    while(*p){while(*p==' ')++p;if(!*p)break;if(index==4U)return 0;size_t at=0U;int quoted=*p=='"';if(quoted)++p;
        while(*p && (quoted?*p!='"':*p!=' ')){char c=*p++;if(quoted&&c=='\\'){if(!*p)return 0;c=*p++;}if(at+1U>=UMI_NATIVE_STAGE_PATH_CAPACITY)return 0;tokens[index][at++]=c;}
        if(quoted){if(*p!='"')return 0;++p;}tokens[index++][at]=0;if(*p&&*p!=' ')return 0;}
    return index==4U;
}
int TestEntryQuoting(void)
{
    UmiNativeStageManifest m=TestManifest();char text[UMI_NATIVE_STAGE_TEXT_CAPACITY],tokens[4][UMI_NATIVE_STAGE_PATH_CAPACITY];size_t n;
    strcpy(m.desk.path,"/home/user/Desk \"quoted\" $value `tick` \\literal;name");
    strcpy(m.launcher.path,"/home/user/Session \"quoted\" $value `tick` \\literal;name");
    CHECK(UmiNativeStageRenderEntry(&m,text,sizeof text,&n)==UMI_STATUS_OK);char *exec=strstr(text,"\nExec=");CHECK(exec!=NULL);
    CHECK(DecodeExec(exec+6,tokens));CHECK(strcmp(tokens[0],m.launcher.path)==0);CHECK(strcmp(tokens[1],"--run")==0);
    CHECK(strcmp(tokens[2],"--desk")==0);CHECK(strcmp(tokens[3],m.desk.path)==0);return 0;
}
int TestEntryGolden(void)
{
    UmiNativeStageManifest m=TestManifest();char text[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t n;
    CHECK(UmiNativeStageRenderEntry(&m,text,sizeof text,&n)==UMI_STATUS_OK);
    const char *expected="[Desktop Entry]\nType=Application\nVersion=1.0\nName=Umicom Desk session\nComment=Open Umicom Desk in the current graphical session\nExec=\"/home/user/bin/session\" --run --desk \"/home/user/bin/desk\"\nTryExec=/home/user/bin/session\nIcon=/home/user/review/umicom-icon.png\nTerminal=false\nDBusActivatable=false\nCategories=System;\nStartupNotify=false\n";
    CHECK(strcmp(text,expected)==0);CHECK(n==strlen(expected));return 0;
}
