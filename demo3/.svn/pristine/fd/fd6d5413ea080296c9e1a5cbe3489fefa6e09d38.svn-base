#include "control.h"

void handle_input(Client* client, char* line, FILE* stream);
void list_visit(Control* control, FILE* stream);
void add_visit(Control* control, char* line);
void visit_sort(Control* control);

/**
 * Handles the inputted arguments to create the control.
 * @param argc The number of arguments.
 * @param argv The string values for the arguments.
 * @return The return values for handling the arguments.
 */
int main(int argc, char** argv) {
    bool sendPort = false;
    Control* control = malloc(sizeof(Control));
    Client* client = malloc(sizeof(Client));
    client->handleInput = handle_input;
    control->visit = malloc(sizeof(char*));

    if (argc > 4 || argc < 3) {
        fprintf(stderr, "Usage: control2310 id info [mapper]\n");
        return 1;
    }

    if (!valid_input(argv[1]) || !valid_input(argv[2])) {
        fprintf(stderr, "Invalid char in parameter\n");
        return 2;
    }

    control->id = calloc(strlen(argv[1]), sizeof(char));
    control->info = calloc(strlen(argv[2]), sizeof(char));
    strcpy(control->id, argv[1]);
    strcpy(control->info, argv[2]);
    client->control = control;

    if (argc == 4) {
        int mapper = atoi(argv[3]);

        if (!valid_port(mapper)) {
            fprintf(stderr, "Invalid port\n");
            return 3;
        } else {
            client->control->port = mapper;
            sendPort = true;
        }
    }
    return start_server(client, sendPort);
}

/**
 * Handles the incoming connections and sending information.
 * @param client The client information.
 * @param line The input to handle.
 * @param stream The stream to send the return information.
 */
void handle_input(Client* client, char* line, FILE* stream) {
    line[strlen(line) - 1] = '\0';
    if (strcmp(line, "log") == 0) {
        list_visit(client->control, stream);
    } else {
        sem_wait(client->guard);
        add_visit(client->control, line);
        sem_post(client->guard);
        fprintf(stream, "%s\n", client->control->info);
        fflush(stream);
    }
    return;
}

/**
 * Outputs the list of processes which have visited
 * this server.
 * @param control The control information.
 * @param stream The stream to send the information.
 */
void list_visit(Control* control, FILE* stream) {
    char buff[BUFF_SIZE];
    if (control->numVisit > 0) {
        for (int i = 0; i < control->numVisit; ++i) {
            sprintf(buff, "%s\n", control->visit[i]);
            fputs(buff, stream);
        }
    }
    fputs(".\n", stream);
    fflush(stream);
}

/**
 * Adds a value to the control data.
 * @param control The control data structure.
 * @param line The line to add to the data structure.
 */
void add_visit(Control* control, char* line) {
    control->visit = realloc(control->visit,
            (control->numVisit + 1) * sizeof(char*));
    control->visit[control->numVisit] = calloc(strlen(line), sizeof(char));
    strcpy(control->visit[control->numVisit], line);
    control->numVisit++;

    visit_sort(control);
}

/**
 * Sorts the list of visitors in lexicographical order.
 * @param control THe data structure to sort.
 */
void visit_sort(Control* control) {
    for (int i = 0; i < control->numVisit; ++i) {
        for (int j = i + 1; j < control->numVisit; ++j) {
            if (strcmp(control->visit[i], control->visit[j]) > 0) {
                char* temp = control->visit[i];
                control->visit[i] = control->visit[j];
                control->visit[j] = temp;
            }
        }
    }
}

