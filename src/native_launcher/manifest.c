/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/native_launcher/manifest.c
 * Purpose: Closed-schema desktop-stage manifest encoding and decoding.
 * This bootstrap reader accepts only the established staging schema, not
 * general JSON-RPC, configuration or language-service documents. Fixed-depth
 * field readers avoid pulling a developer runtime into the delivery tool.
 * Unknown/duplicate keys, invalid Unicode, NULs and numeric overflow fail.
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>
static void WriteBinary(UmiNativeText *out, const UmiNativeStageBinary *binary)
{
    UmiNativeTextString(out,"{\"path\":"); UmiNativeTextJson(out,binary->path);
    UmiNativeTextString(out,",\"sha256\":"); UmiNativeTextJson(out,binary->sha256);
    UmiNativeTextString(out,",\"bytes\":"); UmiNativeTextDecimal(out,binary->bytes);
    UmiNativeTextString(out,",\"machine\":"); UmiNativeTextDecimal(out,binary->machine);
    UmiNativeTextString(out,"}");
}
UmiStatus UmiNativeStageManifestWrite(const UmiNativeStageManifest *m,
    char *outText, size_t capacity, size_t *outLength)
{
    UmiNativeText out; UmiNativeTextInit(&out,outText,capacity);
    if (outLength!=NULL) *outLength=0U;
    UmiStatus status=UmiNativeManifestValidate(m);
    if (status!=UMI_STATUS_OK) return status;
    UmiNativeTextString(&out,"{\n  \"schema\":\"" UMI_NATIVE_STAGE_SCHEMA "\",\n  \"root\":");
    UmiNativeTextJson(&out,m->root);
    UmiNativeTextString(&out,",\n  \"inputs\":{\"desk\":"); WriteBinary(&out,&m->desk);
    UmiNativeTextString(&out,",\"launcher\":"); WriteBinary(&out,&m->launcher);
    UmiNativeTextString(&out,"},\n  \"files\":{\"" UMI_NATIVE_STAGE_ICON_NAME "\":");
    UmiNativeTextJson(&out,m->iconHash);
    UmiNativeTextString(&out,",\"" UMI_NATIVE_STAGE_ENTRY_NAME "\":"); UmiNativeTextJson(&out,m->entryHash);
    UmiNativeTextString(&out,"},\n  \"scope\":\""); UmiNativeTextString(&out,UMI_NATIVE_STAGE_SCOPE);
    UmiNativeTextString(&out,"\"\n}\n");
    return UmiNativeTextFinish(&out,outLength);
}
typedef struct Reader { const unsigned char *data; size_t length; size_t offset; } Reader;
static void Space(Reader *r)
{
    while (r->offset<r->length && (r->data[r->offset]==' '||r->data[r->offset]=='\t'||r->data[r->offset]=='\r'||r->data[r->offset]=='\n')) ++r->offset;
}
static int Take(Reader *r, unsigned char c)
{
    Space(r); if (r->offset==r->length || r->data[r->offset]!=c) return 0; ++r->offset; return 1;
}
static int Hex4(Reader *r, uint32_t *out)
{
    if (r->length-r->offset<4U) return 0;
    uint32_t n=0U;
    for (unsigned i=0U;i<4U;++i) {
        unsigned char c=r->data[r->offset++]; uint32_t digit;
        if(c>='0'&&c<='9') digit=(uint32_t)(c-'0');
        else if(c>='a'&&c<='f') digit=(uint32_t)(c-'a')+10U;
        else if(c>='A'&&c<='F') digit=(uint32_t)(c-'A')+10U;
        else return 0;
        n=n*16U+digit;
    }
    *out=n; return 1;
}
static void Codepoint(UmiNativeText *out,uint32_t c)
{
    unsigned char bytes[4]; size_t n;
    if(c<0x80U) {bytes[0]=(unsigned char)c;n=1U;}
    else if(c<0x800U) {bytes[0]=(unsigned char)(0xc0U|(c>>6U));bytes[1]=(unsigned char)(0x80U|(c&63U));n=2U;}
    else if(c<0x10000U) {bytes[0]=(unsigned char)(0xe0U|(c>>12U));bytes[1]=(unsigned char)(0x80U|((c>>6U)&63U));bytes[2]=(unsigned char)(0x80U|(c&63U));n=3U;}
    else {bytes[0]=(unsigned char)(0xf0U|(c>>18U));bytes[1]=(unsigned char)(0x80U|((c>>12U)&63U));bytes[2]=(unsigned char)(0x80U|((c>>6U)&63U));bytes[3]=(unsigned char)(0x80U|(c&63U));n=4U;}
    UmiNativeTextBytes(out,bytes,n);
}
static int String(Reader *r, char *buffer,size_t capacity)
{
    UmiNativeText out; UmiNativeTextInit(&out,buffer,capacity);
    if(!Take(r,'"')) return 0;
    while(r->offset<r->length) {
        unsigned char c=r->data[r->offset++];
        if(c=='"') return out.status==UMI_STATUS_OK && UmiNativeUtf8((const unsigned char *)buffer,out.length);
        if(c<32U) return 0;
        if(c!='\\') {UmiNativeTextBytes(&out,&c,1U);continue;}
        if(r->offset==r->length) return 0;
        c=r->data[r->offset++];
        if(c=='u') {
            uint32_t point,low;
            if(!Hex4(r,&point)||point==0U) return 0;
            if(point>=0xd800U&&point<=0xdbffU) {
                if(r->length-r->offset<6U||r->data[r->offset++]!='\\'||r->data[r->offset++]!='u'
                    ||!Hex4(r,&low)||low<0xdc00U||low>0xdfffU) return 0;
                point=0x10000U+((point-0xd800U)<<10U)+(low-0xdc00U);
            } else if(point>=0xdc00U&&point<=0xdfffU) return 0;
            Codepoint(&out,point);
        } else {
            switch(c) {
                case '"': case '\\': case '/': break;
                case 'b': c='\b';break; case 'f': c='\f';break;
                case 'n': c='\n';break; case 'r': c='\r';break; case 't': c='\t';break;
                default:return 0;
            }
            UmiNativeTextBytes(&out,&c,1U);
        }
        if(out.status!=UMI_STATUS_OK) return 0;
    }
    return 0;
}
static int Number(Reader *r,uint64_t *value)
{
    Space(r); if(r->offset==r->length||r->data[r->offset]<'0'||r->data[r->offset]>'9') return 0;
    uint64_t number=0U; size_t first=r->offset;
    while(r->offset<r->length&&r->data[r->offset]>='0'&&r->data[r->offset]<='9') {
        uint64_t digit=(uint64_t)(r->data[r->offset++]-'0');
        if(number>(UINT64_MAX-digit)/10U) return 0;
        number=number*10U+digit;
    }
    if(r->offset-first>1U&&r->data[first]=='0') return 0;
    *value=number; return 1;
}
/* Each object has a fixed set of keys. End also rejects a trailing comma. */
static int Next(Reader *r,int *done)
{
    Space(r); if(r->offset==r->length) return 0;
    if(r->data[r->offset]=='}') {++r->offset;*done=1;return 1;}
    return Take(r,',');
}
static int Binary(Reader *r,UmiNativeStageBinary *binary)
{
    unsigned seen=0U; int done=0;
    if(!Take(r,'{')) return 0;
    while(!done) {
        char key[32]; unsigned bit=0U;
        if(!String(r,key,sizeof key)||!Take(r,':')) return 0;
        if(strcmp(key,"path")==0) {bit=1U;if(!String(r,binary->path,sizeof binary->path))return 0;}
        else if(strcmp(key,"sha256")==0) {bit=2U;if(!String(r,binary->sha256,sizeof binary->sha256))return 0;}
        else if(strcmp(key,"bytes")==0) {bit=4U;if(!Number(r,&binary->bytes))return 0;}
        else if(strcmp(key,"machine")==0) {uint64_t value;bit=8U;if(!Number(r,&value)||value>UINT16_MAX)return 0;binary->machine=(uint16_t)value;}
        else return 0;
        if((seen&bit)!=0U||!Next(r,&done)) return 0;
        seen|=bit;
    }
    return seen==15U;
}
static int Inputs(Reader *r,UmiNativeStageManifest *m)
{
    unsigned seen=0U; int done=0;
    if(!Take(r,'{')) return 0;
    while(!done) {
        char key[16]; unsigned bit;
        if(!String(r,key,sizeof key)||!Take(r,':')) return 0;
        if(strcmp(key,"desk")==0) {bit=1U;if(!Binary(r,&m->desk))return 0;}
        else if(strcmp(key,"launcher")==0) {bit=2U;if(!Binary(r,&m->launcher))return 0;}
        else return 0;
        if((seen&bit)!=0U||!Next(r,&done)) return 0;
        seen|=bit;
    }
    return seen==3U;
}
static int Files(Reader *r,UmiNativeStageManifest *m)
{
    unsigned seen=0U; int done=0;
    if(!Take(r,'{')) return 0;
    while(!done) {
        char key[64]; unsigned bit;
        if(!String(r,key,sizeof key)||!Take(r,':')) return 0;
        if(strcmp(key,UMI_NATIVE_STAGE_ICON_NAME)==0) {bit=1U;if(!String(r,m->iconHash,sizeof m->iconHash))return 0;}
        else if(strcmp(key,UMI_NATIVE_STAGE_ENTRY_NAME)==0) {bit=2U;if(!String(r,m->entryHash,sizeof m->entryHash))return 0;}
        else return 0;
        if((seen&bit)!=0U||!Next(r,&done)) return 0;
        seen|=bit;
    }
    return seen==3U;
}
UmiStatus UmiNativeStageManifestRead(const char *text,size_t length,UmiNativeStageManifest *outManifest)
{
    if(outManifest==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outManifest,0,sizeof *outManifest);
    if(text==NULL||length==0U) return UMI_STATUS_INVALID_ARGUMENT;
    if(length>=UMI_NATIVE_STAGE_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(memchr(text,0,length)!=NULL||!UmiNativeUtf8((const unsigned char *)text,length)) return UMI_STATUS_PARSE_ERROR;
    UmiNativeStageManifest value={0}; Reader r={(const unsigned char *)text,length,0U};
    unsigned seen=0U; int done=0;
    if(!Take(&r,'{')) return UMI_STATUS_PARSE_ERROR;
    while(!done) {
        char key[24],constant[256]; unsigned bit;
        if(!String(&r,key,sizeof key)||!Take(&r,':')) return UMI_STATUS_PARSE_ERROR;
        if(strcmp(key,"schema")==0) {bit=1U;if(!String(&r,constant,sizeof constant)||strcmp(constant,UMI_NATIVE_STAGE_SCHEMA)!=0)return UMI_STATUS_PARSE_ERROR;}
        else if(strcmp(key,"root")==0) {bit=2U;if(!String(&r,value.root,sizeof value.root))return UMI_STATUS_PARSE_ERROR;}
        else if(strcmp(key,"inputs")==0) {bit=4U;if(!Inputs(&r,&value))return UMI_STATUS_PARSE_ERROR;}
        else if(strcmp(key,"files")==0) {bit=8U;if(!Files(&r,&value))return UMI_STATUS_PARSE_ERROR;}
        else if(strcmp(key,"scope")==0) {bit=16U;if(!String(&r,constant,sizeof constant)||strcmp(constant,UMI_NATIVE_STAGE_SCOPE)!=0)return UMI_STATUS_PARSE_ERROR;}
        else return UMI_STATUS_PARSE_ERROR;
        if((seen&bit)!=0U||!Next(&r,&done)) return UMI_STATUS_PARSE_ERROR;
        seen|=bit;
    }
    Space(&r);
    if(seen!=31U||r.offset!=r.length||UmiNativeManifestValidate(&value)!=UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    *outManifest=value; return UMI_STATUS_OK;
}
