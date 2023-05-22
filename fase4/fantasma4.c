#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>     /* incloure definicions de funcions estandard */
#include <stdlib.h>    /* per exit() */
#include <unistd.h>    /* per getpid() */
#include "../librerias/winsuport2.h"	 /* incloure definicions de funcions propies */
#include "../librerias/memoria.h"
#include "../librerias/semafor.h"
#include "../librerias/missatge.h"

int df[] = {-1, 0, 1, 0}; /* moviments de les 4 direccions possibles */
int dc[] = {0, -1, 0, 1}; /* dalt, esquerra, baix, dreta */

#define F_MAX 9
//PARAMETROS QUE TENEMOS QUE PASSAR POR MAIN

typedef struct
{          /* per un objecte (menjacocos o fantasma) */
  int f;   /* posicio actual: fila */
  int c;   /* posicio actual: columna */
  int d;   /* direccio actual: [0..3] */
  float r; /* per indicar un retard relati */
  char a;  /* caracter anterior en pos. actual */
} objecte;

objecte f1;   
   
int n_fil, n_col;

int indice;
int id_semafor;
int *fi, idFi;
int retard;
int mem;
/* funcio per moure un fantasma una posicio; retorna 1 si el fantasma   */
/* captura al menjacocos, 0 altrament					*/
int mou_fantasma(void) {
  objecte seg;
  int ret;
  int k, vk, nd, vd[3];

  ret = 0;
  nd = 0;

  do {
    waitS(id_semafor);
    for (k = -1; k <= 1; k++) {
      vk = (f1.d + k) % 4;
      if (vk < 0)
        vk += 4;
      seg.f = f1.f + df[vk];
      seg.c = f1.c + dc[vk];
      seg.a = win_quincar(seg.f, seg.c);

      if ((seg.a == ' ') || (seg.a == '.') || (seg.a == '0')) {
        vd[nd] = vk;
        nd++;
      }      
    }
    signalS(id_semafor);

    waitS(id_semafor);
    if (nd == 0) {
      f1.d = (f1.d + 2) % 4;
      signalS(id_semafor);
    } else {
      if (nd == 1) {
        f1.d = vd[0];
        signalS(id_semafor);
      } else {
        f1.d = vd[rand() % nd];
        signalS(id_semafor);
      }
      seg.f = f1.f + df[f1.d];
      seg.c = f1.c + dc[f1.d];

      waitS(id_semafor);
      seg.a = win_quincar(seg.f, seg.c);
      win_escricar(f1.f, f1.c, ' ', NO_INV);
      f1.f = seg.f;
      f1.c = seg.c;
      f1.a = seg.a;
      signalS(id_semafor);

      waitS(id_semafor);
      char num = indice + '0';
      win_escricar(f1.f, f1.c, num, NO_INV);
      signalS(id_semafor);

      if (f1.a == '0')
        *fi = 1;
      signalS(id_semafor);

      win_retard(retard);
    }
  } while ((*fi) == 0);

  return 0;
}




/* programa principal				    */
int main(int n_args, const char *ll_args[])
{


  FILE* file = stderr; // Obtener el puntero al flujo de error estándar

  // Escribir en el flujo de error estándar
  fprintf(file, "Este es un mensaje de error.\n");


  
  
  mem = atoi(ll_args[1]);
  n_fil = atoi(ll_args[2]);
  n_col = atoi(ll_args[3]);
  idFi = atoi(ll_args[4]);

  indice = atoi(ll_args[10]);
  f1.f = atoi(ll_args[5]);
  f1.c = atoi(ll_args[6]);
  f1.d = atoi(ll_args[7]);
  f1.r = atof(ll_args[8]);
  f1.a = ll_args[9][0];

  retard = atoi(ll_args[11]);

  id_semafor = atoi(ll_args[12]); /* obtenir identificador de semafor */;

  fprintf(file, "mem:%d || n_fil:%d || n_col: %d || indice: %d || f1.f: %d || f1.c: %d ||f1.d: %d || f1.r: %f || f1.a: %c \n", mem,n_fil,n_col,indice,f1.f,f1.c, f1.d, f1.r, f1.a);

  fi = (int*) map_mem(idFi);

  
  int *p_camp;
  p_camp = (int*) map_mem(mem);
  win_set((void*) (intptr_t)p_camp, n_fil, n_col);


  mou_fantasma();

   
  return (0);
}

