#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SEMF_NOMBRE "/mi_semaforo"

int main(int argc, char *argv[])
{
    key_t clave_shm; // Declaración de la clave para la memoria compartida
    int id_shm;      // Identificador de la memoria compartida
    void *ptr_shm;   // Puntero a la memoria compartida
    sem_t *sem_punt; // Puntero al semáforo
    pid_t pid;       // Identificador de proceso (PID)

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <número>\n", argv[0]);
        fprintf(stderr, "Por favor, ingrese un número entero como argumento\n");
        return 1;
    }

    int num_entrada = atoi(argv[1]);

    if (num_entrada < 0 || num_entrada > 100)
    {
        fprintf(stderr, "Error: El número debe estar entre 0 y 100.\n");
        return 1;
    }

    clave_shm = ftok(".", 'S');
    id_shm = shmget(clave_shm, sizeof(int), 0644 | IPC_CREAT);

    if (id_shm == -1)
    {
        perror("shmget");
        return 1;
    }

    ptr_shm = shmat(id_shm, NULL, 0);

    if (ptr_shm == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

    sem_punt = sem_open(SEMF_NOMBRE, O_CREAT | O_EXCL, 0644, 1);

    if (sem_punt == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    pid = fork();


    if (pid < 0)
    {
        perror("Fork falló");
        return 1;
    }


    printf("Padre (pid=%d): Comenzare a contar desde %d hasta 0\n", getpid(), num_entrada); // Imprime el valor recibido por el padre
    int *num_compartido = (int *)ptr_shm;

    if (pid == 0)
    {
        while (num_entrada > 0)
        {
            sem_wait(sem_punt);
            (*num_compartido)--;
            printf("Hijo recibió: %d\n", *num_compartido);
            sem_post(sem_punt);
            num_entrada--;
        }
    }
    else
    {
        while (num_entrada > 0)
        {
            sem_wait(sem_punt);
            *num_compartido = num_entrada;
            printf("Padre envió: %d\n", *num_compartido);
            sem_post(sem_punt);
            num_entrada--;
        }
        waitpid(pid, NULL, 0);
    }

    

    // Desconectar de la memoria compartida
    if (shmdt(ptr_shm) == -1)
    {
        perror("shmdt");
        return 1;
    }

    // Eliminar segmento de memoria compartida
    if (shmctl(id_shm, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        return 1;
    }

    // Cerrar y desvincular semáforo
    sem_close(sem_punt);
    sem_unlink(SEMF_NOMBRE);

    return 0;
}
