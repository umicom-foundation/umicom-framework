/* Umicom assembly interoperability template | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
extern int umicom_add(int left,int right);
int main(void) { (void)printf("2 + 3 = %d\n",umicom_add(2,3)); return 0; }
