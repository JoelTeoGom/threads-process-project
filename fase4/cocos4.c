/*****************************************************************************/
/*									                                         */
/*				     cocos1.c				                                 */
/*									                                         */
/*     Programa inicial d'exemple per a les practiques 2.1 i 2.2 de FSO.     */
/*     Es tracta del joc del menjacocos: es dibuixa un laberint amb una      */
/*     serie de punts (cocos), els quals han de ser "menjats" pel menja-     */
/*     cocos. Aquest menjacocos es representara amb el caracter '0', i el    */
/*     moura l'usuari amb les tecles 'w' (adalt), 's' (abaix), 'd' (dreta)   */
/*     i 'a' (esquerra). Simultaniament hi haura un conjunt de fantasmes,    */
/*     representats per numeros de l'1 al 9, que intentaran capturar al      */
/*     menjacocos. En la primera versio del programa, nomes hi ha un fan-    */
/*     tasma.								                                 */
/*     Evidentment, es tracta de menjar tots els punts abans que algun fan-  */
/*     tasma atrapi al menjacocos.					                         */
/*									                                         */
/*  Arguments del programa:						                             */
/*     per controlar la posicio de tots els elements del joc, cal indicar    */
/*     el nom d'un fitxer de text que contindra la seguent informacio:	     */
/*		n_fil1 n_col fit_tauler creq				                         */
/*		mc_f mc_c mc_d mc_r						                             */
/*		f1_f f1_c f1_d f1_r						                             */
/*									                                         */
/*     on 'n_fil1', 'n_col' son les dimensions del taulell de joc, mes una   */
/*     fila pels missatges de text a l'ultima linia. "fit_tauler" es el nom  */
/*     d'un fitxer de text que contindra el dibuix del laberint, amb num. de */
/*     files igual a 'n_fil1'-1 i num. de columnes igual a 'n_col'. Dins     */
/*     d'aquest fitxer, hi hauran caracter ASCCII que es representaran en    */
/*     pantalla tal qual, excepte el caracters iguals a 'creq', que es visua-*/
/*     litzaran invertits per representar la paret.			                 */
/*     Els parametres 'mc_f', 'mc_c' indiquen la posicio inicial de fila i   */
/*     columna del menjacocos, aixi com la direccio inicial de moviment      */
/*     (0 -> amunt, 1-> esquerra, 2-> avall, 3-> dreta). Els parametres	     */
/*     'f1_f', 'f1_c' i 'f1_d' corresponen a la mateixa informacio per al    */
/*     fantasma 1. El programa verifica que la primera posicio del menja-    */
/*     cocos o del fantasma no coincideixi amb un bloc de paret del laberint.*/
/*	   'mc_r' 'f1_r' son dos reals que multipliquen el retard del moviment.  */
/*     A mes, es podra afegir un segon argument opcional per indicar el      */
/*     retard de moviment del menjacocos i dels fantasmes (en ms);           */
/*     el valor per defecte d'aquest parametre es 100 (1 decima de segon).   */
/*									                                         */
/*  Compilar i executar:					  	                             */
/*     El programa invoca les funcions definides a 'winsuport.h', les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':				                     */
/*									                                         */
/*	   $ gcc -Wall cocos0.c winsuport.o -o cocos0 -lcurses		             */
/*	   $ ./cocos0 fit_param [retard]				                         */
/*									                                         */
/*  Codis de retorn:						  	                             */
/*     El programa retorna algun dels seguents codis al SO:		             */
/*	0  ==>  funcionament normal					                             */
/*	1  ==>  numero d'arguments incorrecte 				                     */
/*	2  ==>  fitxer de configuracio no accessible			                 */
/*	3  ==>  dimensions del taulell incorrectes			                     */
/*	4  ==>  parametres del menjacocos incorrectes			                 */
/*	5  ==>  parametres d'algun fantasma incorrectes			                 */
/*	6  ==>  no s'ha pogut crear el camp de joc			                     */
/*	7  ==>  no s'ha pogut inicialitzar el joc			                     */
/*****************************************************************************/


#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "../librerias/winsuport2.h"	 /* incloure definicions de funcions propies */
#include "../librerias/memoria.h"
#include "../librerias/semafor.h"
#include "../librerias/missatge.h"

#define MIN_FIL 7 /* definir limits de variables globals */
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80

#define F_MAX 9
#define MAX_THREADS 1
#define MAX_PROCS 9
/* definir estructures d'informacio */
typedef struct
{          /* per un objecte (menjacocos o fantasma) */
  int f;   /* posicio actual: fila */
  int c;   /* posicio actual: columna */
  int d;   /* direccio actual: [0..3] */
  float r; /* per indicar un retard relati */
  char a;  /* caracter anterior en pos. actual */
} objecte;

/*variables globals noves fase 1*/
int fi1 = 0;

pthread_t tid[MAX_THREADS];                        /* taula d'identificadors dels threads */

objecte f_list[F_MAX];                             // a la posicio 0 anira el comecocos

/*fase 3*/
int *pt_camp, id_camp, mida_camp;   //camp

pid_t tpid[MAX_PROCS]; /* taula d'identificadors dels processos fill */

int *p_fi, id_fi;   //final

/*fase 4*/
int id_semafor;

////////////////////////////////////////777
int n_fantasma = 0; // numero de fantasmes en joc
int retard;
/* variables globals */
int n_fil1, n_col; /* dimensions del camp de joc */
char tauler[70];   /* nom del fitxer amb el laberint de joc */
char c_req;        /* caracter de pared del laberint */

int df[] = {-1, 0, 1, 0}; /* moviments de les 4 direccions possibles */
int dc[] = {0, -1, 0, 1}; /* dalt, esquerra, baix, dreta */

int cocos;  /* numero restant de cocos per menjar */
int retard; /* valor del retard de moviment, en mil.lisegons */

/* funcio per realitzar la carrega dels parametres de joc emmagatzemats */
/* dins d'un fitxer de text, el nom del qual es passa per referencia a  */
/* 'nom_fit'; si es detecta algun problema, la funcio avorta l'execucio */
/* enviant un missatge per la sortida d'error i retornant el codi per-	*/
/* tinent al SO (segons comentaris al principi del programa).		    */
void carrega_parametres(const char *nom_fit)
{
  FILE *fit;

  fit = fopen(nom_fit, "rt"); /* intenta obrir fitxer */
  if (fit == NULL)
  {
    fprintf(stderr, "No s'ha pogut obrir el fitxer \'%s\'\n", nom_fit);
    exit(2);
  }

  if (!feof(fit))
    fscanf(fit, "%d %d %s %c\n", &n_fil1, &n_col, tauler, &c_req);
  else
  {
    fprintf(stderr, "Falten parametres al fitxer \'%s\'\n", nom_fit);
    fclose(fit);
    exit(2);
  }
  if ((n_fil1 < MIN_FIL) || (n_fil1 > MAX_FIL) ||
      (n_col < MIN_COL) || (n_col > MAX_COL))
  {
    fprintf(stderr, "Error: dimensions del camp de joc incorrectes:\n");
    fprintf(stderr, "\t%d =< n_fil1 (%d) =< %d\n", MIN_FIL, n_fil1, MAX_FIL);
    fprintf(stderr, "\t%d =< n_col (%d) =< %d\n", MIN_COL, n_col, MAX_COL);
    fclose(fit);
    exit(3);
  }

  if (!feof(fit))
    fscanf(fit, "%d %d %d %f\n", &f_list[0].f, &f_list[0].c, &f_list[0].d, &f_list[0].r);
  else
  {
    fprintf(stderr, "Falten parametres al fitxer \'%s\'\n", nom_fit);
    fclose(fit);
    exit(2);
  }
  if ((f_list[0].f < 1) || (f_list[0].f > n_fil1 - 3) ||
      (f_list[0].c < 1) || (f_list[0].c > n_col - 2) ||
      (f_list[0].d < 0) || (f_list[0].d > 3))
  {
    fprintf(stderr, "Error: parametres menjacocos incorrectes:\n");
    fprintf(stderr, "\t1 =< mc.f (%d) =< n_fil1-3 (%d)\n", f_list[0].f, (n_fil1 - 3));
    fprintf(stderr, "\t1 =< mc.c (%d) =< n_col-2 (%d)\n", f_list[0].c, (n_col - 2));
    fprintf(stderr, "\t0 =< mc.d (%d) =< 3\n", f_list[0].d);
    fclose(fit);
    exit(4);
  }

  int i = 1;
  FILE *fp = freopen("error.txt", "a", stderr);
  while (!feof(fit) && i < F_MAX)
  {
    fscanf(fit, "%d %d %d %f\n", &f_list[i].f, &f_list[i].c, &f_list[i].d, &f_list[i].r);

    fprintf(stderr, "%d %d %d %f\n", f_list[i].f, f_list[i].c, f_list[i].d, f_list[i].r);

    if ((f_list[i].f < 1) || (f_list[i].f > n_fil1 - 3) ||
        (f_list[i].c < 1) || (f_list[i].c > n_col - 2) ||
        (f_list[i].d < 0) || (f_list[i].d > 3))
    {
      fprintf(stderr, "Error: parametres fantasma %d incorrectes:\n", i);
      fprintf(stderr, "\t1 =< f1.f (%d) =< n_fil1-3 (%d)\n", f_list[i].f, (n_fil1 - 3));
      fprintf(stderr, "\t1 =< f1.c (%d) =< n_col-2 (%d)\n", f_list[i].c, (n_col - 2));
      fprintf(stderr, "\t0 =< f1.d (%d) =< 3\n", f_list[i].d);
      fclose(fit);
      exit(5);
    }
    i++;
    n_fantasma++;
  }

  fclose(fit); /* fitxer carregat: tot OK! */
  printf("Joc del MenjaCocos\n\tTecles: \'%c\', \'%c\', \'%c\', \'%c\', RETURN-> sortir\n",
         TEC_AMUNT, TEC_AVALL, TEC_DRETA, TEC_ESQUER);
  printf("prem una tecla per continuar:\n");
  getchar();
}

/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
void inicialitza_joc(void)
{
  int r, i, j;
  char strin[12];

  id_camp = ini_mem(mida_camp);
  pt_camp = (int*) map_mem(id_camp);
  win_set((void*) (intptr_t) pt_camp, n_fil1, n_col);

  r = win_carregatauler(tauler, n_fil1 - 1, n_col, c_req);
  fprintf(stderr, "%d \n", r);
  fprintf(stderr, "mida: %d \n", mida_camp);
  fprintf(stderr, "caracter: %c \n", c_req);
  if (r == 0)
  {
    f_list[0].a = win_quincar(f_list[0].f, f_list[0].c);
    if (f_list[0].a == c_req)
      r = -6; /* error: menjacocos sobre pared */
    else
    {

      for (int i = 1; i <= n_fantasma; i++)
      {
        f_list[i].a = win_quincar(f_list[i].f, f_list[i].c);
        char num = i + '0';
        win_escricar(f_list[i].f, f_list[i].c, num, NO_INV);
      }
      cocos = 0; /* compta el numero total de cocos */
      for (i = 0; i < n_fil1 - 1; i++)
        for (j = 0; j < n_col; j++)
          if (win_quincar(i, j) == '.')
            cocos++;

      win_escricar(f_list[0].f, f_list[0].c, '0', NO_INV);

      if (f_list[0].a == '.')
        cocos--; /* menja primer coco */

      sprintf(strin, "Cocos: %d", cocos);
      win_escristr(strin);
    }
  }
  if (r != 0)
  {
    win_fi();
    fprintf(stderr, "Error: no s'ha pogut inicialitzar el joc:\n");
    switch (r)
    {
    case -1:
      fprintf(stderr, "  nom de fitxer erroni\n");
      break;
    case -2:
      fprintf(stderr, "  numero de columnes d'alguna fila no coincideix amb l'amplada del tauler de joc\n");
      break;
    case -3:
      fprintf(stderr, "  numero de columnes del laberint incorrecte\n");
      break;
    case -4:
      fprintf(stderr, "  numero de files del laberint incorrecte\n");
      break;
    case -5:
      fprintf(stderr, "  finestra de camp de joc no oberta\n");
      break;
    case -6:
      fprintf(stderr, "  posicio inicial del menjacocos damunt la pared del laberint\n");
      break;
    case -7:
      fprintf(stderr, "  posicio inicial del fantasma damunt la pared del laberint\n");
      break;
    }
    exit(7);
  }
}



void *mou_menjacocos(void *null) {
  char strin[12];
  objecte seg;
  int tec, ret;
  ret = 0;

  do {
    win_retard(retard);
    waitS(id_semafor);
    tec = win_gettec();  

    if (tec != 0) {
      switch (tec) {
        case TEC_AMUNT:
          f_list[0].d = 0;
          break;
        case TEC_ESQUER:
          f_list[0].d = 1;
          break;
        case TEC_AVALL:
          f_list[0].d = 2;
          break;
        case TEC_DRETA:
          f_list[0].d = 3;
          break;
        case TEC_RETURN:
          ret = -1;
          break;
      }
    }

    seg.f = f_list[0].f + df[f_list[0].d];  // Calcular seguent posicio
    seg.c = f_list[0].c + dc[f_list[0].d];
    seg.a = win_quincar(seg.f, seg.c);  // Calcular caracter seguent posicio
    signalS(id_semafor);
    
    waitS(id_semafor);
    if ((seg.a == ' ') || (seg.a == '.')) {
      
      win_escricar(f_list[0].f, f_list[0].c, ' ', NO_INV);  // Esborra posicio anterior
      
      f_list[0].f = seg.f;
      f_list[0].c = seg.c;  // Actualitza posicio
      
      win_escricar(f_list[0].f, f_list[0].c, '0', NO_INV);  // Redibuixa menjacocos

      if (seg.a == '.') {
        
        cocos--;
        sprintf(strin, "Cocos: %d", cocos);
        win_escristr(strin);
        
        if (cocos == 0)
          fi1 = 1;
      }
      
    }
    signalS(id_semafor);
    win_update();
  } while (!fi1 && !(*p_fi));

  return 0;
}


/* programa principal				    */
int main(int n_args, const char *ll_args[])
{
  char a1[200], a2[10], a3[10], a4[100], a5[100], a6[100],a7[100], a8[10], a9[100], a10[100], a11[10], a12[10];
  int i,n,t,t_total;
  int rc, p; /* variables locals */
 
  srand(getpid()); /* inicialitza numeros aleatoris */

  if ((n_args != 2) && (n_args != 3))
  {
    fprintf(stderr, "Comanda: cocos0 fit_param [retard]\n");
    exit(1);
  }
  carrega_parametres(ll_args[1]);

  if (n_args == 3)
    retard = atoi(ll_args[2]);
  else
    retard = 100;
  

  FILE* file = stderr; // Obtener el puntero al flujo de error estándar

  mida_camp = win_ini(&n_fil1, &n_col, '+', INVERS); /* intenta crear taulell */

  //variable compartida del final de joc2 (fi2 = si fantasma es menja al menjacocos)
  id_fi = ini_mem(sizeof(int));
  p_fi = map_mem(id_fi);
  *p_fi = 0;
   

  id_semafor = ini_sem(1);      //Inicialitzem el semàfor
  sprintf(a12, "%d", id_semafor);

  fprintf(file, "id_semafor: %d.\n", id_semafor);
 //char a; 
  if (mida_camp > 0)                                /* si aconsegueix accedir a l'entorn CURSES */
  {
    inicialitza_joc();
  
    p = 0;

    pthread_create(&tid[0], NULL, mou_menjacocos, NULL);
    
    sprintf(a1, "%d", id_camp); /* convertir id. memoria en string */
    sprintf(a2, "%d", n_fil1);
    sprintf(a3, "%d", n_col);
    sprintf(a4, "%d", id_fi);
    sprintf(a11, "%d", retard);
   


    for ( i = 1; i <= n_fantasma; i++)
    {
      tpid[i] = fork(); /* crea un nou proces */
      if (tpid[i] == (pid_t) 0) /* branca del fill */
      {
        sprintf(a5, "%d", f_list[i].f); //int f;  
        sprintf(a6, "%d", f_list[i].c);  //int c;   
        sprintf(a7, "%d", f_list[i].d);  //int d;  
        sprintf(a8, "%f", f_list[i].r); //float r; 
        sprintf(a9, "%c", f_list[i].a); //char a;  
        sprintf(a10, "%d", i);

        execlp("./fase4/fantasma4", "fase4/fantasma4", a1, a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12, (char *)0);
        exit(0);
      }
        
    }
    pthread_join(tid[0], NULL);

    while (!(*p_fi) && !fi1){
        win_retard(100);
        win_update();
    }

 
    win_fi();

    elim_mem(id_fi);
    elim_mem(id_camp);
    elim_sem(id_semafor);

    if (fi1 == -1)
      printf("S'ha aturat el joc amb tecla RETURN!\n");
    else
    {
      if (fi1)
        printf("Ha guanyat l'usuari!\n");
      else
        printf("Ha guanyat l'ordinador!\n");
    }
  }
  else
  {
    fprintf(stderr, "Error: no s'ha pogut crear el taulell:\n");
    switch (mida_camp)
    {
    case -1:
      fprintf(stderr, "camp de joc ja creat!\n");
      break;
    case -2:
      fprintf(stderr, "no s'ha pogut inicialitzar l'entorn de curses!\n");
      break;
    case -3:
      fprintf(stderr, "les mides del camp demanades son massa grans!\n");
      break;
    case -4:
      fprintf(stderr, "no s'ha pogut crear la finestra!\n");
      break;
    }
    exit(6);
  }
  return (0);
}

