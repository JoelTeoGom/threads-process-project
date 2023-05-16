#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>     /* incloure definicions de funcions estandard */
#include <stdlib.h>    /* per exit() */
#include <unistd.h>    /* per getpid() */
#include "../librerias/winsuport2.h"	 /* incloure definicions de funcions propies */
#include "../librerias/memoria.h"


/* funcio per moure un fantasma una posicio; retorna 1 si el fantasma   */
/* captura al menjacocos, 0 altrament					*/
int mou_fantasma(void)
{
  printf("guay");
}


/* programa principal				    */
int main(int n_args, const char *ll_args[])
{
   
  return (0);
}

