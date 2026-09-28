/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "lesson.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "demo") == 0) return UmiBankReviewLesson(true);
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) return UmiBankReviewLesson(false);
    puts("Umicom Bank command review\n  demo         Explain a fictional transfer before applying it in memory\n"
         "  --self-test  Run the same native lesson without its full report\n  --help       Show this help\n"
         "No command reads your Bank database or starts a real payment.");
    return argc == 1 || (argc == 2 && strcmp(argv[1], "--help") == 0) ? 0 : 2;
}
