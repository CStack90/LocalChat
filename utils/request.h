/*!
*\file request.h
*\brief Fichier d'entête des fonctions de gestion des utilisateurs
*/

#pragma once

// Port for UDP connection
#define UDP_PORT 2058
// Port for TCP connection
#define TCP_PORT 2059
// Max data transmitted in a request data
#define REQUEST_DATA_MAX_LENGTH 128
// User's username length
#define MAX_USER_USERNAME_LENGTH 10
// User's password length
#define MAX_USER_PASSWORD_LENGTH 10
//Token size (must be less than request data length)
#define TOKEN_SIZE 16
//Separator (char) between username and password
#define USER_PASSWORD_FILE_SEPARATOR '\t'
//Separator in login, logout, create and delete request
#define USER_PASSWORD_REQUEST_SEPARATOR ' '

struct request
{
    int type;
    /** Type correpondence
    * 1 log in
    * -1 log out
    * 2 create account
    * -2 delete account
    * 0 get current connected users
    */
    char data[REQUEST_DATA_MAX_LENGTH]; //Data sent with request
};
