#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) 
{
    if (argc != 3) // Verifica si se proporcionan dos argumentos en la línea de comandos
    {
        fprintf(stderr, "Esquema: %s <numero1> <numero2>\n", argv[0]); // Imprime un mensaje de error en el flujo de error estándar
        fprintf(stderr, "Por favor, ingrese dos números enteros como argumentos.\n");
        return 1; // Devuelve un código de error
    }

    int num1 = atoi(argv[1]); // Convierte el primer argumento a un entero
    int num2 = atoi(argv[2]); // Convierte el segundo argumento a un entero

    // Verifica si los números están en el rango válido
    if (num1 < 0 || num1 > 100 || num2 < 0 || num2 > 100)
    {
        fprintf(stderr, "Error: Los números deben estar entre 0 y 100.\n"); // Imprime un mensaje de error en el flujo de error estándar
        return 1;                                                           // Devuelve un código de error
    }

    int sum = num1 + num2;                                                                                  // Calcula la suma de los números
    printf("Padre (pid = %d), El resultado de la operación de %d + %d es %d\n", getpid(), num1, num2, sum); // Imprime el resultado

    // Crea un número de procesos igual a la suma de los números
    for (int i = 0; i < sum; i++)
    {
        pid_t pid = fork(); // Crea un nuevo proceso
        if (pid < 0)
        {
            perror("Error de Fork_1"); // Imprime un mensaje de error
            return 1;                  // Devuelve un código de error
        }
        else if (pid == 0)
        {
            // Proceso hijo
            printf("Niño (pid - %d), EL padre es ppid = %d, El niño es: %d\n", getpid(), getppid(), i + 1); // Imprime información del proceso hijo
            // Crea procesos adicionales dentro del proceso hijo
            for (int j = 0; j <= i; j++)
            {
                pid_t child_pid = fork(); // Crea un nuevo proceso dentro del proceso hijo
                if (child_pid < 0)
                {
                    perror("Error de Fork_2"); // Imprime un mensaje de error
                    return 1;                  // Devuelve un código de error
                }
                else if (child_pid == 0)
                {
                    // Proceso nieto
                    printf("Niño (pid - %d), EL padre es ppid = %d, el niño es: %d\n", getpid(), getppid(), j + 1); // Imprime información del proceso nieto
                    return 0;                                                                                       // Termina el proceso nieto
                }
            }
            return 0; // Termina el proceso hijo
        }
        else
        {
            waitpid(pid, NULL, 0); // El padre espera a que el proceso hijo termine
        }
    }

    return 0; // Termina el programa con éxito
}
