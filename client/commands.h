#pragma once

#include "../utils/utils.h"
#include "../utils/request.h"
#include <string.h>

/* Constants definition */
//To log in
#define LOGIN_COMMAND "/login"
//To log out
#define LOGOUT_COMMAND "/logout"
//To create an account
#define CREATE_ACCOUNT_COMMAND "/create"
//To delete an account
#define DELETE_ACCOUNT_COMMAND "/delete"
//To get connected users list
#define LIST_COMMAND "/list"
//To exit the program
#define EXIT_COMMAND "/exit"
//Print help commands
#define HELP_COMMAND "/help"


/**
*\brief Display all commands to the user
*
*/
void printHelp();

/**
*\brief Check if a message start with a specific command
*
*\param message String check matching to command
*\param command Command to check
*\return 1 This is the command
*\return 0 This isn't the command
*/
int is_command(char* message, char* command);

/**
*\brief Get token using UDP connection, send this token using TCP connection
*
*\param message Message containing username and password
*\param token Token got by the server
*\param adr_s Server address
*\param udp_socket Socket from UDP connection (request)
*\param tcp_socket Socket from TCP connection (communication)
*/
void login(char message[REQUEST_DATA_MAX_LENGTH],char* token, struct sockaddr_in adr_s, int udp_socket, int tcp_socket);

/**
*\brief Send token to end properly the program
*
*\param token Token got during the login
*\param adr_s Server address
*\param udp_socket Socket from UDP connection (request)
*\param tcp_socket Socket from TCP connection (communication)
*\param exit_status Exit condition in the communication thread
 */
void logout(char* token, struct sockaddr_in adr_s, int udp_socket, int tcp_socket, int* exit_status);

void createAccount(char message[REQUEST_DATA_MAX_LENGTH], struct sockaddr_in adr_s, int udp_socket);

void deleteAccount(char message[REQUEST_DATA_MAX_LENGTH], struct sockaddr_in adr_s, int udp_socket);

/**
*\brief Print list of all connected users
*
*\param adr_s Server address
*\param udp_socket Socket from UDP connection (request)
 */
void connectedUsers(struct sockaddr_in adr_s, int udp_socket);

/**
*\brief Command manager, call function about command input
*
*\param message User message
*\param exit_status Exit condition in the communication thread 
*\param token Token got by the server (for login and password)
*\param tcp_sock Socket from TCP connection (communication)
*\return 0 No command detected
*\return 1 Command detected
 */
int commande_detection(char message[REQUEST_DATA_MAX_LENGTH], int* exit_status, char* token, int tcp_sock);