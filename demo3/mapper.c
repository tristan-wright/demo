#include "mapper.h"

void handle_input(Client* client, char* line, FILE* stream);
void add_map(Client* client, char* line);
void get_map(Map* dict, char* line, FILE* stream);
void list_map(Map* dict, FILE* stream);
bool valid_input(char* line);
bool valid_port_num(char* key);
bool in_map(Map* dict, char* key);
void sort_map(Map* dict);

/**
 * Main function that initialises all the used variables
 * and accepts the initial arguments.
 * @param argc The number of arguments.
 * @param argv The values of the arguments.
 * @return The returned status value.
 */
int main(int argc, char** argv) {
    Client* client = malloc(sizeof(Client));
    client->dict = malloc(sizeof(Map));
    client->dict->map = malloc(sizeof(Pair));
    client->dict->numMap = 0;
    client->handleInput = handle_input;

    int error = start_server(client, false);
    for (int i = 0; i < client->dict->numMap; ++i) {
        free(&client->dict->map[i]);
    }
    free(client->dict->map);
    free(client->dict);
    free(client);

    return error;
}

/**
 * A function that will be used in handling the received text
 * input through tcp connections. Will determine the actions required
 * from the received text.
 * @param client The client data structure holding the mappers data.
 * @param line The input text received.
 * @param stream The output stream to send the determined response.
 */
void handle_input(Client* client, char* line, FILE* stream) {
    line[strlen(line) - 1] = '\0';
    if (line[0] == '@') {
        list_map(client->dict, stream);
    } else if (line[0] == '!') {
        line = &line[1];
        add_map(client, line);
    } else if (line[0] == '?') {
        line = &line[1];
        get_map(client->dict, line, stream);
    }
}

/**
 * Adds an inputted line to the map stored in the mapper data
 * structure.
 * @param client The client data structure.
 * @param line The mapping that needs to be processed and added
 * to the mapping.
 */
void add_map(Client* client, char* line) {
    char* key;
    char* val;
    Map* dict = client->dict;

    if (strlen(line) < 3 || strstr(line, ":") == NULL) {
        return;
    }

    key = strtok(line, ":");
    val = strtok(NULL, ":");

    if (!valid_input(key) || !valid_input(val)
            || in_map(dict, key) || !valid_port_num(val)) {
        return;
    }

    Pair* pair = malloc(sizeof(Pair));
    pair->key = calloc(strlen(key) + 1, sizeof(char));
    strcpy(pair->key, key);

    pair->val = calloc(strlen(val) + 1, sizeof(char));
    strcpy(pair->val, val);

    dict->map = realloc(dict->map, sizeof(Pair) * (dict->numMap + 1));
    sem_wait(client->guard);
    dict->map[dict->numMap] = *pair;
    dict->numMap += 1;

    sort_map(dict);
    sem_post(client->guard);
}

/**
 * Searches the mappings to find a key and value pair stored.
 * @param dict The mapping to search for the key.
 * @param line The key to search for in the mapping.
 * @param stream The output to return the mapping's value.
 */
void get_map(Map* dict, char* line, FILE* stream) {
    char buff[BUFF_SIZE];
    for (int i = 0; i < dict->numMap; ++i) {
        if (strcmp(dict->map[i].key, line) == 0) {
            sprintf(buff, "%s\n", dict->map[i].val);
            fputs(buff, stream);
            fflush(stream);
            return;
        }
    }
    sprintf(buff, ";\n");
    fputs(buff, stream);
    fflush(stream);
}

/**
 * Outputs all the key:value pairs in the mappings.
 * @param dict The mapping data.
 * @param stream The output stream to send the response.
 */
void list_map(Map* dict, FILE* stream) {
    char buff[BUFF_SIZE];
    for (int i = 0; i < dict->numMap; ++i) {
        sprintf(buff, "%s:%s\n", dict->map[i].key, dict->map[i].val);
        fputs(buff, stream);
        fflush(stream);
    }
    fflush(stream);
}

/**
 * Checks if a key is stored in the map.
 * @param dict The mapping to search.
 * @param key The key to search for in the mapping.
 * @return True if the key is stored in the mapping.
 */
bool in_map(Map* dict, char* key) {
    for (int i = 0; i < dict->numMap; ++i) {
        if (strcmp(dict->map[i].key, key) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * Check's if an inputted number is valid.
 * @param key The value in the key to check for.
 * @return True if all the values in the string are numbers.
 */
bool valid_port_num(char* key) {
    for (int i = 0; i < strlen(key); ++i) {
        if (!isdigit(key[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Sorts a mapping in lexicographical order.
 * @param dict The mapping to search.
 */
void sort_map(Map* dict) {
    for (int i = 0; i < dict->numMap; ++i) {
        for (int j = i + 1; j < dict->numMap; ++j) {
            Pair* first = &dict->map[i];
            Pair* second = &dict->map[j];
            Pair temp;
            if (strcmp(first->key, second->key) > 0) {
                temp = *first;
                dict->map[i] = *second;
                dict->map[j] = temp;
            }
        }
    }
}