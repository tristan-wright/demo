#include "main.h"

bool read_deck(Board* board, char* deck);
int game(Hub* hub);
int initialise(Hub* hub);

/**
 * Reads the deck file information into the Board struct.
 * @param board The board struct information.
 * @param deck The name of the deck file to read.
 * @return True if the deck has been successfully read
 *          into the Board struct.
 */
bool read_deck(Board* board, char* deck) {
    char* buff = calloc(256, sizeof(char));

    FILE* file = fopen(deck, "r");
    if (file == NULL) {
        return false;
    }
    fgets(buff, 255, file);
    board->numCards = atoi(&buff[0]);
    if (board->numCards < 4) {
        return false;
    }

    board->cards = calloc(board->numCards, sizeof(int));
    for (int i = 1; i < board->numCards; ++i) {
        if (isalpha(buff[i]) == 0) {
            return false;
        }
        board->cards[i - 1] = buff[i] - 64;
    }
    return true;
}

/**
 * The main game loop that handles the messages between players.
 * @param hub The Hub struct.
 * @return The exit code corresponding to errors encountered.
 */
int game(Hub* hub) {
    Board* board = hub->board;
    char wbuffer[255];

    for (int i = 0; i < board->numPlayers; ++i) {
        write(hub->fdsend[i][1], hub->path, strlen(hub->path) + 1);
    }

    for (int j = 0; j < board->numPlayers; ++j) {
        read(hub->fdreceive[j][0], wbuffer, 255);
        fprintf(stdout, "%s", wbuffer);
    }
    return 0;
}

/**
 * Initialises the Hub struct information used for operations.
 * @param hub The Hub struct.
 * @return The exit code corresponding to errors encountered.
 */
int initialise(Hub* hub) {
    hub->fdsend = calloc(hub->board->numPlayers, sizeof(int*));
    hub->fdreceive = calloc(hub->board->numPlayers, sizeof(int*));
    hub->send = calloc(hub->board->numPlayers, sizeof(FILE*));
    hub->receive = calloc(hub->board->numPlayers, sizeof(FILE*));

    for (int i = 0; i < hub->board->numPlayers; ++i) {
        hub->fdsend[i] = calloc(2, sizeof(int));
        hub->fdreceive[i] = calloc(2, sizeof(int));
        if (pipe(hub->fdsend[i]) == -1 || pipe(hub->fdreceive[i]) == -1) {
            fprintf(stderr, "Error starting process\n");
            return 4;
        }

        pid_t pid = fork();
        if (pid < 0) {
            exit(1);
        } else if (pid == 0) {
            close(hub->fdsend[i][1]);
            close(hub->fdreceive[i][0]);
            fclose(stderr);
            dup2(hub->fdsend[i][0], 0);
            dup2(hub->fdreceive[i][1], 1);

            char numPlayers = hub->board->numPlayers + '0';
            char pid = i + '0';
            char* args[] = {hub->players[i], &numPlayers, &pid, NULL};
            if(execvp(args[0], args) == -1) {
                exit(4);
            }
        } else {
            close(hub->fdsend[i][0]);
            close(hub->fdreceive[i][1]);
            hub->send[i] = fdopen(hub->fdsend[i][1], "w+");
            hub->receive[i] = fdopen(hub->fdreceive[i][0], "r+");
            game(hub);

            int ret;
            waitpid(pid, &ret, 0);

            if (WIFEXITED(ret)) {
                int code = WEXITSTATUS(ret);
                if (code == 4) {
                    fprintf(stderr, "Error starting process\n");
                    return 4;
                }
            }
        }
    }
    return 0;
}

/**
 * The main function that starts the dealer. Handles arguments given
 * to the program.
 * @param argc The number of arguments.
 * @param argv The locations of each argument.
 * @return The exit code encountered.
 */
int main(int argc, char** argv) {
    Hub* hub = malloc(sizeof(Hub));
    Board* board = malloc(sizeof(Board));
    hub->path = calloc(255, sizeof(char));

    if (argc < 4) {
        fprintf(stderr, "Usage: 2310dealer deck path p1 {p2}\n");
        return 1;
    }
    board->numPlayers = argc - 3;
    hub->players = calloc(board->numPlayers, sizeof(char*));

    if (!read_deck(board, argv[1])) {
        fprintf(stderr, "Error reading deck\n");
        return 2;
    }

    FILE* path = fopen(argv[2], "r");
    if (path == NULL || !read_path(board, path)) {
        fprintf(stderr, "Error reading path\n");
        return 3;
    }
    fseek(path, 0, SEEK_SET);
    fgets(hub->path, 255, path);

    for (int i = 3; i < board->numPlayers + 3; ++i) {
        hub->players[i - 3] = argv[i];
    }

    hub->board = board;
    return initialise(hub);
}