#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>     /* incloure definicions de funcions estandard */
#include <stdlib.h>    /* per exit() */
#include <unistd.h>    /* per getpid() */
#include "../librerias/winsuport2.h"	 /* incloure definicions de funcions propies */
#include "../librerias/memoria.h"

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

objecte f_list[F_MAX];   
   
int n_fil, n_col;

int indice;

int *fi, idFi;
int retard;
int mem;
/* funcio per moure un fantasma una posicio; retorna 1 si el fantasma   */
/* captura al menjacocos, 0 altrament					*/
int mou_fantasma(void)
{
  objecte seg;
  int ret;
  int k, vk, nd, vd[3];

  ret = 0;
  nd = 0;
  do{

    for (k = -1; k <= 1; k++) /* provar direccio actual i dir. veines */
    {
      vk = (f_list[indice].d + k) % 4; /* direccio veina */
      if (vk < 0)
        vk += 4;                    /* corregeix negatius */
      seg.f = f_list[indice].f + df[vk]; /* calcular posicio en la nova dir.*/
      seg.c = f_list[indice].c + dc[vk];
      seg.a = win_quincar(seg.f, seg.c); /* calcular caracter seguent posicio */
      if ((seg.a == ' ') || (seg.a == '.') || (seg.a == '0'))
      {
        vd[nd] = vk; /* memoritza com a direccio possible */
        nd++;
      }
    }
    if (nd == 0)                           /* si no pot continuar, */
      f_list[indice].d = (f_list[indice].d + 2) % 4; /* canvia totalment de sentit */
    else
    {
      if (nd == 1)                     /* si nomes pot en una direccio */
        f_list[indice].d = vd[0];           /* li assigna aquesta */
      else                             /* altrament */
        f_list[indice].d = vd[rand() % nd]; /* segueix una dir. aleatoria */

      seg.f = f_list[indice].f + df[f_list[indice].d]; /* calcular seguent posicio final */
      seg.c = f_list[indice].c + dc[f_list[indice].d];
      seg.a = win_quincar(seg.f, seg.c);                           /* calcular caracter seguent posicio */
      win_escricar(f_list[indice].f, f_list[indice].c, f_list[indice].a, NO_INV); /* esborra posicio anterior */
      f_list[indice].f = seg.f;
      f_list[indice].c = seg.c;
      f_list[indice].a = seg.a; 
      char num =  indice + '0';                             /* actualitza posicio */
      win_escricar(f_list[indice].f, f_list[indice].c, num, NO_INV); /* redibuixa fantasma */
      if (f_list[indice].a == '0')
        *fi = 1; /* ha capturat menjacocos */
      
      win_retard(retard);
    }

  }while ((*fi) == 0);
  

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
  f_list[indice].f = atoi(ll_args[5]);
  f_list[indice].c = atoi(ll_args[6]);
  f_list[indice].d = atoi(ll_args[7]);
  f_list[indice].r = atof(ll_args[8]);
  f_list[indice].a = ll_args[9][0];

  retard = atoi(ll_args[11]);

  

  fi = (int*) map_mem(idFi);

  
  int *p_camp;
  p_camp = (int*) map_mem(mem);
  win_set((void*) (intptr_t)p_camp, n_fil, n_col);


  mou_fantasma();

   
  return (0);
}

