#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{

    // Verifica si el número de parámetros es correcto
    if (argc != 2)
    {
        fprintf(stderr, "Esquema: %s <número>\n", argv[0]);
        fprintf(stderr, "Por favor, ingrese un número entero como argumento\n");
        return 1; // Devuelve un código de error
    }

    // Analiza el número de entrada
    int num_entrada = atoi(argv[1]);

    // Verifica si el número de entrada está dentro del rango [0, 100]
    if (num_entrada < 0 || num_entrada > 100)
    {
        fprintf(stderr, "Error: El número debe estar entre 0 y 100.\n");
        return 1; 
    }

    // Convertir el número de entrada a uint8_t
    uint8_t numero = (uint8_t)num_entrada;

    // Pipes
    int p1[2], p2[2];

    // Crear pipes
    if (pipe(p1) == -1 || pipe(p2) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    { // Proceso hijo
        // Cerrar los extremos no utilizados de los pipes
        close(p1[1]);
        close(p2[0]);

        while (1)
        {
            uint8_t valor;
            // Leer desde p1
            if (read(p1[0], &valor, sizeof(uint8_t)) == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (valor == 0)
            {
                break;
            }
            valor--;
            // Escribir en p2
            if (write(p2[1], &valor, sizeof(uint8_t)) == -1)
            {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        // Cerrar los extremos de los pipes
        close(p1[0]);
        close(p2[1]);
        exit(EXIT_SUCCESS);
    }
    else
    { // Proceso padre
        // Cerrar los extremos no utilizados de los pipes
        close(p1[0]);
        close(p2[1]);

        while (1)
        {
            // Escribir el número en p1
            if (write(p1[1], &numero, sizeof(uint8_t)) == -1)
            {
                perror("write");
                exit(EXIT_FAILURE);
            }

            // Leer desde p2
            if (read(p2[0], &numero, sizeof(uint8_t)) == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (numero == 0)
            {
                break;
            }
            printf("%d\n", numero);
        }

        // Cerrar los extremos de los pipes
        close(p1[1]);
        close(p2[0]);

        // Esperar a que el proceso hijo termine
        waitpid(pid, NULL, 0);
    }

    return 0;
}
