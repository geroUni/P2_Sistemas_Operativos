#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

#define PadreAHijo p1
#define HijoAPadre p2

int main(int argc, char *argv[])
{
    // Verifica si el número de parámetros es correcto
    if (argc != 2)
    {
        fprintf(stderr, "Esquema: %s <numero>\n", argv[0]);
        fprintf(stderr, "Error. Ingresa un unico parametro.\n");
        return 1;
    }

    char *endptr;
    int num = strtol(argv[1], &endptr, 10); // Convertir a entero

    // Verificar si el número es un entero
    if (*endptr != '\0' || endptr == argv[1])
    {
        printf("No es entero.\n");
        return 1;
    }

    // Verifica si el número está dentro del rango [0, 100]
    if (num < 0 || num > 100)
    {
        fprintf(stderr, "Error: El número debe estar entre 0 y 100.\n");
        return 1;
    }

    // Convertir el número pasado a uint8_t
    uint8_t numero = (uint8_t)num;

    // Define los Pipes
    int p1[2], p2[2];

    // Crear los pipes
    if (pipe(PadreAHijo) == -1 || pipe(HijoAPadre) == -1)
    {
        perror("Error: pipe");
        return 1;
    }

    // Fork
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error: fork");
        return 1;
    }

    if (pid == 0)
    { // Hijo
        close(PadreAHijo[1]);
        close(HijoAPadre[0]);

        while (1)
        {
            uint8_t valor;

            // Leer desde PadreAHijo
            if (read(PadreAHijo[0], &valor, sizeof(uint8_t)) == -1)
            {
                perror("Error: HijoAPadre write read");
                return 1;
            }

            printf("Hijo (pid=%d): %d\n", getpid(), valor);
            if (valor == 0)
            {
                break;
            }
            valor--;

            // Escribir en HijoAPadre
            if (write(HijoAPadre[1], &valor, sizeof(uint8_t)) == -1)
            {
                perror("Error: HijoAPadre write");
                return 1;
            }
        }

        close(PadreAHijo[0]);
        close(HijoAPadre[1]);
        return 0;
    }
    else
    { // Padre
        close(PadreAHijo[0]);
        close(HijoAPadre[1]);

        printf("Padre (pid=%d): Comenzare a contar desde %d hasta 0\n", getpid(), numero); // Imprimir el valor recibido por el padre

        while (numero > 0)
        {
            printf("Padre (pid=%d): %d\n", getpid(), numero); // Imprimir el valor recibido por el padre

            numero--;

            // Escribir en PadreAHijo
            if (write(PadreAHijo[1], &numero, sizeof(uint8_t)) == -1)
            {
                perror("Error: PadreAHijo write");
                return 1;
            }

            // Leer desde HijoAPadre
            if (read(HijoAPadre[0], &numero, sizeof(uint8_t)) == -1)
            {
                perror("Error: HijoAPadre read");
                return 1;
            }
        }

        // Cerrar pipes
        close(PadreAHijo[1]);
        close(HijoAPadre[0]);

        // Esperar a que hijo termine
        waitpid(pid, NULL, 0);
    }

    return 0;
}
