#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SEMF_NOMBRE "/my_semaphore"

int main(int argc, char *argv[]) {
    key_t key_shm; // Declaración de la clave para la memoria compartida
    int shmid; // Identificador de la memoria compartida
    void *shm_ptr; // Puntero a la memoria compartida
    sem_t *sem_punt; // Puntero al semáforo
    pid_t pid; // Identificador de proceso (PID)

    if (argc != 2) { // Verifica que el programa se haya llamado con un argumento
        fprintf(stderr, "Esquema: %s <número>\n", argv[0]); // Mensaje de uso correcto
        fprintf(stderr, "Por favor, ingrese un número entero como argumento\n"); // Mensaje de error
        return 1;
    }

    int num_entrada = atoi(argv[1]); // Convierte el argumento en un número entero

    if (num_entrada < 0 || num_entrada > 100) { // Verifica que el número esté en el rango especificado
        fprintf(stderr, "Error: El número debe estar entre 0 y 100.\n"); // Mensaje de error
        return 1;
    }

    key_shm = ftok(".", 'S'); // Genera la clave para la memoria compartida
    shmid = shmget(key_shm, sizeof(int), 0644 | IPC_CREAT); // Crea o abre el segmento de memoria compartida

    if (shmid == -1) { // Verifica si hubo un error al crear o abrir la memoria compartida
        perror("shmget"); // Muestra un mensaje de error detallado
        return 1;
    }

    shm_ptr = shmat(shmid, NULL, 0); // Adjunta la memoria compartida al espacio de direcciones del proceso

    if (shm_ptr == (void *)-1) { // Verifica si hubo un error al adjuntar la memoria compartida
        perror("shmat"); // Muestra un mensaje de error detallado
        return 1;
    }

    sem_punt = sem_open(SEMF_NOMBRE, O_CREAT, 0644, 1); // Abre o crea el semáforo

    if (sem_punt == SEM_FAILED) { // Verifica si hubo un error al abrir o crear el semáforo
        perror("sem_open"); // Muestra un mensaje de error detallado
        return 1;
    }

    pid = fork(); // Crea un proceso hijo

    if (pid < 0) { // Verifica si hubo un error al crear el proceso hijo
        perror("Error de Fork"); // Muestra un mensaje de error detallado
        return 1;
    }

    if (pid == 0) { // Proceso hijo
        sem_wait(sem_punt); // Espera a que el semáforo esté disponible
        sprintf((char *)shm_ptr, "%s", "¡Hola desde el hijo!"); // Escribe en la memoria compartida
        sem_post(sem_punt); // Libera el semáforo
    } else { // Proceso padre
        sem_wait(sem_punt); // Espera a que el semáforo esté disponible
        sprintf((char *)shm_ptr, "%s", "¡Hola desde el padre!"); // Escribe en la memoria compartida
        sem_post(sem_punt); // Libera el semáforo
        waitpid(pid, NULL, 0); // El padre espera a que el proceso hijo termine
    }

    // Desconectar de la memoria compartida
    if (shmdt(shm_ptr) == -1) { // Verifica si hubo un error al desconectar la memoria compartida
        perror("shmdt"); // Muestra un mensaje de error detallado
        return 1;
    }

    // Eliminar segmento de memoria compartida
    if (shmctl(shmid, IPC_RMID, NULL) == -1) { // Verifica si hubo un error al eliminar la memoria compartida
        perror("shmctl"); // Muestra un mensaje de error detallado
        return 1;
    }

    // Cerrar y desvincular semáforo
    sem_close(sem_punt); // Cierra el semáforo
    sem_unlink(SEMF_NOMBRE); // Elimina el semáforo

    return 0; // Termina el programa sin errores
}
