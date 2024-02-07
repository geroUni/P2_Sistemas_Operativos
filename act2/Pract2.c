#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // Check if the number of parameters is correct
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse the input number
    int input_number = atoi(argv[1]);

    // Check if the input number is within the range [0, 100]
    if (input_number < 0 || input_number > 100) {
        fprintf(stderr, "Error: Input number must be between 0 and 100.\n");
        exit(EXIT_FAILURE);
    }

    // Convert the input number to uint8_t
    uint8_t number = (uint8_t) input_number;

    // Pipes
    int p1[2], p2[2];
    
    // Create pipes
    if (pipe(p1) == -1 || pipe(p2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        // Close unused pipe ends
        close(p1[1]);
        close(p2[0]);

        while (1) {
            uint8_t value;
            // Read from p1
            if (read(p1[0], &value, sizeof(uint8_t)) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (value == 0) {
                break;
            }
            value--;
            // Write to p2
            if (write(p2[1], &value, sizeof(uint8_t)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        // Close pipe ends
        close(p1[0]);
        close(p2[1]);
        exit(EXIT_SUCCESS);
    } else { // Parent process
        // Close unused pipe ends
        close(p1[0]);
        close(p2[1]);

        while (1) {
            // Write number to p1
            if (write(p1[1], &number, sizeof(uint8_t)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            // Read from p2
            if (read(p2[0], &number, sizeof(uint8_t)) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (number == 0) {
                break;
            }
            printf("%d\n", number);
        }

        // Close pipe ends
        close(p1[1]);
        close(p2[0]);

        // Wait for child process to finish
        waitpid(pid, NULL, 0);
    }

    return 0;
}
