#pragma once

/*! \page ""
 */
#define  _GNU_SOURCE


/*----Importation des bibliothèques de base----*/
#include <stdio.h>/*bibliothèque d'entrée/sortie standard*/
#include <unistd.h>/*Importation de la bibliothèque standard d'unix*/
#include <stdlib.h>/*bibliothèque standard*/
#include <string.h>/* Bibliothèque pour les chaines de caracteres */
#include <time.h>/* Bibliothèque pour le random (creationTab) */
#include <pthread.h>/* Bibliothèque de gestion des threads */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h> /* pour le isdigit */

/**
*\brief Lecture d'une chaine de caractère
*\param str_chaine Chaine de caractère à entrer
*\param int_taille Taille de la chaine voulue (Longueur réelle)
*\return Taille actuelle de la chaine
*/
int saisieString(char* str_chaine,int int_taille);

/**
*\brief Vérifie si un string est un nombre (ne contient que des chiffres)
*\param source La chaine de caractères à vérifier
*\return 1 si la chaine est un nombre, 0 sinon
*/
int isNumber(char source[]);