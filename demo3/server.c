#include "server.h"

int handle_server(Client* client, int serverSocket,
        struct sockaddr_in clientAddr);
void* handle_connection(void* data);
bool send_msg(unsigned int port, char* buffer);

/**
 * Creates a multi-threaded tcp server that will use
 * the "client->handleInput" to determine the handling
 * for each connection.
 * @param client The client data to handle connections.
 * @param sendPort Decision whether to communicate the server's
 * binded port.
 * @return The return value for the process.
 */
int start_server(Client* client, bool sendPort) {
    struct addrinfo* ai = 0;
    struct addrinfo sa;
    struct sockaddr_in myAddr, clientAddr;
    int serverSocket;

    memset(&sa, 0, sizeof(struct addrinfo));
    sa.ai_family = AF_INET;
    sa.ai_socktype = SOCK_STREAM;
    sa.ai_flags = AI_PASSIVE;

    if (getaddrinfo("127.0.0.1", 0, &sa, &ai)) {
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(serverSocket, (struct sockaddr*)ai->ai_addr,
            sizeof(struct sockaddr))) {
        return 3;
    }

    memset(&myAddr, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    if (getsockname(serverSocket, (struct sockaddr*)&myAddr, &len)) {
        return 4;
    }
    
    client->port = ntohs(myAddr.sin_port);
    fprintf(stdout, "%u\n", client->port);
    fflush(stdout);

    if (sendPort) {
        char buffer[BUFF_SIZE];
        sprintf(buffer, "!%s:%d\n", client->control->id, client->port);
        if (!send_msg(client->control->port, buffer)) {
            fprintf(stderr, "Can not connect to map\n");
            return 4;
        }
    }

    if (listen(serverSocket, REQUESTS_SIZE)) {
        return 4;
    }
    return handle_server(client, serverSocket, clientAddr);
}

/**
 * Uses the given connection structs to handle incoming
 * connections.
 * @param client The client data to handle connections.
 * @param serverSocket The serverSocket that it is bound to.
 * @param clientAddr The client connection information.
 * @return The exit status for handling the connections.
 */
int handle_server(Client* client, int serverSocket,
        struct sockaddr_in clientAddr) {
    int clientSocket;
    sem_t lock;
    pthread_t* tids = malloc(sizeof(pthread_t));
    socklen_t clientAddrSize = sizeof(struct sockaddr_in);

    sem_init(&lock, 0, 1);
    client->guard = &lock;

    int tcount = 0;
    while (1) {
        clientSocket = accept(serverSocket,
                (struct sockaddr*) &clientAddr, &clientAddrSize);

        if (clientSocket < 0) {
            return 5;
        } else {
            tcount++;
            client->socketId = clientSocket;
            tids = realloc(tids, tcount * sizeof(pthread_t));
            pthread_create(&tids[tcount - 1], 0, handle_connection, client);
        }
    }

    for (int i = 0; i < tcount; ++i) {
        void* v;
        pthread_join(tids[i], &v);
    }
    sem_destroy(client->guard);
    return 0;
}

/**
 * Handles the incoming connection by receiving and send
 * information. Takes the bound socket and creates the file streams.
 * @param data The data that is affected by the incoming stream.
 * @return A void pointer of NULL.
 */
void* handle_connection(void* data) {
    Client* client = (Client*) data;
    char line[BUFF_SIZE];

    FILE* stream = fdopen(client->socketId, "r");
    FILE* out = fdopen(client->socketId, "w");
    while (fgets(line, BUFF_SIZE, stream) != NULL) {
        client->handleInput(client, line, out);
    }

    fclose(out);
    fclose(stream);
    return NULL;
}

/**
 * Determines whether a string is a valid input.
 * @param line The string to check.
 * @return True if the string is valid and does not
 * contain illegal characters.
 */
bool valid_input(char* line) {
    if (line == NULL) {
        return false;
    }

    int len = strlen(line);
    if (len == 0) {
        return false;
    }

    for (int i = 0; i < len; ++i) {
        if (line[i] == '\n' || line[i] == '\r' || line[i] == ':') {
            return false;
        }
    }
    return true;
}

/**
 * Used to send a buffer message to a local port.
 * @param port The local port to send the TCP message.
 * @param buffer The loaded buffer which contains the message.
 * @return True if the message was successfully sent.
 */
bool send_msg(unsigned int port, char* buffer) {
    int sock;
    struct sockaddr_in sai;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        return false;
    }
    memset(&sai, 0, sizeof(struct sockaddr_in));
    sai.sin_family = AF_INET;
    sai.sin_port = htons(port);
    sai.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*) &sai, sizeof(sai)) < 0) {
        return false;
    }

    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        return false;
    }

    close(sock);
    return true;
}

/**
 * Determines whether a port number is valid. 0 < port < 65535.
 * @param port The port number to check.
 * @return True if the number is a valid port number.
 */
bool valid_port(int port) {
    return !(port <= 0 || port > 65535);
}