/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifdef UMI_REAL_SQLITE
#include <sqlite3.h>
#endif
int main(void)
{
#ifdef UMI_REAL_SQLITE
    return sqlite3_libversion_number()>0?0:1;
#else
    return 0;
#endif
}
