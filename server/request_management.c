#include "request_management.h"

void *login(void* args){
    /* Request informations */
    struct request_processing *parent_info = args;

    int separator_pos;//Index of the separator
    char username[MAX_USER_USERNAME_LENGTH], password[MAX_USER_PASSWORD_LENGTH];//Username and password got from request
    char token[TOKEN_SIZE];//Token generated

    char data[REQUEST_DATA_MAX_LENGTH];
    strcpy(data,(*parent_info).request.data);//Put request data in data

    printf("\t[Login-thread] - Received data (length : %ld): %s\n", strlen(data), data); //Log

    /* parse data to username and password */
    //Get pos of separator
    for (separator_pos = 0; separator_pos < strlen(data) && data[separator_pos] != USER_PASSWORD_REQUEST_SEPARATOR; separator_pos++);
    
    //Check string param length
    if (separator_pos >= MAX_USER_USERNAME_LENGTH || strlen(data)-separator_pos > MAX_USER_PASSWORD_LENGTH){
        (*parent_info).request.type = -1; //There is an error
        strcpy((*parent_info).request.data,"Username or password are too long");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client));
        pthread_exit(NULL);
    } else if (separator_pos == 0 || separator_pos == strlen(data) || separator_pos == strlen(data)-1){
        (*parent_info).request.type = -1; //There is an error
        strcpy((*parent_info).request.data,"Username or password are empty");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client));
        pthread_exit(NULL);
    }

    /* Copy username and password from data */
    strncpy(username,data,separator_pos);
    username[separator_pos]='\0';
    strncpy(password,&data[separator_pos]+1,strlen(data)-separator_pos);

    if(findNickname(username,password,ACCOUNT_FILE,1) != 1){
        (*parent_info).request.type = -1; 
        strcpy((*parent_info).request.data,"Wrong username/password");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
    }else{
        /* Adding user to the shared memory */
        switch (add_user((*parent_info).shared_memory,username,&(*token))){
        case 0://All went right
            (*parent_info).request.type = 0;
            strcpy((*parent_info).request.data,token);
            sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
            break;
        case 1://User already connected
            (*parent_info).request.type = -1; 
            strcpy((*parent_info).request.data,"User already connected");
            sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
            break;
        default://Shared memory full of connected user
            (*parent_info).request.type = -1; 
            strcpy((*parent_info).request.data,"Maximum number of simultaneous connections reached");
            sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
            break;
        }
    }
    pthread_exit(NULL);
}

void *logout(void* args){
    /* Request informations */
    struct request_processing *parent_info = args;

    char token[TOKEN_SIZE];//Token got when log in

    /* Token size verification */
    if(strlen((*parent_info).request.data) != (TOKEN_SIZE-1)){ //Token doesn't have the right format
        (*parent_info).request.type = -1; 
        strcpy((*parent_info).request.data,"The token doesn't have the right format");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
    }

    printf("[Logout-thread] - Received data (length : %ld): %s\n", strlen(token), token); //Log
    
    /* Sending response */
    strcpy(token,(*parent_info).request.data);//Setting token variable

    switch (remove_user((*parent_info).shared_memory,token)){//removing user by token
    case 0://All went right
        (*parent_info).request.type = 0;
        strcpy((*parent_info).request.data,"User disconnected");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
        break;
    default://User not found
        (*parent_info).request.type = -1; 
        strcpy((*parent_info).request.data,"User not found");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
        break;
    }
    pthread_exit(NULL);
}

void *account_creation(void* args){
    /* Request informations */
    struct request_processing *parent_info = args;
    int separator_pos;//Index of the separator
    char username[MAX_USER_USERNAME_LENGTH], password[MAX_USER_PASSWORD_LENGTH];//Username and password got from request

    char data[REQUEST_DATA_MAX_LENGTH];
    strcpy(data,(*parent_info).request.data);//Put request data in data

    printf("[Account_creation-thread] - Received data (length : %ld): %s\n", strlen(data), data); //Log
    
    /* parse data to username and password */
    //Get pos of separator
    for (separator_pos = 0; separator_pos < strlen(data) && data[separator_pos] != USER_PASSWORD_REQUEST_SEPARATOR; separator_pos++);

    //Check string param length
    if (separator_pos >= MAX_USER_USERNAME_LENGTH || strlen(data)-separator_pos > MAX_USER_PASSWORD_LENGTH){
        (*parent_info).request.type = -1; //There is an error
        strcpy((*parent_info).request.data,"Username or password are too long");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client));
        pthread_exit(NULL);
    }else if (separator_pos == 0 || separator_pos == strlen(data) || separator_pos == strlen(data)-1){
        (*parent_info).request.type = -1; //There is an error
        strcpy((*parent_info).request.data,"Username or password are empty");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client));
        pthread_exit(NULL);
    }

    /* Copy username and password from data */
    strncpy(username,data,separator_pos);
    username[separator_pos]='\0';
    strncpy(password,&data[separator_pos]+1,strlen(data)-separator_pos);
    
    /* Sending response */
    if(creation(username,password,ACCOUNT_FILE) != 1){
        (*parent_info).request.type = -1; 
        strcpy((*parent_info).request.data,"Username already exists");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
    }else{
        (*parent_info).request.type = 0; 
        strcpy((*parent_info).request.data,"Account created");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
    }

    pthread_exit(NULL);
}

void *account_deletion(void* args){
    /* Request informations */
    struct request_processing *parent_info = args;
    int separator_pos;//Index of the separator
    char username[MAX_USER_USERNAME_LENGTH], password[MAX_USER_PASSWORD_LENGTH];//Username and password got from request

    char data[REQUEST_DATA_MAX_LENGTH];
    strcpy(data,(*parent_info).request.data);//Put request data in data

    printf("[Account_deletion-thread] - Received data (length : %ld): %s\n", strlen(data), data); //Log
    
    /* parse data to username and password */
    //Get pos of separator
    for (separator_pos = 0; separator_pos < strlen(data) && data[separator_pos] != USER_PASSWORD_REQUEST_SEPARATOR; separator_pos++);

    //Check string param length
    if (separator_pos >= MAX_USER_USERNAME_LENGTH || strlen(data)-separator_pos > MAX_USER_PASSWORD_LENGTH){
        (*parent_info).request.type = -1; //There is an error
        strcpy((*parent_info).request.data,"Username or password are too long");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client));
        pthread_exit(NULL);
    }else if (separator_pos == 0 || separator_pos == strlen(data) || separator_pos == strlen(data)-1){
        (*parent_info).request.type = -1; //There is an error
        strcpy((*parent_info).request.data,"Username or password are empty");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client));
        pthread_exit(NULL);
    }

    /* Copy username and password from data */
    strncpy(username,data,separator_pos);
    username[separator_pos]='\0';
    strncpy(password,&data[separator_pos]+1,strlen(data)-separator_pos);
    
    if(findNickname(username,password,ACCOUNT_FILE,1) != 1){
        (*parent_info).request.type = -1; 
        strcpy((*parent_info).request.data,"Wrong username/password");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
    }else{
        /* Sending response */
        if(delete(username,ACCOUNT_FILE) != 1){
            (*parent_info).request.type = -1; 
            strcpy((*parent_info).request.data,"Something went wrong");
            sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
        }else{
            (*parent_info).request.type = 0; 
            strcpy((*parent_info).request.data,"Account deleted");
            sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
        }
    }
    
    pthread_exit(NULL);
}

void *connected_users(void* args){
    /* Request informations */
    struct request_processing *parent_info = args;
    
    char connected_list[REQUEST_DATA_MAX_LENGTH]; //String of all connected usernames
    strcpy(connected_list,"");
    int bool_empty_list = 1; //If there is at least one user connected

    printf("[Connected_users-thread] - Received data (length : %ld): %s\n", strlen((*parent_info).request.data), (*parent_info).request.data); //Log

    /* Size Verification */
    if(MAX_USERS_CONNECTED*MAX_USER_USERNAME_LENGTH > REQUEST_DATA_MAX_LENGTH){ //Size verification
        //The request may not contains all usernames
        (*parent_info).request.type = -1; 
        strcpy((*parent_info).request.data,"Request data is too short (modify it in server side)");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
    }

    /* Build response*/
    for (size_t i = 0; i < MAX_USERS_CONNECTED; i++)
    {
        if (strcmp((*parent_info).shared_memory[i].username,"") != 0){
            bool_empty_list = 0; //Detected one user
            strcat(connected_list,(*parent_info).shared_memory[i].username);
            connected_list[strlen(connected_list)+1] = '\0'; //Adding end string char
            connected_list[strlen(connected_list)] = USER_PASSWORD_FILE_SEPARATOR; //Adding separator (same as username/password)
        }
    }
    connected_list[strlen(connected_list)-1] = '\0'; //replacing last separator by end-string character
    
    /* Send response */
    if (bool_empty_list){ //Empty list
        (*parent_info).request.type = 0; 
        strcpy((*parent_info).request.data,"");
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
    }else{
        (*parent_info).request.type = 0; 
        strcpy((*parent_info).request.data,connected_list);
        sendto ((*parent_info).sock, (void *) &(*parent_info).request, sizeof(struct request), 0, (struct sockaddr *) &(*parent_info).adr_client, sizeof((*parent_info).adr_client)); 
    }

    pthread_exit(NULL);
}