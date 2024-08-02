/*!
*\file user_management.h
*\brief Fichier d'entête des fonctions de gestion des utilisateurs
*/

#pragma once

#include <sys/mman.h>
#include <unistd.h> //cwd for get path to users.csv
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../utils/request.h"
#include "../utils/utils.h"

// Number of user that can be connected at the same time
#define MAX_USERS_CONNECTED 3
//File of account's username/password
#define ACCOUNT_FILE "users.txt"

struct user{
    char username[MAX_USER_USERNAME_LENGTH]; //Username of the user
    int sock; //Socket descriptor or token if first message not sent
    char token[TOKEN_SIZE];
};

/**
*\brief Generate a random token (with alphanumeric characters)
*
*\param char* res of size TOKEN_SIZE, generated token is returned
 */
void token_generation(char* res);

/**
*\brief Add user to the shared_memory (array of connected users)
*
*\param shared_memory Array of connected users
*\param username Username of the user to add
*\param token Token generated returned
*\return 0 User successfully added
*\return 1 Username already taken
*\return 2 Users array is full (No index available)
*/
int add_user(struct user *shared_memory, char username[MAX_USER_USERNAME_LENGTH], char* token);

/**
*\brief Remove user from the shared_memory (array of connected users)
*
*\param shared_memory Array of connected users
*\param token Token of the user to remove(got when log in)
*\return 0 User successfully removed
*\return 1 User not found
*/
int remove_user(struct user *shared_memory, char token[TOKEN_SIZE]);

/**
*\brief returns the number of lines of the csv
*
*\param path Path of the .csv
*\return int number of lines
*/
int numberOfLines(char* path);

/**
*\brief gives a list having one line per nickname and password of the csv
*
*\param path Path of the .csv
*\return char** list of useful data
*/
char** listOfCouples(char* path); 

/**
*\brief either finds the given nickname or says if the password matches with the nickname
*
*\param nickname username you want to check
*\param password password you may want to check
*\param path Path of the .csv
*\param checkPass indicates weither you want to check the password (1) or check the username (0)
*\return -1 if the username doesn't exist
*\return [|0,n|] for the position of the username
*\return 0 if the password doesnt match
*\return 1 if the password does match
*/
int findNickname(char* nickname, char* password, char* path, int checkPass);

/**
*\brief writes down the combination nickname password in the .csv
*
*\param nickname username you want to check
*\param password password you may want to check
*\param path Path of the .csv
*\return 0 if account not created
*\return 1 if account created
*/
int creation(char* nickname,char* password,char* path);

/**
*\brief deletes the combination associated with nickname in the .csv
*
*\param nickname username you want to delete
*\param path Path of the .csv
*\return 0 if account not deleted
*\return 1 if account deleted
*/
int delete(char* nickname,char* path);