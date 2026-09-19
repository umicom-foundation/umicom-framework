/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_text_comparison.c
 * PURPOSE: Verify immutable review, exact identity, bounded alignment and navigation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/text_comparison.h"
#include "umicom/developer_productivity/diff_hunk.h"
#include "umicom/developer_productivity/diff_navigation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)

#ifdef UMI_TEST_COMPARISON_ALLOCATION
static long allocation = -1;
static int Fail(void) { if(allocation < 0) return 0; if(allocation-- == 0){allocation=-1;return 1;}return 0; }
void *__real_malloc(size_t n);
void *__real_calloc(size_t n,size_t s);
void *__wrap_malloc(size_t n){return Fail()?NULL:__real_malloc(n);}
void *__wrap_calloc(size_t n,size_t s){return Fail()?NULL:__real_calloc(n,s);}
#endif

static int Create(const char *left, const char *right, UmiTextComparison **out)
{
    CHECK(UmiTextComparisonCreate(left,strlen(left),right,strlen(right),out)==UMI_STATUS_OK);
    return 0;
}

static int Exhaustive(void)
{
    /* Independent forward dynamic programme for all 900 nonempty pairs of
     * binary-alphabet sequences of lengths 1..4. One property test, not 900
     * artificially enumerated CTest entries. */
    for(unsigned leftCode=2U;leftCode<32U;++leftCode) {
        char left[16]={0}; size_t nl=0;
        for(unsigned c=leftCode;c>1U;c>>=1U){left[nl*2U]=(c&1U)?'A':'B';left[nl*2U+1U]='\n';++nl;}
        for(unsigned rightCode=2U;rightCode<32U;++rightCode) {
            char right[16]={0};size_t nr=0;
            for(unsigned c=rightCode;c>1U;c>>=1U){right[nr*2U]=(c&1U)?'A':'B';right[nr*2U+1U]='\n';++nr;}
            size_t dp[5][5]={{0}};
            for(size_t i=1;i<=nl;++i)for(size_t j=1;j<=nr;++j)
                dp[i][j]=left[(i-1U)*2U]==right[(j-1U)*2U]?dp[i-1U][j-1U]+1U:
                    dp[i-1U][j]>dp[i][j-1U]?dp[i-1U][j]:dp[i][j-1U];
            UmiDeveloperDiffDocument *diff=NULL;
            CHECK(umi_developer_diff_document_create(left,right,NULL,&diff)==UMI_STATUS_OK);
            CHECK(umi_developer_diff_document_stats(diff).equal_rows==dp[nl][nr]);
            size_t li=0,ri=0;
            for(size_t row=0;row<umi_developer_diff_document_row_count(diff);++row){
                UmiDeveloperDiffRow entry;
                CHECK(umi_developer_diff_document_row_at(diff,row,&entry)==UMI_STATUS_OK);
                if(entry.left_line){CHECK(entry.left_line==++li);CHECK(entry.left_text[0]==left[(li-1U)*2U]);}
                if(entry.right_line){CHECK(entry.right_line==++ri);CHECK(entry.right_text[0]==right[(ri-1U)*2U]);}
                if(entry.kind==UMI_DEVELOPER_DIFF_EQUAL)CHECK(strcmp(entry.left_text,entry.right_text)==0);
            }
            CHECK(li==nl&&ri==nr);
            umi_developer_diff_document_destroy(diff);
        }
    }
    return 0;
}

static int Run(const char *name)
{
    UmiTextComparison *comparison=NULL;
    UmiTextComparisonSummary summary;
    const char *left=NULL,*right=NULL;size_t ll=0,rl=0,row=SIZE_MAX;
    if(strcmp(name,"lcs-exhaustive")==0)return Exhaustive();
    if(strcmp(name,"alignment")==0 || strcmp(name,"repeated-lines")==0){
        int repeated=strcmp(name,"repeated-lines")==0;
        CHECK(Create(repeated?"A\nB\nA\n":"A\nB\n",repeated?"B\nA\nB\n":"B\nA\n",&comparison)==0);
        CHECK(UmiTextComparisonGetSummary(comparison,&summary)==UMI_STATUS_OK);
        CHECK(summary.changes.equal_rows==(repeated?2U:1U));
        CHECK(summary.changes.inserted_rows==1U&&summary.changes.deleted_rows==1U);
    }else if(strcmp(name,"owned-text")==0){
        char a[]="Umicom Notes: current\n",b[]="Umicom Notes: saved\n";
        CHECK(Create(a,b,&comparison)==0);a[0]='X';b[0]='Y';
        CHECK(UmiTextComparisonText(comparison,0,&left,&ll)==UMI_STATUS_OK);
        CHECK(UmiTextComparisonText(comparison,1,&right,&rl)==UMI_STATUS_OK);
        CHECK(strcmp(left,"Umicom Notes: current\n")==0&&strcmp(right,"Umicom Notes: saved\n")==0);
        CHECK(ll==strlen(left)&&rl==strlen(right));
    }else if(strcmp(name,"final-newline")==0 || strcmp(name,"line-endings")==0){
        CHECK(Create("notes\n",strcmp(name,"line-endings")==0?"notes\r\n":"notes",&comparison)==0);
        CHECK(UmiTextComparisonGetSummary(comparison,&summary)==UMI_STATUS_OK);
        CHECK(!summary.identicalBytes&&summary.changes.changed_rows==0U);
        CHECK(summary.leftFinalNewline);
        CHECK(summary.rightFinalNewline==(strcmp(name,"line-endings")==0));
    }else if(strcmp(name,"empty")==0 || strcmp(name,"identical")==0){
        const char *text=strcmp(name,"empty")==0?"":"notes\n";
        CHECK(Create(text,text,&comparison)==0);
        CHECK(UmiTextComparisonGetSummary(comparison,&summary)==UMI_STATUS_OK);
        CHECK(summary.identicalBytes&&summary.changes.changed_rows==0U);
        CHECK(summary.leftLines==(*text?1U:0U));
        CHECK(UmiTextComparisonNavigate(comparison,SIZE_MAX,1,&row)==UMI_STATUS_NOT_FOUND);
    }else if(strcmp(name,"navigation")==0){
        CHECK(Create("old\nsame\nend\n","new\nsame\nchanged\n",&comparison)==0);
        CHECK(UmiTextComparisonNavigate(comparison,SIZE_MAX,1,&row)==UMI_STATUS_OK&&row==0U);
        CHECK(UmiTextComparisonNavigate(comparison,row,1,&row)==UMI_STATUS_OK&&row==2U);
        CHECK(UmiTextComparisonNavigate(comparison,row,1,&row)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiTextComparisonNavigate(comparison,SIZE_MAX,-1,&row)==UMI_STATUS_OK&&row==2U);
        CHECK(UmiTextComparisonNavigate(comparison,row,-1,&row)==UMI_STATUS_OK&&row==0U);
        CHECK(UmiTextComparisonNavigate(comparison,row,-1,&row)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiTextComparisonNavigate(comparison,SIZE_MAX-1U,-1,&row)==UMI_STATUS_OK&&row==2U);
        CHECK(UmiTextComparisonNavigate(comparison,SIZE_MAX-1U,1,&row)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiTextComparisonNavigate(comparison,0,0,&row)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(strcmp(name,"context-overflow")==0){
        UmiDeveloperDiffDocument *diff=NULL;UmiDeveloperDiffHunkList hunks;
        CHECK(umi_developer_diff_document_create("head\nsame\nold\n","head\nsame\nnew\n",NULL,&diff)==UMI_STATUS_OK);
        CHECK(umi_developer_diff_hunks_build(diff,SIZE_MAX,&hunks)==UMI_STATUS_OK);
        CHECK(hunks.count==1U&&hunks.hunks[0].first_row==0U&&hunks.hunks[0].last_row==2U);
        umi_developer_diff_document_destroy(diff);
    }else if(strcmp(name,"line-limit")==0 || strcmp(name,"long-line")==0 || strcmp(name,"maximum-text")==0){
        size_t n=strcmp(name,"line-limit")==0?1026U:strcmp(name,"long-line")==0?2048U:UMI_TEXT_COMPARISON_MAXIMUM_BYTES;
        char *text=malloc(n+1U);CHECK(text!=NULL);memset(text,'x',n);text[n]='\0';
        if(strcmp(name,"line-limit")==0)for(size_t i=1;i<n;i+=2U)text[i]='\n';
        CHECK(Create(text,text,&comparison)==0);
        CHECK(UmiTextComparisonGetSummary(comparison,&summary)==UMI_STATUS_OK);
        CHECK(summary.alignmentStatus==UMI_STATUS_CAPACITY_EXCEEDED&&summary.identicalBytes);
        CHECK(UmiTextComparisonText(comparison,0,&left,&ll)==UMI_STATUS_OK&&ll==n&&memcmp(left,text,n)==0);
        UmiDeveloperDiffRow entry;
        CHECK(UmiTextComparisonRow(comparison,0,&entry)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiTextComparisonNavigate(comparison,SIZE_MAX,1,&row)==UMI_STATUS_CAPACITY_EXCEEDED);
        free(text);
    }else if(strcmp(name,"invalid")==0){
        char binary[4]={'a','\0','b','\0'};char unterminated[2]={'a','b'};
        CHECK(UmiTextComparisonCreate(NULL,0,"",0,&comparison)==UMI_STATUS_INVALID_ARGUMENT&&!comparison);
        CHECK(UmiTextComparisonCreate("",0,"",0,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTextComparisonCreate(binary,3,"",0,&comparison)==UMI_STATUS_INVALID_ARGUMENT&&!comparison);
        CHECK(UmiTextComparisonCreate(unterminated,1,"",0,&comparison)==UMI_STATUS_INVALID_ARGUMENT&&!comparison);
        CHECK(UmiTextComparisonCreate("",UMI_TEXT_COMPARISON_MAXIMUM_BYTES+1U,"",0,&comparison)==UMI_STATUS_CAPACITY_EXCEEDED&&!comparison);
        CHECK(Create("a","b",&comparison)==0);
        left="old";ll=7;
        CHECK(UmiTextComparisonText(comparison,2,&left,&ll)==UMI_STATUS_INVALID_ARGUMENT&&!left&&!ll);
        CHECK(UmiTextComparisonGetSummary(NULL,&summary)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTextComparisonGetSummary(comparison,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTextComparisonNavigate(NULL,0,1,&row)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTextComparisonRow(comparison,0,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        UmiDeveloperDiffRow entry;
        CHECK(UmiTextComparisonRow(comparison,SIZE_MAX,&entry)==UMI_STATUS_NOT_FOUND);
        UmiTextComparisonDestroy(NULL);
    }else if(strcmp(name,"options")==0){
        UmiDeveloperDiffOptions options;UmiDeveloperDiffDocument *diff=NULL;
        umi_developer_diff_options_init(&options);options.ignore_case=1;options.ignore_trailing_whitespace=1;
        CHECK(umi_developer_diff_document_create("Notes  \n","notes\n",&options,&diff)==UMI_STATUS_OK);
        CHECK(umi_developer_diff_document_stats(diff).equal_rows==1U);umi_developer_diff_document_destroy(diff);
    }
#ifdef UMI_TEST_COMPARISON_ALLOCATION
    else if(strcmp(name,"allocation")==0){
        unsigned failures=0,successes=0;
        for(long n=0;n<12;++n){
            allocation=n;
            UmiStatus status=UmiTextComparisonCreate("old\n",4,"new\n",4,&comparison);
            allocation=-1;
            if(status==UMI_STATUS_OK){++successes;UmiTextComparisonDestroy(comparison);comparison=NULL;}
            else {CHECK(status==UMI_STATUS_OUT_OF_MEMORY&&!comparison);++failures;}
        }
        CHECK(failures>0&&successes>0);
    }
#endif
    else {fprintf(stderr,"Unknown case: %s\n",name);return 1;}
    UmiTextComparisonDestroy(comparison);
    return 0;
}
int main(int argc,char **argv){CHECK(argc==2);int result=Run(argv[1]);if(!result)printf("%s: passed\n",argv[1]);return result;}
