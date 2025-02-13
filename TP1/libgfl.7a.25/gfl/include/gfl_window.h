/*!==================================================================*
 **@file    gfl_window.h
 * @author  Universite Gustave Eiffel
 * @author  E.Incerti - eric.incerti@univ-eiffel.fr
 * @brief   Base public control functions
 * @version 6.e
 * @date    Aug.2022 (doc generation)
 * @cond SHOW_SECURITY_DEFINE
 *!==================================================================*/
#ifdef __cplusplus
  extern "C" {
#else
  #define _GNU_SOURCE
#endif

#ifndef _GFL_WINDOW_H_
  #define _GFL_WINDOW_H_
/**@endcond                   */

  #include <gfl.h>

  /*!****************************************************************************!*/
  /*! LA FENETRE GRAPHIQUE                                                       !*/
  /*!****************************************************************************!*/

  /*! r�glage de la vitesse (synchro verticale) 
   *  - Par d�faut elle est r�gl�e � 60fps (VSYNC_MODE=1)
   *  - si c'est trop rapide, aller voir ce que dit glxgears
  !*/ 
  void gfl_SetFPS(uint fps);
  uint gfl_GetFPS(void);

  /*! initialisation de la fenetre et mise en place des parametres Gl
   * 1� fonction � appeler - indispensable
   * - windname : nom de la fen�tre (le + classique : argv[0])
   * - wwidth/wheight : tailles en pixel
  !*/
  void gfl_InitWindow(char* windname, int wwidth, int wheight);

  #ifdef _GFL2D_
    #include <gfl_pixmap.h> 
    GFLpixmap* gfl_InitImage(char *imgfname, char **pathname, char **rootname, char **extname);
    
    /*! definit la zone graphique reelle associee a la fenetre
     *  >> par d�faut c'est la zone [(0,0)->(wwidth,wheight)]
     *  >> ATTENTION : les ordonn�es (y) vont du BAS vers le HAUT
     *                 contrairement au coord. pixel standard
     *              -> Peut provoquer un renversement vertical
    !*/
    bool gfl_SetWindowCoord(double xmin, double ymin, double xmax, double ymax);

    /*! definit la zone graphique reelle associee a la fenetre  -- variante
     *  (xmin,ymin) : coord. du coin inf�rieur gauche de la fen�tre
     *  - si 'width' ou 'height' n�gatif ou null => width==height (rapport d'aspect 1:1)
     *  - si les 2 sont donn�es (>0.) => rapport d'aspect w:h
     *  - si les 2 sont <=0. => erreur.
     !*/
    bool gfl_SetDrawZone(double xmin, double ymin, double width, double height );
    /*! m�me principe, mais on donne les coord. du centre de la zone -- en g�n�ral (0.,0.) !*/
    bool gfl_SetCenteredDrawZone(double xctr, double yctr, double width, double height );
    
    
    /*! les limites de la fen�tre r�elle
     *  Elle change (ou pas...) lorsque la fen�tre graphique est redimensionn�e
    !*/
    double gfl_GetXMin(void);      /* coordonnee courante en X du bord gauche       */
    double gfl_GetYMin(void);      /* coordonnee courante en Y du bord inf�rieur    */
    double gfl_GetXMax(void);      /* coordonnee courante en X du bord droit        */
    double gfl_GetYMax(void);      /* coordonnee courante en Y du bord sup�rieur    */
  
    double gfl_GetXPixSize(void);  /* taille reelle du pixel en X                   */
    double gfl_GetYPixSize(void);  /* taille reelle du pixel en Y                   */
  
    /*! r�glage/r�cup�ration du facteur de zoom !*/
    void   gfl_SetZoom(double z);  /* fixe  la valeur du acteur de Zoom (x,y)       */
    double gfl_GetZoom(void);      /* r�cup�re la valeur du acteur de Zoom (x,y)    */
  
    /*!****************************************************************************!*/
    /*!  UTILITAIRES PRATIQUES                                                     !*/
    /*!****************************************************************************!*/
    /*! TRUE par d�faut - pour d�activer Zoom et Panscan (fige la fen�tre)          !*/
    void   gfl_SetPanScanAndZoom(bool FLAG);
    /*! trace les axes (0,x,y)                                                      !*/
    void   gfl_Axes(void);
    /*! trace une grilles de graduation                                             !*/
    void   gfl_Grad(GFLcolor col_axe, double step, GFLcolor col_grad);
    /*! trace une grilles de graduation � plusieurs niveaux                         !*/
    void   gfl_MultiGrad(GFLcolor col_axe, int nb_level, ...);
  #endif
  

  #ifdef _GFL3D_
    /*!****************************************************************************!*/
    /*!  UTILITAIRES PRATIQUES                                                     !*/
    /*!****************************************************************************!*/
    /*! fixe / recupere la vitesse d'action de la souris (panscan, mouvements...) !*/
    double gfl_GetMouseSpeed(void);
    void   gfl_SetMouseSpeed(double speed);
  
    /*! affiche un repere global ou local (moyennant la bonne transfo) !*/
    void   gfl_Repere();
  
    /*! fl�che d'origine <O>, de dir./long. <V>
     *  - r   : rayon du cyclindre
     *  - h   : taille de la pointe
     *  - col : couleur
    !*/
    void   gfl_ArrowVect(GFLpoint O, GFLvector V, float r, float h, GFLcolor col);
  #endif


  /*!****************************************************************************!*/
  /*!  LES 6 FONCTIONS ESSENTIELLES DU MOTEUR GRAPHIQUE                          !*/
  /*!****************************************************************************!*/
  /*! 1�) INITIALISATION (chargement)
   *  >>> selectionne la fonction d'initialisation des donnees                        *
   *  >>> Cette fonction est appel�e une seule fois, avant le lancement
   *      de la boucle principale.
   *      C'est elle et elle seule qui doit contenir les �ventuelles allocation
   *      de m�moire.
   *      Elle se combine � la fonction de contr�le d�finie par <gfl_SetCtrlFunction>
   *  >>> Elle d�finit la partie "statique" du programme
   *  <<< Son usage n'est pas indispensable mais conseill�
   *      Tout son contenu pourrait �tre int�gr� directement dans la fonction <main>
   *
  !*/
  void gfl_SetInitFunction(void (*f)(void));

  /*! 2�) INTERFACE DE CONTR�LE (boutons, scrollbars, points de contr�le...)
   *  >>> selectionne la fonction des actions de contr�le (bouttons, scrollbars, clavier...)
   *  >>> Cette fonction est appel�e une seule fois, avant le lancement
   *      de la boucle principale, juste apr�s l'appel  � la fonction d'initialisation
   *      d�finie par <gfl_SetInitFunction>
   *  >>> Elle se combine aux actions et param�tres de contr�le pr�d�finis
   *  <<< Son usage n'est pas indispensable mais conseill�
   *      Tout son contenu pourrait �tre int�gr� dans la fonction d'initialisation
   *      ou directement dans la fonction <main>
  !*/
  void gfl_SetCtrlFunction(void (*f)(void));

  /*! 3�) GESTION DES EVENEMENTS (clavier, souris)
   *  >>> selectionne la fonction de gestion des �v�nements (clavier, souris...)
   *  >>> la gestion des interruption est volontairement brid�e et contraignante
   *      - seul le boutton gauche de la souris est disponible
   *        le boutton droit et la molette son associ�s � des action pr�d�finies
   *      - les touches clavier doivent �tre associ�es � des proc�dures autonomes
   *        de prototype <void action(void)> et certaines s�quences de touches sont
   *        verrouill�es pour des actions pr�d�finies [cf. <gfl_keys.h>] :
   *        <ESC>,<SPACE>,<F11>,<F12>
   *        <Ctrl+'q'>,<Ctl+'f'>,<Ctrl+'w'>,<Ctrl+'r'>
   *        '?','+','-','='
   *      - la prise en compte des interruptions est synchronis�e avec l'appel
   *        � la fonction de 'display' (ou 'refresh')
  !*/
  void gfl_SetEvtsFunction(void (*f)(void));

  /*! 4�) AFFICHAGE - FONCTION DE DESSIN
   *  >>> selectionne la fonction associee aux actions de d'affichage (Display)
   *  >>> elle est indispensable pour afficher quelque chose
   *  >>> elle est appel�e en boucle infinie, en synchro avec la fonction d'anim.
   *      - elle ne doit contenir QUE des appels d'affichage
   *      - SURTOUT PAS d'appel de gestion m�moire (malloc/free)
   *  >>> la synchro affichage/calcul est g�r�e par le moteur GL mais peut �tre
   *      param�tr� par la fonction <gfl_SetRefreshFreq(int F_AFF)> qui fixe le
   *      rapport de fr�quence d'appel pour ces 2 fctions
   *      <F_AFF> appels � la fonction d'anim pour 1 appel � la fonction d'affichage
  !*/
  void gfl_SetDrawFunction(void (*f)(void));

  /*! 5�) ANIMATION - FONCTION DE CALCUL
   *  >>> selectionne la fonction associee aux actions d'animation / calculs
   *  >>> elle est indispensable pour animer quelque chose
   *  >>> elle est appel�e en boucle infinie, en synchro avec la fonction d'affichage.
   *      - elle ne doit contenir QUE des calculs
   *      - SURTOUT PAS d'appel de gestion m�moire (malloc/free)
   *      - pas d'appel d'affichage (ne serait pa pris en compte).
   *  >>> la synchro affichage/calcul est g�r�e par le moteur GL mais peut �tre
   *      param�tr� par la fonction <gfl_SetRefreshFreq(int F_AFF)> qui fixe le
   *      rapport de fr�quence d'appel pour ces 2 fctions
   *      <F_AFF> appels � la fonction d'anim pour 1 appel � la fonction d'affichage
  !*/
  void   gfl_SetAnimFunction(void (*f)(void));

  /*! 6�) SORTIE
   *  >>> selectionne la fonction associee aux actions � faire en sortie (atexit)
   *  >>> c'est elle, en particulier, qui doit lib�rer la m�moire �ventuellement
   *      allou�e dans la fonction d'initialisation
   *  >>> elle est appel�e une seule fois, � la sortie du programme
  !*/
  void   gfl_SetExitFunction(void (*f)(void));

  /*!****************************************************************************!*/
  /*!  LA FONCTION DE DEMARRAGE                                                  !*/
  /*!****************************************************************************!*/
  /*! lance la boucle principale (glutMainLoop)
   *  >>> derni�re fonction � appeler
  !*/
  int    gfl_MainStart(void);

  /*!****************************************************************************!*/
  /*!  RECUPERATION DES VARIABLES D'ENVIRONNEMENT                                !*/
  /*!****************************************************************************!*/
  /*! fixe / recupere la couleur de fond de la fenetre                            !*/
  void   gfl_SetBkGdCol(double BkGd);
  double gfl_GetBkGdCol(void      );
  
  /*! affichage/anim : pause de <usec> micro-sec. entre chaque appel � <anim>    !*/
  void   gfl_SetDelay(uint usec);
  /*! affichage/anim : 1 affichage pour <freq> passes de calcul                  !*/
  void   gfl_SetRefreshFreq(int freq);

  /*! les tailles de la fen�tre graphiquee (pixels)
   *  >>> elles changent lorsque la fen�tre est redimensionn�e                    !*/
  int    gfl_GetPixWidth(void);  /* largeur courante (en pixels)                  */
  int    gfl_GetPixHeight(void); /* hauteur courante (en pixels)                  */

  /*! ====================================================================== !*/
  /*! Affichage de texte
   *  >> OpenGl est tr�s limit� en ce qui concerne l'affichage de texte
   *     il ne propose que 2 fontes, 4 tailles et 1 seul style (normal)
   *     -> n�anmoins un style 'bold' (gras) existe ici
   *     De plus l'affichage de texte s'av�re assez co�teux.
   *     -> donc � �viter dans les appli visant la performance (animations)
  !*/

  /*! Selection de fonte d'�criture (taille et style) :
   *  size  's' (small) 'm' (medium) 'l' (large) 'L' (Large)
   *  style 'n' (normal) 'b' (bold)
   *  pos   'l','c','r' : left, center, right
  !*/
  void gfl_SetFontAttributes(char size, char style, char pos);

  /*! affiche en coord. 'pixel' (entiers : x,y)
   *  les attributs (taille, style, position) sont fix�s par <gfl_SetFontAttributes>
   *  col : couleur du texte
   *  le reste : comme <printf(const char *format, ...)>
   *  le texte garde sa position dans la fen�tre ind�pendemment du redimensionnement,
   *  et des variations (zoom/panscan) de la fen�tre r�elle.
  !*/
  void gfl_StaticPrint(int x, int y, GFLcolor col, const char *format, ...);

  #ifdef _GFL2D_
    /*! affiche en coord. 'fenetre' (r�eels : x,y)
     *  les attributs (taille, style, position) sont fix�s par <gfl_SetFontAttributes>
     *  col : couleur du texte
     *  le reste : comme <printf(const char *format, ...)> *
     *  le texte suit les variations (zoom/panscan) de la fen�tre r�elle.
    !*/
    void gfl_Print(double x, double y, GFLcolor col, const char *format, ...);
  #endif
  
  /*! comme <gfl_StaticPrint> mais avec un cadre
   *  (x,y)  : position d'ancrage fen�tre
   *  margin : marge entre le texte et le cadre
   *  txtcol : couleur du texte et du cadre
   *  bkgcol : couleur de fond
   *  borderw: epaisseur du cadre (pixels)
   *  le reste : comme <printf(const char *format, ...)> *
  !*/
  void gfl_StaticTextBox(int x, int y, int margin,
                         GFLcolor txtcol, GFLcolor bkgcol, int borderw,
                         const char *format, ...);

  /*!****************************************************************************!*/
  /*!  UTILITAIRES D'USAGE PLUS MARGINAL                                         !*/
  /*!****************************************************************************!*/

  /*! int�gr� � la fin de la fonction d'affichage (pass�e � <gfl_SetDrawFunction>,
   *  cet appel permet de faire de l'image par image dans une animation
   *  => affiche une image et s'arr�te.
   *  => la touche "espace" permet de passer � l'�tape suivante.
  !*/
  void   gfl_Stop(void);
  /*! force l'affichage (usage limit� � quelques situations un peu sp�ciales)    !*/
  void   gfl_Refresh(void);
  /*! pour sortir proprement en cas d'urgence (erreur)
   *  >>> elle int�gre la fonction de sortie utilisateur <gfl_SetExitFunction>
   *  >>> normalement il n'y a aucune raison d'en faire usage
   */
  void   gfl_Quit(void);

  void gfl_EnableRealTimeVideo(void);
  void gfl_DisableRealTimeVideo(void);

 #endif

#ifdef __cplusplus
  }
#endif
/*!=============================================================!*/
