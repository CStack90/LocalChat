#pragma once

#include "../utils/request.h"
#include "../utils/utils.h"
#include "user_management.h"

struct request_processing{
    struct request request;
    unsigned int sock;
    struct sockaddr_in adr_client;
    struct user *shared_memory;
};

struct tcp_informations{
    int sock_c;
    struct user *shared_memory;
};

/**
*\brief Log in an user and put it in the shared memory for make it connected
*
* Exemple request data : MyUser/Password
* There is need to have the character of separation (USER_PASSWORD_REQUEST_SEPARATOR) and username/password length less than max constants
*
*\param args [struct request_processing] all infomations that a thread needs to respond to a request
*\return void* Nothing
*/
void *login(void* args);

/**
*\brief Log out an user and remove it from the shared memory for make it disconnected
*
*\param args [struct request_processing] all infomations that a thread needs to respond to a request
*\return void* Nothing
*/
void *logout(void* args);

/**
*\brief Create an user and put it in the CSV file (used for account store)
*
*\param args [struct request_processing] all infomations that a thread needs to respond to a request
*\return void* Nothing
*/
void *account_creation(void* args);

/**
*\brief Delete an user and remove it from the CSV file (used for account store)
*
*\param args [struct request_processing] all infomations that a thread needs to respond to a request
*\return void* Nothing
*/
void *account_deletion(void* args);

/**
*\brief Collect each user connected to the server and send them to the client
*
*\param args [struct request_processing] all infomations that a thread needs to respond to a request
*\return void* Nothing
*/
void *connected_users(void* args);
