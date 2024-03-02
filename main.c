#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 100
#define MAX_WORD_LENGTH 20
#define MAX_PLAYERS 100
#define PLAYER_INFO_LENGTH 50

// Function to read words from file into an array
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

// Function to scramble a word
void scrambleWord(char *word) {
    int length = strlen(word);
    for (int i = 0; i < length - 1; i++) {
        int j = i + rand() % (length - i);
        char temp = word[i];
        word[i] = word[j];
        word[j] = temp;
    }
}

// Function to check if a word is in the list of words
int isInWordList(const char *word, char words[MAX_WORDS][MAX_WORD_LENGTH], int numWords) {
    for (int i = 0; i < numWords; i++) {
        if (strcmp(word, words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to read player data from file
int readPlayersFromFile(const char *filename, char players[MAX_PLAYERS][PLAYER_INFO_LENGTH], int *numPlayers) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    int count = 0;
    while (fgets(players[count], PLAYER_INFO_LENGTH, file) != NULL && count < MAX_PLAYERS) {
        count++;
    }

    fclose(file);
    *numPlayers = count;
    return 1;
}

// Function to write player data to file
int writePlayersToFile(const char *filename, char players[MAX_PLAYERS][PLAYER_INFO_LENGTH], int numPlayers) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    for (int i = 0; i < numPlayers; i++) {
        fprintf(file, "%s", players[i]);
    }

    fclose(file);
    return 1;
}

// Function to find player by name
int findPlayerByName(const char *name, char players[MAX_PLAYERS][PLAYER_INFO_LENGTH], int numPlayers) {
    for (int i = 0; i < numPlayers; i++) {
        char temp[20];
        strcpy(temp, players[i]);
        char *token = strtok(temp, " ");
        if (strcmp(token, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char words[MAX_WORDS][MAX_WORD_LENGTH];
    char players[MAX_PLAYERS][PLAYER_INFO_LENGTH];
    int numWords, numPlayers;

    // Seed random number generator
    srand(time(NULL));

    // Read players data from file
    if (!readPlayersFromFile("data/players.txt", players, &numPlayers)) {
        printf("Error reading player data.\n");
        return 1;
    }

    int choice;
    do {
        // Display main menu
        printf("\nScramble Game Menu\n");
        printf("1. Play\n");
        printf("2. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                // Display mode selection
                printf("\nSelect Mode:\n");
                printf("1. Countries\n");
                printf("2. Foods\n");
                printf("3. Science\n");
                printf("Enter your choice: ");
                int modeChoice;
                scanf("%d", &modeChoice);
                const char *filename;
                char filepath[50]; // Adjust the size accordingly
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
                        continue; // Go back to main menu
                }

                // Read words from selected file
                numWords = readWordsFromFile(filename, words);
                if (numWords == 0) {
                    printf("No words found in file.\n");
                    return 1;
                }

                // Ask for username
                char playerName[20];
                printf("Enter your username: ");
                scanf("%s", playerName);

                // Check if player exists
                int playerIndex = findPlayerByName(playerName, players, numPlayers);
                if (playerIndex == -1) {
                    // If player doesn't exist, create new player
                    sprintf(players[numPlayers], "%d %s 0 1 0\n", numPlayers + 1, playerName); // Add number of games played
                    numPlayers++;
                } else {
                    // If player exists, update shuffle value and player name
                    int score, shuffle, gamesPlayed;
                    sscanf(players[playerIndex], "%*d %*s %d %d %d", &score, &shuffle, &gamesPlayed);
                    shuffle++;
                    score++; // Increase score for playing again
                    sprintf(players[playerIndex], "%d %s %d %d %d\n", playerIndex + 1, playerName, score, shuffle, gamesPlayed); // Update player information
                }

                // Randomly select a word
                int index = rand() % numWords;
                char selectedWord[MAX_WORD_LENGTH];
                strcpy(selectedWord, words[index]);

                // Scramble the selected word
                scrambleWord(selectedWord);

                // Present the scrambled word to the player
                printf("Unscramble the following word: %s [%d]\n", selectedWord, strlen(selectedWord));

                // Get player's guess
                char guess[MAX_WORD_LENGTH];
                int attempts = 3;
                int correct = 0;
                while (attempts > 0) {
                    printf("Enter your guess (%d attempts remaining): ", attempts);
                    scanf("%s", guess);
                    if (isInWordList(guess, words, numWords)) {
                        printf("Congratulations! You guessed the word correctly.\n");
                        correct = 1;

                        // Update player's score and shuffle value
                        int score, shuffle, gamesPlayed;
                        sscanf(players[playerIndex], "%*d %*s %d %d %d", &score, &shuffle, &gamesPlayed);
                        score++;
                        shuffle += attempts; // Reward shuffle by number of remaining attempts
                        sprintf(players[playerIndex], "%d %s %d %d %d\n", playerIndex + 1, playerName, score, shuffle, gamesPlayed); // Reset number of games played

                        // Write updated player data to file after completing the game
                        if (!writePlayersToFile("data/players.txt", players, numPlayers)) {
                            printf("Error writing player data.\n");
                        }

                        break;
                    } else {
                        printf("Incorrect guess. Try again.\n");
                        attempts--;
                    }
                }
                if (!correct) {
                    printf("Sorry, the correct word was: %s\n", words[index]);
                }
                break;
            }
            case 2:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please enter 1 or 2.\n");
        }
    } while (choice != 2);

    return 0;
}

