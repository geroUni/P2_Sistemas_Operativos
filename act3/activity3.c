#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SEMF_NOMBRE "/mi_semaforo"   // Definición del nombre del semáforo
#define SEMF2_NOMBRE "/mi_semaforo2" // Definición del nombre del segundo semáforo

int main(int argc, char *argv[])
{
    key_t clave_shm;  // Clave para la memoria compartida
    int id_shm;       // Identificador de la memoria compartida
    void *ptr_shm;    // Puntero a la memoria compartida
    sem_t *sem_punt;  // Puntero del semáforo
    sem_t *sem_punt2; // Puntero del segundo semáforo
    pid_t pid;        // Identificador de proceso (PID)
    char *endptr; // Para Convertir cadenas a números

    sem_unlink(SEMF_NOMBRE);  // Elimina el semáforo si ya existe
    sem_unlink(SEMF2_NOMBRE); // Elimina el segundo semáforo si ya existe

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <número>\n", argv[0]);                          // Muestra el uso correcto del programa
        fprintf(stderr, "Por favor, ingrese un número entero como argumento\n"); // Indica al usuario qué debe ingresar
        return 1;
    }

    int num_entrada = strtol(argv[1], &endptr, 10); // Convertir a entero

    // Verificar si el número es un entero
    if (*endptr != '\0' || endptr == argv[1]) {
        printf("Error: El número proporcionado no es entero.\n");
        return 1;
    }

    if (num_entrada < 0 || num_entrada > 100)
    {
        fprintf(stderr, "Error: El número debe estar entre 0 y 100.\n"); // Muestra un mensaje de error si el número está fuera de rango
        return 1;
    }

    clave_shm = ftok(".", 'S');                                // Genera una clave única para la memoria compartida
    id_shm = shmget(clave_shm, sizeof(int), 0644 | IPC_CREAT); // Obtiene el identificador de la memoria compartida

    if (id_shm == -1)
    {
        perror("Error: shmget"); // Muestra un mensaje de error si falla la obtención de la memoria compartida
        return 1;
    }

    ptr_shm = shmat(id_shm, NULL, 0); // Añade el segmento de memoria compartida en el puntero

    if (ptr_shm == (void *)-1)
    {
        perror("Error: shmat"); // Muestra un mensaje de error si el puntero de la memoria compartida falla.
        return 1;
    }

    sem_punt = sem_open(SEMF_NOMBRE, O_CREAT | O_EXCL, 0644, 1);   // Abre o crea el semáforo
    sem_punt2 = sem_open(SEMF2_NOMBRE, O_CREAT | O_EXCL, 0644, 0); // Abre o crea el segundo semáforo

    if (sem_punt == SEM_FAILED)
    {
        perror("Error: sem_open"); // Muestra un mensaje de error si falla la apertura del semáforo
        return 1;
    }

    if (sem_punt2 == SEM_FAILED)
    {
        perror("Error: sem_open"); // Muestra un mensaje de error si falla la apertura del segundo semáforo
        return 1;
    }

    int *num_compartido = (int *)ptr_shm; // Puntero al entero compartido
    *num_compartido = num_entrada;        // Almacena el número en la memoria compartida

    pid = fork(); // Crea un proceso hijo

    //
    if (pid < 0)
    {
        perror("Error: Fork"); // Muestra un mensaje de error si falla la creación del proceso hijo
        return 1;
    }

    if (pid == 0) // Proceso hijo
    {
        while (1)
        {
            sem_wait(sem_punt2); // Espera a que el segundo semáforo esté disponible
            if (*num_compartido >= 0)
            {
                printf("Hijo (pid=%d): %d\n", getpid(), *num_compartido); // Imprime el número compartido si es mayor o igual a cero
                (*num_compartido)--;                                      // Decrementa el número compartido
            }
            else
            {
                sem_post(sem_punt); // Libera el semáforo principal
                break;
            }
            sem_post(sem_punt); // Libera el semáforo principal
            sleep(0.0000001);   // Duerme por un corto tiempo
        }
    }
    else // Proceso padre
    {
        printf("Padre (pid=%d): Comenzaré a contar desde %d hasta 0\n", getpid(), num_entrada); // Imprime un mensaje del padre
        while (1)
        {
            sem_wait(sem_punt); // Espera a que el semáforo principal esté disponible
            if (*num_compartido >= 0)
            {
                printf("Padre (pid=%d): %d\n", getpid(), *num_compartido); // Imprime el número compartido si es mayor o igual a cero
                (*num_compartido)--;                                       // Decrementa el número compartido
            }
            else
            {
                sem_post(sem_punt2); // Libera el segundo semáforo
                break;
            }
            sem_post(sem_punt2); // Libera el segundo semáforo
            sleep(0.0000001);    // Duerme por un corto tiempo
        }
        waitpid(pid, NULL, 0); // Espera a que el proceso hijo termine

        // Desconecta la memoria compartida
        if (shmdt(ptr_shm) == -1)
        {
            perror("Error: shmdt"); // Muestra un mensaje de error si falla la desconexión de la memoria compartida
            return 1;
        }

        // Elimina el segmento de memoria compartida
        if (shmctl(id_shm, IPC_RMID, NULL) == -1)
        {
            perror("Error: shmctl"); // Muestra un mensaje de error si falla la eliminación del segmento de memoria compartida
            return 1;
        }

        // Cerrar y desvincular los semáforos
        sem_close(sem_punt);      // Cierra el semáforo
        sem_unlink(SEMF_NOMBRE);  // Desvincula el semáforo
        sem_close(sem_punt2);     // Cierra el 2 semáforo
        sem_unlink(SEMF2_NOMBRE); // Desvincula el 2 semáforo
    }

    return 0;
}
