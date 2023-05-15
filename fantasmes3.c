/* funcio per moure un fantasma una posicio; retorna 1 si el fantasma   */
/* captura al menjacocos, 0 altrament					*/
int mou_fantasma(void)
{
  objecte seg;
  int ret;
  int k, vk, nd, vd[3];
  
  ret = 0; nd = 0;
  for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
  {
    vk = (f1.d + k) % 4;		/* direccio veina */
    if (vk < 0) vk += 4;		/* corregeix negatius */
    seg.f = f1.f + df[vk]; /* calcular posicio en la nova dir.*/
    seg.c = f1.c + dc[vk];
    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
    if ((seg.a==' ') || (seg.a=='.') || (seg.a=='0'))
    { vd[nd] = vk;			/* memoritza com a direccio possible */
      nd++;
    }
  }
  if (nd == 0)				/* si no pot continuar, */
  	f1.d = (f1.d + 2) % 4;		/* canvia totalment de sentit */
  else
  { if (nd == 1)			/* si nomes pot en una direccio */
  	f1.d = vd[0];			/* li assigna aquesta */
    else				/* altrament */
    	f1.d = vd[rand() % nd];		/* segueix una dir. aleatoria */

    seg.f = f1.f + df[f1.d];  /* calcular seguent posicio final */
    seg.c = f1.c + dc[f1.d];
    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
    win_escricar(f1.f,f1.c,f1.a,NO_INV);	/* esborra posicio anterior */
    f1.f = seg.f; f1.c = seg.c; f1.a = seg.a;	/* actualitza posicio */
    win_escricar(f1.f,f1.c,'1',NO_INV);		/* redibuixa fantasma */
    if (f1.a == '0') ret = 1;		/* ha capturat menjacocos */
  }
  return(ret);
}
