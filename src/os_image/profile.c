/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Strict native source manifests. Build metadata is data, not executable CMake
 * or shell text. Every published snapshot is hashed before and after a build. */
#include "internal.h"
static char *Line(char **cursor) {
    if(!**cursor)return NULL;
    char *line=*cursor,*end=strchr(line,'\n');
    if(!end)return NULL;
    *end=0;
    *cursor=end+1;
    return line;
}
static char *Value(char **cursor,const char *key) {
    char *line=Line(cursor);
    size_t n=strlen(key);
    if(!line||strncmp(line,key,n)||line[n]!='\t'||!line[n+1U]||strchr(line+n+1U,'\t'))return NULL;
    return line+n+1U;
}
static char *TextCopy(const void *data,size_t size) {
    if(!data||!size||size>OI_META_LIMIT||((const unsigned char*)data)[size-1U]!='\n'||memchr(data,0,size))return NULL;
    const unsigned char *p=data;
    for(size_t i=0;i<size;++i)if((p[i]<32U&&p[i]!='\n'&&p[i]!='\t')||p[i]>126U)return NULL;
    char *text=malloc(size+1U);
    if(text) {
        memcpy(text,data,size);
        text[size]=0;
    }
    return text;
}
static int Version(const char *s) {
    if(!s||!*s||strlen(s)>=32U)return 0;
    unsigned parts=0;
    const char *at=s;
    for(;;) {
        const char *end=strchr(at,'.');
        size_t n=end?(size_t)(end-at):strlen(at);
        if(!n||n>9U||(n>1U&&at[0]=='0'))return 0;
        for(size_t i=0;i<n;++i)if(at[i]<'0'||at[i]>'9')return 0;
        ++parts;
        if(!end)break;
        at=end+1;
    }
    return parts==3U;
}
static int NativeName(const char *s) {
    return UmiOsImageValidateName(s)==UMI_STATUS_OK&& ((!strncmp(s,"framework/",10)&&s[10])||(!strncmp(s,"umicomOS/",9)&&s[9]));
}
UmiStatus OiPlanDecode(const void *data,size_t size,OiPlan *p) {
    if(!p)return UMI_STATUS_INVALID_ARGUMENT;
    char *text=TextCopy(data,size);
    if(!text)return UMI_STATUS_PARSE_ERROR;
    OiPlan *temp=calloc(1,sizeof *temp);
    if(!temp) {
        free(text);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    char *at=text,*v;
    UmiStatus s=UMI_STATUS_PARSE_ERROR;
    if(strcmp(Line(&at),"UMICOM_OS_INPUTS\t1"))goto end;
    v=Value(&at,"arch");
    if(!v)goto end;
    if(!strcmp(v,"riscv64"))temp->arch=UMI_OS_IMAGE_RISCV64;
    else if(!strcmp(v,"x86_64"))temp->arch=UMI_OS_IMAGE_X86_64;
    else goto end;
    v=Value(&at,"buildroot");
    if(!OiHash(v,40))goto end;
    strcpy(temp->commit,v);
    v=Value(&at,"linux");
    if(!Version(v))goto end;
    strcpy(temp->linuxVersion,v);
    v=Value(&at,"linux-sha256");
    if(!OiHash(v,64))goto end;
    strcpy(temp->linuxHash,v);
    uint64_t total=0;
    char *line;
    while((line=Line(&at))!=NULL) {
        if(strncmp(line,"file\t",5)||temp->count==UMI_OS_IMAGE_MAX_INPUTS)goto end;
        char *hash=line+5,*len=strchr(hash,'\t');
        if(!len)goto end;
        *len++=0;
        char *name=strchr(len,'\t');
        if(!name)goto end;
        *name++=0;
        uint64_t bytes;
        if(!OiHash(hash,64)||!OiNumber(len,&bytes)||bytes>UMI_OS_IMAGE_MAX_FILE||!NativeName(name)||strchr(name,'\t'))goto end;
        if(temp->count&&strcmp(temp->inputs[temp->count-1U].name,name)>=0)goto end;
        if(bytes>OI_INPUT_LIMIT-total)goto end;
        total+=bytes;
        OiInput *f=&temp->inputs[temp->count++];
        strcpy(f->name,name);
        strcpy(f->hash,hash);
        f->size=bytes;
    }
    if(*at||!temp->count)goto end;
    s=UmiNativeSha256Buffer(data,size,temp->sourceId);
    if(s==UMI_STATUS_OK)*p=*temp;
    end: free(temp);
    free(text);
    return s;
}
static UmiStatus Location(const char *root,OiPlan *p) {
    unsigned char *bytes=NULL;
    size_t n=0;
    UmiStatus s=OiReadJoined(root,"location.umi",OI_META_LIMIT,&bytes,&n);
    if(s!=UMI_STATUS_OK)return s;
    char *text=TextCopy(bytes,n);
    free(bytes);
    if(!text)return UMI_STATUS_PARSE_ERROR;
    char *at=text,*v;
    s=UMI_STATUS_PARSE_ERROR;
    if(strcmp(Line(&at),"UMICOM_OS_LOCATION\t1"))goto end;
    v=Value(&at,"buildroot");
    if(!v||OiAbsolute(v,1)!=UMI_STATUS_OK)goto end;
    strcpy(p->buildroot,v);
    v=Value(&at,"git");
    if(!v||OiAbsolute(v,1)!=UMI_STATUS_OK)goto end;
    strcpy(p->git,v);
    v=Value(&at,"source");
    if(!v||strcmp(v,p->sourceId)||*at)goto end;
    s=UMI_STATUS_OK;
    end:free(text);
    return s;
}
UmiStatus OiPlanLoad(const char *root,OiPlan *p) {
    unsigned char *data=NULL;
    size_t size=0;
    UmiStatus s=OiReadJoined(root,"input-manifest.umi",OI_META_LIMIT,&data,&size);
    if(s==UMI_STATUS_OK)s=OiPlanDecode(data,size,p);
    free(data);
    if(s!=UMI_STATUS_OK)return s;
    s=Location(root,p);
    if(s!=UMI_STATUS_OK)return s;
    char inputs[UMI_OS_IMAGE_PATH];
    s=OiJoin(root,"inputs",inputs);
    for(size_t i=0;i<p->count&&s==UMI_STATUS_OK;++i) {
        char path[UMI_OS_IMAGE_PATH],hash[65];
        uint64_t bytes=0;
        s=OiJoin(inputs,p->inputs[i].name,path);
        if(s==UMI_STATUS_OK)s=OiDigest(path,hash,&bytes);
        if(s==UMI_STATUS_OK&&(strcmp(hash,p->inputs[i].hash)||bytes!=p->inputs[i].size))s=UMI_STATUS_INVALID_STATE;
    }
    return s;
}
UmiStatus OiProfileLoad(const char *os,OiPlan *p) {
    unsigned char *data=NULL;
    size_t n=0;
    UmiStatus s=OiReadJoined(os,"image/native/profile.umi",OI_META_LIMIT,&data,&n);
    if(s!=UMI_STATUS_OK)return s;
    char *text=TextCopy(data,n);
    free(data);
    if(!text)return UMI_STATUS_PARSE_ERROR;
    char *at=text,*v;
    s=UMI_STATUS_PARSE_ERROR;
    if(strcmp(Line(&at),"UMICOM_OS_PROFILE\t1"))goto end;
    v=Value(&at,"buildroot");
    if(!OiHash(v,40))goto end;
    strcpy(p->commit,v);
    v=Value(&at,"linux");
    if(!Version(v))goto end;
    strcpy(p->linuxVersion,v);
    v=Value(&at,"linux-sha256");
    if(!OiHash(v,64))goto end;
    strcpy(p->linuxHash,v);
    while(*at) {
        v=Value(&at,"file");
        if(!v)goto end;
        if(!NativeName(v)||p->count==UMI_OS_IMAGE_MAX_INPUTS)goto end;
        if(p->count&&strcmp(p->inputs[p->count-1U].name,v)>=0)goto end;
        strcpy(p->inputs[p->count++].name,v);
    }
    if(*at||!p->count)goto end;
    /* The profile itself must be frozen, not left as an ambient configuration. */
    int found=0;
    for(size_t i=0;i<p->count;++i)if(!strcmp(p->inputs[i].name,"umicomOS/image/native/profile.umi"))found=1;
    if(!found)goto end;
    s=UMI_STATUS_OK;
    end:free(text);
    return s;
}
