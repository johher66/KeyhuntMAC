#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define NUM_THREADS 8 // Adjust for M3 core count

// 256-bit integer representation using an array
typedef struct {
    uint64_t part[4]; // 4 x 64-bit parts = 256-bit number
} uint256_t;

// Private key range for puzzle #68
const uint256_t KEY_MIN = {{0x10, 0x00, 0x00, 0x00}};
const uint256_t KEY_MAX = {{0x1F, 0xFF, 0xFF, 0xFF}};
const uint64_t TOTAL_COMBINATIONS = 0x0FFFFFFFFFFFFFFFF; // Approximate count

uint64_t attempts = 0;
double start_time;
const char payout_address[] = "bc1qe05hk5djywfc7jplqgmwzfu7x608wpwkpjd54c";
const char target_wallet[] = "1MVDYgVaSN6iKKEsbzRUAYFrYJadLYZvvZ";
int puzzle_number = 68;

// Function to generate a random 256-bit key
uint256_t random_key() {
    uint256_t key;
    for (int i = 0; i < 4; i++) {
        key.part[i] = ((uint64_t)rand() << 32) | rand();
    }
    return key;
}

void* hunt_keys(void* arg) {
    while (1) {
        uint256_t key = random_key();
        __sync_fetch_and_add(&attempts, 1);
        
        if (attempts % 1000000 == 0) {
            double elapsed = (clock() - start_time) / CLOCKS_PER_SEC;
            double speed = attempts / elapsed / 1000; // Convert to kkeys/sec
            
            // Move cursor up 4 lines and overwrite output
            printf("\r\033[4A");  
            printf("* BTC Payout address: %s\n", payout_address);
            printf("* Solving Puzzle no.: %d\n", puzzle_number);
            printf("* BTC Wallet to solve: %s\n", target_wallet);
            printf("* Speed: %.2f kkeys/second\t* Average Threads: %d   \n", speed, NUM_THREADS);
            fflush(stdout);
        }
    }
    return NULL;
}

int main() {
    printf("Using hardcoded BTC payout address: %s\n", payout_address);
    printf("Solving puzzle number %d\n", puzzle_number);
    printf("Target BTC Wallet: %s\n", target_wallet);

    srand(time(NULL));
    start_time = clock();
    pthread_t threads[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, hunt_keys, (void*)i);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Keyhunt completed.\n");
    return 0;
}
