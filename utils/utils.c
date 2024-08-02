#include "utils.h"

int saisieString(char* str_chaine,int int_taille){
	/* Déclaration de la chaine de caractere */
	char* ligne;
	/* declaration de la taille de la chaine et du nombre alloué */
	size_t size_alloue;
	int int_tailleReelle;
	do{
		/* Initialisation de ligne */
		ligne = NULL;
		/* Lecture de la chaine de caractère */
		int_tailleReelle = getline(&ligne, &size_alloue,stdin);
		/* Prevention erreur de saisie */
		if (int_tailleReelle > int_taille){
			/* Message à l'utilisateur */
			printf("La longueur ne doit pas excéder %d caractères.\n",int_taille-1);
		}
	/* Demande tant que la chaine n'est pas de la bonne taille */
	}while(int_tailleReelle > int_taille || int_tailleReelle==1);
	/* Copie intégrale de la chaine*/
	strncpy(str_chaine,ligne,int_tailleReelle-1);
	str_chaine[int_tailleReelle-1] = '\0';

	/* Renvoi de la taille */
	return(int_tailleReelle);
}

int isNumber(char source[])
{
    for (int i = 0; source[i]!= '\0'; i++)
    {
        if (isdigit(source[i]) == 0)
              return 0;
    }
    return 1;
}