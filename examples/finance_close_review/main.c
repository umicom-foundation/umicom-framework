/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "lesson.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) return UmiFinanceCloseLesson(0);
    if (argc == 2 && strcmp(argv[1], "demo") == 0) return UmiFinanceCloseLesson(1);
    puts("Use --self-test or demo. Both use a new memory-only practice book; no broker connection.");
    return argc == 2 && strcmp(argv[1], "--help") == 0 ? 0 : 2;
}
