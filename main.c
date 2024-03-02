#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define MAX_WORDS 100
#define MAX_WORD_LENGTH 20
#define MAX_PLAYERS 100
#define MAX_NAME_LENGTH 20
#define BORDER_CHAR '-'
#define BORDER_WIDTH 60

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


void printTitle() {
    printf("     _____                         _      _       \n");
    printf("    /  __ \\                       | |    | |      \n");
    printf(" ___| /  \\/ _ __  __ _  _ __ ___  | |__  | |  ___ \n");
    printf("/ __|| |    | '__|/ _` || '_ ` _ \\ | '_ \\ | | / _ \\\n");
    printf("\\__ \\| \\__/\\| |  | (_| || | | | | || |_) || ||  __/\n");
    printf("|___/ \\____/|_|   \\__,_||_| |_| |_||_.__/ |_| \\___|\n");
    printf("                                                  \n");
    printf("                                                  \n");
    printf("\n");
}


int printLeaderBoard() {
    printf(" _        _______  _______  ______   _______  _______    ______   _______  _______  _______  ______  \n");
    printf("( \\      (  ____ \\(  ___  )(  __  \\ (  ____ \\(  ____ )  (  ___ \\ (  ___  )(  ___  )(  ____ )(  __  \\ \n");
    printf("| (      | (    \\/| (   ) || (  \\  )| (    \\/| (    )|  | (   ) )| (   ) || (   ) || (    )|| (  \\  )\n");
    printf("| |      | (__    | (___) || |   ) || (__    | (____)|  | (__/ / | |   | || (___) || (____)|| |   ) |\n");
    printf("| |      |  __)   |  ___  || |   | ||  __)   |     __)  |  __ (  | |   | ||  ___  ||     __)| |   | |\n");
    printf("| |      | (      | (   ) || |   ) || (      | (\\ (     | (  \\ \\ | |   | || (   ) || (\\ (   | |   ) |\n");
    printf("| (____/\\| (____/\\| )   ( || (__/  )| (____/\\| ) \\ \\__  | )___) )| (___) || )   ( || ) \\ \\__| (__/  )\n");
    printf("(_______/(_______/|/     \\|(______/ (_______/|/   \\__/  |/ \\___/ (_______)|/     \\||/   \\__/(______/ \n");
    printf("                                                                                                     \n");

}



void holdScreen() {
    printf("Press Enter to continue...");
    getchar();
    getchar();
    clearScreen();
}

struct Player {
    int index;
    char name[20];
    int score;
    int gamesPlayed;
};

int readWordsFromFile(const char *filename, char words[MAX_WORDS][MAX_WORD_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    int count = 0;
    while (fscanf(file, "%s", words[count]) != EOF && count < MAX_WORDS) {
        count++;
    }

    fclose(file);
    return count;
}

void scrambleWord(char *word) {
    int length = strlen(word);
    for (int i = 0; i < length - 1; i++) {
        int j = i + rand() % (length - i);
        char temp = word[i];
        word[i] = word[j];
        word[j] = temp;
    }
}

int isInWordList(const char *word, char words[MAX_WORDS][MAX_WORD_LENGTH], int numWords) {
    for (int i = 0; i < numWords; i++) {
        if (strcmp(word, words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int readPlayersFromFile(const char *filename, struct Player players[MAX_PLAYERS], int *numPlayers) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    int count = 0;
    while (fscanf(file, "%d %s %d %d", &players[count].index, players[count].name, &players[count].score, &players[count].gamesPlayed) != EOF && count < MAX_PLAYERS) {
        count++;
    }

    fclose(file);
    *numPlayers = count;
    return 1;
}

int writePlayersToFile(const char *filename, struct Player players[MAX_PLAYERS], int numPlayers) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    for (int i = 0; i < numPlayers; i++) {
        fprintf(file, "%d %s %d %d\n", players[i].index, players[i].name, players[i].score, players[i].gamesPlayed);
    }

    fclose(file);
    return 1;
}

int findPlayerByName(const char *name, struct Player players[MAX_PLAYERS], int numPlayers) {
    for (int i = 0; i < numPlayers; i++) {
        if (strcmp(players[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int comparePlayers(const void *a, const void *b) {
    const struct Player *playerA = (const struct Player *)a;
    const struct Player *playerB = (const struct Player *)b;
    return (playerB->score - playerA->score);
}

void displayLeaderboard(struct Player players[MAX_PLAYERS], int numPlayers) {
    clearScreen();
    printLeaderBoard();    
    qsort(players, numPlayers, sizeof(struct Player), comparePlayers);

    printf("+%-*.*s+%*.*s+\n", MAX_NAME_LENGTH, MAX_NAME_LENGTH, "------------------------", 10, 10, "----------");
    printf("| %-20s | %-10s |\n", "Name", "Score");
    printf("+%-*.*s+%*.*s+\n", MAX_NAME_LENGTH, MAX_NAME_LENGTH, "------------------------", 10, 10, "----------");

    for (int i = 0; i < numPlayers; i++) {
        printf("| %-20s | %-10d |\n", players[i].name, players[i].score);
    }

    printf("+%-*.*s+%*.*s+\n", MAX_NAME_LENGTH, MAX_NAME_LENGTH, "------------------------", 10, 10, "----------");
    holdScreen();
}

int main() {
    char words[MAX_WORDS][MAX_WORD_LENGTH];
    struct Player players[MAX_PLAYERS];
    int numWords, numPlayers;

    srand(time(NULL));

    if (!readPlayersFromFile("data/players.txt", players, &numPlayers)) {
        printf("Error reading player data.\n");
        return 1;
    }

    int choice;
    do {
        printTitle();
        printf("1. Play\n");
        printf("2. Leaderboard\n");
        printf("3. Delete Player\n");
        printf("4. Exit\n\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                clearScreen();
                printf("\nSelect Mode:\n");
                printf("1. Countries\n");
                printf("2. Foods\n");
                printf("3. Science\n");
                printf("Enter your choice: ");
                int modeChoice;
                scanf("%d", &modeChoice);
                const char *filename;
                char filepath[50];
                switch (modeChoice) {
                    case 1:
                        strcpy(filepath, "data/countries.txt");
                        filename = filepath;
                        break;
                    case 2:
                        strcpy(filepath, "data/foods.txt");
                        filename = filepath;
                        break;
                    case 3:
                        strcpy(filepath, "data/sciences.txt");
                        filename = filepath;
                        break;
                    default:
                        printf("Invalid mode choice.\n");
                        continue;
                }

    numWords = readWordsFromFile(filename, words);
    if (numWords == 0) {
        printf("No words found in file.\n");
        return 1;
    }

    char playerName[20];
    printf("Enter your username: ");
    scanf("%s", playerName);

    int playerIndex = findPlayerByName(playerName, players, numPlayers);
    if (playerIndex == -1) {
        sprintf(players[numPlayers].name, "%s", playerName);
        players[numPlayers].index = numPlayers + 1;
        players[numPlayers].score = 0;
        players[numPlayers].gamesPlayed = 1;
        numPlayers++;
    } else {
        players[playerIndex].gamesPlayed++;
    }

    int index = rand() % numWords;
    char selectedWord[MAX_WORD_LENGTH];
    strcpy(selectedWord, words[index]);

    scrambleWord(selectedWord);

    printf("Unscramble the following word: %s [%d]\n", selectedWord, strlen(selectedWord));

    char guess[MAX_WORD_LENGTH];
    int attempts = 3;
    int correct = 0;
    while (attempts > 0) {
        printf("Enter your guess (%d attempts remaining): ", attempts);
        scanf("%s", guess);
        if (isInWordList(guess, words, numWords)) {
            printf("Congratulations! You guessed the word correctly.\n");
            correct = 1;

            players[playerIndex].score++;
            holdScreen();
            break;
        } else {
            printf("Incorrect guess. Try again.\n");
            attempts--;
        }
    }
    if (!correct) {
        printf("Sorry, the correct word was: %s\n", words[index]);
        holdScreen();
    }

    if (!writePlayersToFile("data/players.txt", players, numPlayers)) {
        printf("Error writing player data.\n");
    }
    break;
}

            case 2: {
                displayLeaderboard(players, numPlayers);
                break;
            }
            case 3: {
                clearScreen();
                char playerNameToDelete[20];
                printf("Enter the name of the player you want to delete: ");
                scanf("%s", playerNameToDelete);

                int playerIndexToDelete = findPlayerByName(playerNameToDelete, players, numPlayers);
                if (playerIndexToDelete == -1) {
                    printf("Player '%s' not found.\n", playerNameToDelete);
                    holdScreen();
                } else {
                    for (int i = playerIndexToDelete; i < numPlayers - 1; i++) {
                        players[i] = players[i + 1];
                    }
                    numPlayers--;
                    printf("Player '%s' deleted successfully.\n", playerNameToDelete);
                    holdScreen();
                }
                break;
            }
            case 4: {
                printf("Exiting...\n");
                break;
            }
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 4);

    if (!writePlayersToFile("data/players.txt", players, numPlayers)) {
        printf("Error writing player data.\n");
    }

    return 0;
}
