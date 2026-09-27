/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Protocol-only JSON parser. The input is bounded before indexing; each token
 * records its subtree end, so member traversal never searches neighbouring
 * objects. No substring can masquerade as a top-level QMP acknowledgement. */
#include "json_internal.h"
#include <string.h>
#include <limits.h>
static int Hex(unsigned char c){
    if(c>='0'&&c<='9')return c-'0';
    if(c>='a'&&c<='f')return c-'a'+10;
    if(c>='A'&&c<='F')return c-'A'+10;
    return -1;
}
static int Scalar(const unsigned char *b,size_t n,size_t *position,uint32_t *out){
    size_t p=*position;
    if(p>=n)return 0;
    unsigned char c=b[p++];
    uint32_t cp;
    unsigned extra;
    if(c<0x80){
        cp=c;
        extra=0;
    }
    else if(c>=0xc2&&c<=0xdf){
        cp=c&31U;
        extra=1;
    }
    else if(c>=0xe0&&c<=0xef){
        cp=c&15U;
        extra=2;
    }
    else if(c>=0xf0&&c<=0xf4){
        cp=c&7U;
        extra=3;
    }
    else return 0;
    if(extra>n-p)return 0;
    for(unsigned i=0;i<extra;++i){
        c=b[p++];
        if((c&0xc0U)!=0x80U)return 0;
        cp=(cp<<6)|(c&63U);
    }
    if((extra==1&&cp<0x80)||(extra==2&&cp<0x800)||(extra==3&&cp<0x10000)||cp>0x10ffff||(cp>=0xd800&&cp<=0xdfff))return 0;
    *position=p;
    *out=cp;
    return 1;
}
int VmUtf8(const char *s,size_t maximum){
    if(!s)return 0;
    size_t n=0;
    while(n<maximum&&s[n])++n;
    if(n==maximum)return 0;
    size_t p=0;
    while(p<n){
        uint32_t cp;
        if(!Scalar((const unsigned char*)s,n,&p,&cp)||cp<32||cp==127)return 0;
    }
    return 1;
}
static int Unit(const unsigned char *b,size_t end,size_t *p,uint32_t *out){
    if(end-*p<4U)return 0;
    uint32_t v=0;
    for(unsigned i=0;i<4U;++i){
        int h=Hex(b[(*p)++]);
        if(h<0)return 0;
        v=(v<<4)|(unsigned)h;
    }
    *out=v;
    return 1;
}
static int Character(const unsigned char *b,size_t end,size_t *p,uint32_t *cp){
    if(*p>=end)return 0;
    if(b[*p]!='\\')return Scalar(b,end,p,cp);
    ++*p;
    if(*p>=end)return 0;
    unsigned char c=b[(*p)++];
    switch(c){
        case '"':case '\\':case '/':*cp=c;
        return 1;
        case 'b':*cp=8;
        return 1;
        case 'f':*cp=12;
        return 1;
        case 'n':*cp=10;
        return 1;
        case 'r':*cp=13;
        return 1;
        case 't':*cp=9;
        return 1;
        case 'u':break;
        default:return 0;
    }
    uint32_t a;
    if(!Unit(b,end,p,&a))return 0;
    if(a>=0xd800&&a<=0xdbff){
        if(end-*p<6U||b[*p]!='\\'||b[*p+1U]!='u')return 0;
        *p+=2U;
        uint32_t low;
        if(!Unit(b,end,p,&low)||low<0xdc00||low>0xdfff)return 0;
        a=0x10000U+((a-0xd800U)<<10)+(low-0xdc00U);
    }
    else if(a>=0xdc00&&a<=0xdfff)return 0;
    *cp=a;
    return 1;
}
static void White(VjDocument *d){
    while(d->used<d->length&&strchr(" \r\n\t",d->bytes[d->used]))++d->used;
}
static UmiStatus Value(VjDocument *d,int parent,unsigned depth,int *result);
static int EqualKeys(const VjDocument *d,int a,int b){
    size_t x=d->tokens[a].start,y=d->tokens[b].start;
    while(x<d->tokens[a].end&&y<d->tokens[b].end){
        uint32_t u,v;
        if(!Character(d->bytes,d->tokens[a].end,&x,&u)||!Character(d->bytes,d->tokens[b].end,&y,&v)||u!=v)return 0;
    }
    return x==d->tokens[a].end&&y==d->tokens[b].end;
}
static UmiStatus Value(VjDocument *d,int parent,unsigned depth,int *result){
    White(d);
    if(depth>24U||d->count==512U)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(d->used==d->length)return UMI_STATUS_PARSE_ERROR;
    int index=(int)d->count++;
    VjToken *t=&d->tokens[index];
    t->start=d->used;
    t->parent=parent;
    *result=index;
    unsigned char c=d->bytes[d->used++];
    if(c=='{'||c=='['){
        t->type=c=='{'?VJ_OBJECT:VJ_ARRAY;
        unsigned char close=c=='{'?'}':']';
        White(d);
        if(d->used<d->length&&d->bytes[d->used]==close)++d->used;
        else for(;;){
            int key=-1,value=-1;
            UmiStatus s;
            if(c=='{'){
                if(d->used>=d->length||d->bytes[d->used]!='"')return UMI_STATUS_PARSE_ERROR;
                s=Value(d,index,depth+1U,&key);
                if(s!=UMI_STATUS_OK)return s;
                for(int old=index+1;old<key;){
                    if(EqualKeys(d,old,key))return UMI_STATUS_PARSE_ERROR;
                    int v=d->tokens[old].next;
                    old=d->tokens[v].next;
                }
                White(d);
                if(d->used>=d->length||d->bytes[d->used++]!=':')return UMI_STATUS_PARSE_ERROR;
            }
            s=Value(d,index,depth+1U,&value);
            if(s!=UMI_STATUS_OK)return s;
            White(d);
            if(d->used>=d->length)return UMI_STATUS_PARSE_ERROR;
            c=d->bytes[t->start];
            unsigned char separator=d->bytes[d->used++];
            if(separator==close)break;
            if(separator!=',')return UMI_STATUS_PARSE_ERROR;
            White(d);
        }
        t->end=d->used;
    }
    else if(c=='"'){
        t->type=VJ_STRING;
        t->start=d->used;
        int closed=0;
        while(d->used<d->length){
            if(d->bytes[d->used]=='"'){
                t->end=d->used++;
                closed=1;
                break;
            }
            if(d->bytes[d->used]<32U)return UMI_STATUS_PARSE_ERROR;
            uint32_t cp;
            if(!Character(d->bytes,d->length,&d->used,&cp))return UMI_STATUS_PARSE_ERROR;
        }
        if(!closed)return UMI_STATUS_PARSE_ERROR;
    }
    else{
        --d->used;
        size_t begin=d->used;
        if(c=='t'||c=='f'||c=='n'){
            const char *word=c=='t'?"true":c=='f'?"false":"null";
            size_t n=strlen(word);
            if(n>d->length-d->used||memcmp(d->bytes+d->used,word,n))return UMI_STATUS_PARSE_ERROR;
            d->used+=n;
            t->type=c=='t'?VJ_TRUE:c=='f'?VJ_FALSE:VJ_NULL;
        }
        else{
            t->type=VJ_NUMBER;
            if(d->bytes[d->used]=='-')++d->used;
            if(d->used==d->length)return UMI_STATUS_PARSE_ERROR;
            if(d->bytes[d->used]=='0')++d->used;
            else{
                if(d->bytes[d->used]<'1'||d->bytes[d->used]>'9')return UMI_STATUS_PARSE_ERROR;
                while(d->used<d->length&&d->bytes[d->used]>='0'&&d->bytes[d->used]<='9')++d->used;
            }
            if(d->used<d->length&&d->bytes[d->used]=='.'){
                ++d->used;
                size_t n=d->used;
                while(d->used<d->length&&d->bytes[d->used]>='0'&&d->bytes[d->used]<='9')++d->used;
                if(n==d->used)return UMI_STATUS_PARSE_ERROR;
            }
            if(d->used<d->length&&(d->bytes[d->used]=='e'||d->bytes[d->used]=='E')){
                ++d->used;
                if(d->used<d->length&&(d->bytes[d->used]=='+'||d->bytes[d->used]=='-'))++d->used;
                size_t n=d->used;
                while(d->used<d->length&&d->bytes[d->used]>='0'&&d->bytes[d->used]<='9')++d->used;
                if(n==d->used)return UMI_STATUS_PARSE_ERROR;
            }
        }
        t->start=begin;
        t->end=d->used;
    }
    t->next=(int)d->count;
    return UMI_STATUS_OK;
}
UmiStatus VjParse(const void *p,size_t n,VjDocument *d){
    if(!p||!d||!n||n>UMI_VM_QMP_FRAME||memchr(p,0,n))return UMI_STATUS_PARSE_ERROR;
    memset(d,0,sizeof *d);
    d->bytes=p;
    d->length=n;
    int token;
    UmiStatus s=Value(d,-1,0,&token);
    White(d);
    if(s==UMI_STATUS_OK&&(d->used!=n||d->tokens[0].type!=VJ_OBJECT))s=UMI_STATUS_PARSE_ERROR;
    return s;
}
UmiStatus VjString(const VjDocument *d,int token,char *out,size_t cap){
    if(!d||token<0||(size_t)token>=d->count||!out||!cap||d->tokens[token].type!=VJ_STRING)return UMI_STATUS_PARSE_ERROR;
    size_t p=d->tokens[token].start,used=0;
    out[0]=0;
    while(p<d->tokens[token].end){
        uint32_t cp;
        if(!Character(d->bytes,d->tokens[token].end,&p,&cp)||cp==0)return UMI_STATUS_PARSE_ERROR;
        unsigned char b[4];
        size_t n;
        if(cp<0x80U){
            b[0]=(unsigned char)cp;
            n=1;
        }
        else if(cp<0x800U){
            b[0]=(unsigned char)(0xc0U|(cp>>6));
            b[1]=(unsigned char)(0x80U|(cp&63U));
            n=2;
        }
        else if(cp<0x10000U){
            b[0]=(unsigned char)(0xe0U|(cp>>12));
            b[1]=(unsigned char)(0x80U|((cp>>6)&63U));
            b[2]=(unsigned char)(0x80U|(cp&63U));
            n=3;
        }
        else{
            b[0]=(unsigned char)(0xf0U|(cp>>18));
            b[1]=(unsigned char)(0x80U|((cp>>12)&63U));
            b[2]=(unsigned char)(0x80U|((cp>>6)&63U));
            b[3]=(unsigned char)(0x80U|(cp&63U));
            n=4;
        }
        if(n>=cap-used)return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(out+used,b,n);
        used+=n;
    }
    out[used]=0;
    return UMI_STATUS_OK;
}
int VjGet(const VjDocument *d,int object,const char *name){
    if(!d||object<0||(size_t)object>=d->count||d->tokens[object].type!=VJ_OBJECT)return -1;
    for(int k=object+1;k<d->tokens[object].next;){
        char key[128];
        int v=d->tokens[k].next;
        if(VjString(d,k,key,sizeof key)==UMI_STATUS_OK&&!strcmp(key,name))return v;
        k=d->tokens[v].next;
    }
    return -1;
}
UmiStatus VjUnsigned(const VjDocument *d,int t,uint64_t *out){
    if(!d||!out||t<0||(size_t)t>=d->count||d->tokens[t].type!=VJ_NUMBER)return UMI_STATUS_PARSE_ERROR;
    uint64_t v=0;
    for(size_t p=d->tokens[t].start;p<d->tokens[t].end;++p){
        unsigned c=d->bytes[p];
        if(c<'0'||c>'9'||v>(UINT64_MAX-(c-'0'))/10U)return UMI_STATUS_PARSE_ERROR;
        v=v*10U+(c-'0');
    }
    *out=v;
    return UMI_STATUS_OK;
}
