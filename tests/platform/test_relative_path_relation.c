/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_relative_path_relation.c
 *
 * PURPOSE:
 *   Verify pure portable name relations and unchanged output on admission refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/platform/rooted_files.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); return 1; } } while (0)
typedef struct { const char *name, *first, *second; UmiRootedPathRelation expected; } Case;
static const Case cases[] = {
    {"equal", "src/main.c", "src/main.c", UMI_ROOTED_PATH_EQUAL},
    {"ascii-case", "SRC/Main.c", "src/main.C", UMI_ROOTED_PATH_EQUAL},
    {"separator", "src\\main.c", "src/main.c", UMI_ROOTED_PATH_EQUAL},
    {"first-parent", "src", "src/main.c", UMI_ROOTED_PATH_FIRST_PARENT},
    {"second-parent", "src/main.c", "SRC", UMI_ROOTED_PATH_SECOND_PARENT},
    {"nested-parent", "a/b", "A\\B/c", UMI_ROOTED_PATH_FIRST_PARENT},
    {"boundary", "src", "src2/main.c", UMI_ROOTED_PATH_DISJOINT},
    {"siblings", "src/main.c", "src/test.c", UMI_ROOTED_PATH_DISJOINT},
    {"extension", "file", "file.txt", UMI_ROOTED_PATH_DISJOINT},
    {"unicode-bytes", "caf\xc3\xa9.c", "CAF\xc3\xa9.c", UMI_ROOTED_PATH_EQUAL},
    {"unicode-not-folded", "caf\xc3\xa9.c", "caf\xc3\x89.c", UMI_ROOTED_PATH_DISJOINT},
};
typedef struct { const char *name, *path; } Invalid;
static const Invalid invalid[] = {
    {"empty", ""},
    {"absolute", "/root"},
    {"drive", "C:/root"},
    {"traversal", "a/../b"},
    {"dot", "a/./b"},
    {"double-separator", "a//b"},
    {"trailing-separator", "a/"},
    {"trailing-dot", "a."},
    {"trailing-space", "a "},
    {"reserved", "CON.txt"},
    {"git", ".GiT/config"},
    {"stream", "a:stream"},
    {"control", "a\nb"},
};
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    for (size_t i=0U;i<sizeof cases/sizeof cases[0];++i) {
        if (strcmp(argv[1],cases[i].name)!=0) continue;
        UmiRootedPathRelation observed=UMI_ROOTED_PATH_DISJOINT;
        CHECK(UmiRootedFileValidateRelativePath(cases[i].first)==UMI_STATUS_OK);
        CHECK(UmiRootedFileCompareRelativePaths(cases[i].first,cases[i].second,&observed)==UMI_STATUS_OK);
        CHECK(observed==cases[i].expected);
        return 0;
    }
    for (size_t i=0U;i<sizeof invalid/sizeof invalid[0];++i) {
        if (strcmp(argv[1],invalid[i].name)!=0) continue;
        UmiRootedPathRelation observed=UMI_ROOTED_PATH_SECOND_PARENT;
        CHECK(UmiRootedFileValidateRelativePath(invalid[i].path)!=UMI_STATUS_OK);
        CHECK(UmiRootedFileCompareRelativePaths("valid",invalid[i].path,&observed)!=UMI_STATUS_OK);
        CHECK(observed==UMI_ROOTED_PATH_SECOND_PARENT);
        CHECK(UmiRootedFileCompareRelativePaths(invalid[i].path,"valid",&observed)!=UMI_STATUS_OK);
        CHECK(observed==UMI_ROOTED_PATH_SECOND_PARENT);
        return 0;
    }
    if (strcmp(argv[1],"null")==0) {
        UmiRootedPathRelation observed=UMI_ROOTED_PATH_EQUAL;
        CHECK(UmiRootedFileValidateRelativePath(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiRootedFileCompareRelativePaths(NULL,"valid",&observed)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(observed==UMI_ROOTED_PATH_EQUAL);
        CHECK(UmiRootedFileCompareRelativePaths("a","b",NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1],"capacity")==0) {
        char text[UMI_PATH_CAPACITY+1U]; memset(text,'x',sizeof text);text[sizeof text-1U]='\0';
        UmiRootedPathRelation observed=UMI_ROOTED_PATH_EQUAL;
        CHECK(UmiRootedFileCompareRelativePaths(text,"valid",&observed)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(observed==UMI_ROOTED_PATH_EQUAL);
    } else return 2;
    return 0;
}
