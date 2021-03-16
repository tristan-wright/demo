#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <arpa/inet.h>

#ifndef ASS4_SERVER_H
#define ASS4_SERVER_H

#define REQUESTS_SIZE 10
#define BUFF_SIZE 255

typedef void (*Handle)();

typedef struct PairStruct {
    char* key;
    char* val;
} Pair;

typedef struct MapStruct {
    int numMap;
    Pair* map;
} Map;

typedef struct ControlStruct {
    char* id;
    char* info;
    int numVisit;
    char** visit;
    unsigned int port;
} Control;

typedef struct RocStruct {
    char* id;
    unsigned int port;
    int numAirports;
    char** airports;
    int* ports;
} Roc;

typedef struct ClientStruct {
    unsigned int port;
    int socketId;
    Handle handleInput;
    Map* dict;
    Control* control;
    Roc* roc;
    sem_t* guard;
} Client;

int start_server(Client* client, bool sendPort);
bool valid_input(char* line);
bool valid_port(int port);

#endif //ASS4_SERVER_H
