/*!=================================================================!*/
/*!= E.Incerti - eric.incerti@univ-eiffel.fr                       =!*/
/*!= Université Gustave Eiffel                                     =!*/
/*!= Code "squelette" pour prototypage avec libgfl.7e              =!*/
/*!=================================================================!*/

/* le seul #include nécessaire a priori
 * contient les libs C standards et OpenGl */
#include <gfl.h>

#include "Link.hpp"
#include "PMat.hpp"

/* tailles initiales de la fenêtre graphique (en pixels)     */
#define WWIDTH  720 
#define WHEIGHT 540

#define NBM 3 // Nb of particles
#define NBL 2 // Nb of links

/* -----------------------------------------------------------------------
 * ici, en général pas mal de variables GLOBALES
 * - les variables de données globales (points, vecteurs....)
 * - les FLAGS de dialogues
 * - les paramètres de dialogue
 * - ......
 * Pas trop le choix, puisque TOUT passe par des fonctions <void f(void)>
 * ----------------------------------------------------------------------- */
float h;
PMat TabM[NBM];
Link TabL[NBL];

/* la fonction d'initialisation : appelée 1 seule fois, au début     */
static void init(void)
{
	/*!  Ici, pas de dessin, pas de calcul dynamique, rien que des initialisations
	*   -> allocation(s) de mémoire, init. des paramètres globaux,
	*      construction de modèles....
	*      ouverture de fichiers, pré-chargement de données....
	*
	*   Tout ce qu'il y a ici pourrait être écrit directement dans le main()
	*   juste après l'appel à gfl_InitWindow()
	!*/
	TabM[0] = PMat(1.0f, gfl_Point2d(-1, 1), gfl_Vector2d(1, 1),  gfl_Vector2d(1, 1), 0);
	TabM[1] = PMat(1.0f, gfl_Point2d( 0, 1), gfl_Vector2d(1, 1),  gfl_Vector2d(1, 1), 0);
	TabM[2] = PMat(1.0f, gfl_Point2d( 1, 1), gfl_Vector2d(1, 1),  gfl_Vector2d(1, 1), 0);

	TabL[0].Connect(TabM[0], TabM[1]);
	TabL[1].Connect(TabM[1], TabM[2]);

	TabL[0].Hook_Spring(1.0f);
	TabL[1].Hook_Spring(1.0f);
}

/* la fonction de contrôle : appelée 1 seule fois, juste APRES <init> */
static void ctrl(void)
{
	/*! Interface de dialogue (partie statique) : création des boutons, scrollbars....
	*  Tout ce qu'il y a ici pourrait être directement écrit dans la fonction init(),
	*  mais c'est plus 'propre' et plus pratique de séparer.
	!*/
}

/* la fonction de contrôle : appelée 1 seule fois, juste APRES <init> */
static void evts(void)
{
	/*! Interface de dialogue (partie dynamique) : les touches clavier, la souris ....
	*  Tout ce qu'il y a ici pourrait être directement écrit dans la fonction draw(),
	*  mais c'est plus 'propre' et plus pratique de séparer.
	!*/
}

/* la fonction de dessin : appelée en boucle (indispensable) */
static void draw(void)
{
	/*! C'est la fonction de dessin principale : elle ne réalise que de l'affichage
	*  sa mise à jour est automatique :
	*  - si un paramètre de contrôle est modifié
	*  - si la fonction <anim()> (calcul) est activée
	*  ATTENTION : surtout pas d'alloc. mémoire ici !!!
	!*/
	gfl_Axes(); /* spécifique 2D : affiche le repère principal (O,x,y) */
}

/* la fonction d'animation : appelée en boucle draw/anim/draw/anim... (facultatif) */
static void anim(void)
{
	/*! C'est la fonction de 'calcul' qui va modifier les 'objets' affichés
	*  par la fonction de dessin (déplacement des objets, calculs divers...)
	*  Si elle n'est pas définie, c'est qu'il n'y a pas d'animation.
	*  ATTENTION : surtout pas d'alloc. mémoire ici !!!
	!*/
}

/* la fonction de sortie  (facultatif) */
static void quit(void)
{
	/*! Ici, les opérations à réaliser en sortie du programme
	*  - libération de la mémoire éventuellement alloueé dans <init()>
	*  - fermeture de fichiers ....
	*  - bilan et messages...
	*  Au final cette fonction est exécutée par un appel à <atexit()>
	!*/
}

/***************************************************************************/
/* La fonction principale : NE CHANGE (presque) JAMAIS                     */
/***************************************************************************/
int main(int argc, char **argv)
{
	/* 1°) creation de la fenetre - titre et tailles (pixels)  */
	gfl_InitWindow(*argv,WWIDTH,WHEIGHT);
	/* 2°) définition de la zone de travail en coord. réeelles *
	*     par défaut (pas d'initialisation) la zone 'réelle'  *
	*     est [(0.,0.),(WWIDTH,WHEIGHT)]                      */
	gfl_SetCenteredDrawZone(0.,0.,10.,0.);
	/* --- autre façon de faire ---
	*     ATTENTION : veiller à respecter les proportions
	*                 (wxmax-wxmin)/(wymax-wymin) = WWIDTH/WHEIGHT
	*/
	//double wxmin = -10., wymin = -10.,
			//wxmax = +10., wymax = +10.;
	//gfl_SetWindowCoord(wxmin,wymin,wxmax,wymax);



	/* 3°) association des fonctions */
	gfl_SetInitFunction(init); /* fonction d'initialisation */
	gfl_SetCtrlFunction(ctrl); /* fonction de contrôle      */
	gfl_SetEvtsFunction(evts); /* fonction d'événements     */
	gfl_SetDrawFunction(draw); /* fonction de dessin        */
	gfl_SetAnimFunction(anim); /* fonction d'animation      */
	gfl_SetExitFunction(quit); /* fonction de sortie        */

	/* 4°) lancement de la boucle principale */
	return gfl_MainStart();
	/* RIEN APRES CA */
}
