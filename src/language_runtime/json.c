/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/json.c
 *
 * PURPOSE:
 *   Implement a bounded parent-linked JSON tokenizer and value readers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
typedef struct P { const char *j; size_t n,p; UmiLanguageRuntimeJsonDocument *d; int parent; } P;
/* Provide the tok operation used by this module and its client applications. */
static int tok(P *p,UmiLanguageRuntimeJsonTokenType t,int s){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->d->token_count>=UMI_LANGUAGE_RUNTIME_MAX_TOKENS)return -1;i=p->d->token_count++;p->d->tokens[i]=(UmiLanguageRuntimeJsonToken){t,s,-1,0,p->parent};/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->parent>=0)p->d->tokens[p->parent].size++;return(int)i;}
/* Provide the ws operation used by this module and its client applications. */
static void ws(P*p){/* Continue only while work remains available; the loop body advances the state on each pass. */ while(p->p<p->n&&isspace((unsigned char)p->j[p->p]))p->p++;}
/* Provide the val operation used by this module and its client applications. */
static UmiStatus val(P*p);
/* Provide the str operation used by this module and its client applications. */
static UmiStatus str(P*p){int i=tok(p,UMI_LANGUAGE_RUNTIME_JSON_STRING,(int)p->p+1);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i<0)return UMI_STATUS_CAPACITY_EXCEEDED;p->p++;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(p->p<p->n){char c=p->j[p->p++];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='"'){p->d->tokens[i].end=(int)p->p-1;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if((unsigned char)c<0x20U)return UMI_STATUS_PARSE_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\\'){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->p>=p->n)return UMI_STATUS_PARSE_ERROR;c=p->j[p->p++];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='u'){/* Visit each bounded item once so every record receives the same rule. */ for(int k=0;k<4;k++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->p>=p->n||!isxdigit((unsigned char)p->j[p->p]))return UMI_STATUS_PARSE_ERROR;p->p++;}}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strchr("\"\\/bfnrt",c)==NULL)return UMI_STATUS_PARSE_ERROR;}}return UMI_STATUS_PARSE_ERROR;}
/* Provide the prim operation used by this module and its client applications. */
static UmiStatus prim(P*p){size_t s=p->p;int i;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(p->p<p->n){char c=p->j[p->p];/* Preserve the original failure result so the caller can respond to the correct cause. */ if(isspace((unsigned char)c)||c==','||c==']'||c=='}')break;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==':'||c=='"'||c=='['||c=='{'||(unsigned char)c<0x20U)return UMI_STATUS_PARSE_ERROR;p->p++;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(p->p==s)return UMI_STATUS_PARSE_ERROR;i=tok(p,UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE,(int)s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(i<0)return UMI_STATUS_CAPACITY_EXCEEDED;p->d->tokens[i].end=(int)p->p;return UMI_STATUS_OK;}
/* Provide the arr operation used by this module and its client applications. */
static UmiStatus arr(P*p){int old=p->parent,i=tok(p,UMI_LANGUAGE_RUNTIME_JSON_ARRAY,(int)p->p);UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(i<0)return UMI_STATUS_CAPACITY_EXCEEDED;p->parent=i;p->p++;ws(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->p<p->n&&p->j[p->p]==']'){p->d->tokens[i].end=(int)++p->p;p->parent=old;return UMI_STATUS_OK;}/* Visit each bounded item once so every record receives the same rule. */ for(;;){s=val(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;ws(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->p>=p->n)return UMI_STATUS_PARSE_ERROR;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->j[p->p]==']'){p->d->tokens[i].end=(int)++p->p;p->parent=old;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->j[p->p]!=',')return UMI_STATUS_PARSE_ERROR;p->p++;ws(p);}}
/* Provide the obj operation used by this module and its client applications. */
static UmiStatus obj(P*p){int old=p->parent,i=tok(p,UMI_LANGUAGE_RUNTIME_JSON_OBJECT,(int)p->p);UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(i<0)return UMI_STATUS_CAPACITY_EXCEEDED;p->parent=i;p->p++;ws(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->p<p->n&&p->j[p->p]=='}'){p->d->tokens[i].end=(int)++p->p;p->parent=old;return UMI_STATUS_OK;}/* Visit each bounded item once so every record receives the same rule. */ for(;;){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->p>=p->n||p->j[p->p]!='"')return UMI_STATUS_PARSE_ERROR;s=str(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;ws(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->p>=p->n||p->j[p->p]!=':')return UMI_STATUS_PARSE_ERROR;p->p++;ws(p);s=val(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;ws(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->p>=p->n)return UMI_STATUS_PARSE_ERROR;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->j[p->p]=='}'){p->d->tokens[i].end=(int)++p->p;p->parent=old;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->j[p->p]!=',')return UMI_STATUS_PARSE_ERROR;p->p++;ws(p);}}
/* Provide the val operation used by this module and its client applications. */
static UmiStatus val(P*p){ws(p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->p>=p->n)return UMI_STATUS_PARSE_ERROR;/* Select the behaviour associated with the requested command or state value. */ switch(p->j[p->p]){case'{':return obj(p);case'[':return arr(p);case'"':return str(p);default:return prim(p);}}
/*
 * Read language runtime json into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_language_runtime_json_parse(const char*j,UmiLanguageRuntimeJsonDocument*d){P p;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(j==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof(*d));d->json=j;p=(P){j,strlen(j),0U,d,-1};s=val(&p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;ws(&p);return p.p==p.n?UMI_STATUS_OK:UMI_STATUS_PARSE_ERROR;}
/* Provide the eq operation used by this module and its client applications. */
static int eq(const UmiLanguageRuntimeJsonDocument*d,int t,const char*s){size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||s==NULL||t<0||(size_t)t>=d->token_count)return 0;n=strlen(s);return d->tokens[t].type==UMI_LANGUAGE_RUNTIME_JSON_STRING&&d->tokens[t].end-d->tokens[t].start==(int)n&&strncmp(d->json+d->tokens[t].start,s,n)==0;}
/* Provide the sib operation used by this module and its client applications. */
static int sib(const UmiLanguageRuntimeJsonDocument*d,int t){int i=t+1,p=d->tokens[t].parent;/* Continue only while work remains available; the loop body advances the state on each pass. */ while((size_t)i<d->token_count){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(d->tokens[i].parent==p)return i;i++;}return-1;}
/*
 * Provide the language runtime json object get operation used by this module and its
 * client applications.
 */
int umi_language_runtime_json_object_get(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k){int i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||k==NULL||o<0||(size_t)o>=d->token_count||d->tokens[o].type!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return-1;i=o+1;/* Continue only while work remains available; the loop body advances the state on each pass. */ while((size_t)i<d->token_count&&d->tokens[i].parent==o){int v=sib(d,i);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v<0||d->tokens[v].parent!=o)return-1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(eq(d,i,k))return v;i=sib(d,v);}return-1;}
/*
 * Return the number of records represented by language runtime json object without
 * changing their state.
 */
size_t umi_language_runtime_json_object_count(const UmiLanguageRuntimeJsonDocument*d,int o){size_t c=0;int i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||o<0||(size_t)o>=d->token_count||d->tokens[o].type!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return 0;i=o+1;/* Continue only while work remains available; the loop body advances the state on each pass. */ while((size_t)i<d->token_count&&d->tokens[i].parent==o){int v=sib(d,i);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v<0||d->tokens[v].parent!=o)break;c++;i=sib(d,v);}return c;}
/*
 * Find language runtime json object entry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_runtime_json_object_entry_at(const UmiLanguageRuntimeJsonDocument*d,int o,size_t ix,int*k,int*v){size_t c=0;int i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||k==NULL||v==NULL||o<0||(size_t)o>=d->token_count||d->tokens[o].type!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return UMI_STATUS_INVALID_ARGUMENT;i=o+1;/* Continue only while work remains available; the loop body advances the state on each pass. */ while((size_t)i<d->token_count&&d->tokens[i].parent==o){int x=sib(d,i);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(x<0||d->tokens[x].parent!=o)return UMI_STATUS_PARSE_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c++==ix){*k=i;*v=x;return UMI_STATUS_OK;}i=sib(d,x);}return UMI_STATUS_NOT_FOUND;}
/*
 * Find language runtime json array while leaving the underlying catalogue or model owned
 * by this module.
 */
int umi_language_runtime_json_array_at(const UmiLanguageRuntimeJsonDocument*d,int a,size_t ix){size_t c=0;int i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||a<0||(size_t)a>=d->token_count||d->tokens[a].type!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return-1;i=a+1;/* Continue only while work remains available; the loop body advances the state on each pass. */ while((size_t)i<d->token_count&&d->tokens[i].parent==a){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c++==ix)return i;i=sib(d,i);}return-1;}
/*
 * Return the number of records represented by language runtime json array without changing
 * their state.
 */
size_t umi_language_runtime_json_array_count(const UmiLanguageRuntimeJsonDocument*d,int a){size_t c=0;int i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||a<0||(size_t)a>=d->token_count||d->tokens[a].type!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return 0;i=a+1;/* Continue only while work remains available; the loop body advances the state on each pass. */ while((size_t)i<d->token_count&&d->tokens[i].parent==a){c++;i=sib(d,i);}return c;}
/* Provide the hx operation used by this module and its client applications. */
static int hx(char c){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c>='0'&&c<='9')return c-'0';/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c>='a'&&c<='f')return 10+c-'a';/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c>='A'&&c<='F')return 10+c-'A';return-1;}
/* Provide the utf8 operation used by this module and its client applications. */
static UmiStatus utf8(char*out,size_t cap,size_t*u,unsigned cp){unsigned char b[3];size_t n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(cp<=0x7F){b[0]=(unsigned char)cp;n=1;}else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(cp<=0x7FF){b[0]=(unsigned char)(0xC0|(cp>>6));b[1]=(unsigned char)(0x80|(cp&63));n=2;}/* Use this fallback path when the earlier condition does not apply. */ else{b[0]=(unsigned char)(0xE0|(cp>>12));b[1]=(unsigned char)(0x80|((cp>>6)&63));b[2]=(unsigned char)(0x80|(cp&63));n=3;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(*u+n+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out+*u,b,n);*u+=n;out[*u]=0;return UMI_STATUS_OK;}
/*
 * Provide the language runtime json string operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_json_string(const UmiLanguageRuntimeJsonDocument*d,int t,char*out,size_t cap){size_t c,u=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||out==NULL||cap==0||t<0||(size_t)t>=d->token_count||d->tokens[t].type!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_INVALID_ARGUMENT;out[0]=0;c=(size_t)d->tokens[t].start;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(c<(size_t)d->tokens[t].end){unsigned char x=(unsigned char)d->json[c++];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(x=='\\'){char e=d->json[c++];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e=='u'){unsigned cp=0;/* Visit each bounded item once so every record receives the same rule. */ for(int k=0;k<4;k++){int h=hx(d->json[c++]);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h<0)return UMI_STATUS_PARSE_ERROR;cp=cp*16U+(unsigned)h;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(utf8(out,cap,&u,cp)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;continue;}/* Select the behaviour associated with the requested command or state value. */ switch(e){case'"':x='"';break;case'\\':x='\\';break;case'/':x='/';break;case'b':x='\b';break;case'f':x='\f';break;case'n':x='\n';break;case'r':x='\r';break;case't':x='\t';break;default:return UMI_STATUS_PARSE_ERROR;}}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[u++]=(char)x;out[u]=0;}return UMI_STATUS_OK;}
/*
 * Provide the language runtime json int64 operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_json_int64(const UmiLanguageRuntimeJsonDocument*d,int t,int64_t*out){char b[64],*e;long long v;size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||out==NULL||t<0||(size_t)t>=d->token_count||d->tokens[t].type!=UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)return UMI_STATUS_INVALID_ARGUMENT;n=(size_t)(d->tokens[t].end-d->tokens[t].start);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n==0||n>=sizeof(b))return UMI_STATUS_PARSE_ERROR;memcpy(b,d->json+d->tokens[t].start,n);b[n]=0;v=strtoll(b,&e,10);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==b||*e)return UMI_STATUS_PARSE_ERROR;*out=(int64_t)v;return UMI_STATUS_OK;}
/*
 * Provide the language runtime json bool operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_json_bool(const UmiLanguageRuntimeJsonDocument*d,int t,int*out){const UmiLanguageRuntimeJsonToken*x;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||out==NULL||t<0||(size_t)t>=d->token_count)return UMI_STATUS_INVALID_ARGUMENT;x=&d->tokens[t];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(x->type!=UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(x->end-x->start==4&&strncmp(d->json+x->start,"true",4)==0){*out=1;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(x->end-x->start==5&&strncmp(d->json+x->start,"false",5)==0){*out=0;return UMI_STATUS_OK;}return UMI_STATUS_PARSE_ERROR;}
/*
 * Provide the language runtime json is null operation used by this module and its client
 * applications.
 */
int umi_language_runtime_json_is_null(const UmiLanguageRuntimeJsonDocument*d,int t){return d!=NULL&&t>=0&&(size_t)t<d->token_count&&d->tokens[t].type==UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE&&d->tokens[t].end-d->tokens[t].start==4&&strncmp(d->json+d->tokens[t].start,"null",4)==0;}
