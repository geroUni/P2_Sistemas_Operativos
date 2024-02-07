#include <stdio.h>     // Incluye la librería estándar de entrada y salida.
#include <stdlib.h>    // Incluye la librería estándar de funciones de utilidad.
#include <sys/ipc.h>   // Incluye la librería para el manejo de claves IPC.
#include <sys/shm.h>   // Incluye la librería para el manejo de memoria compartida.
#include <sys/types.h> // Incluye la librería para tipos de datos específicos del sistema.
#include <semaphore.h> // Incluye la librería para el manejo de semáforos.
#include <fcntl.h>     // Incluye la librería para el manejo de descriptores de archivos.
#include <unistd.h>    // Incluye la librería estándar de funciones de sistema Unix.

int main()
{

    key_t key_shm, key_sem; // Declaración de variables para las claves de memoria compartida y semáforos.
    int shmid;              // Variable para el identificador de la memoria compartida.
    void *shm_ptr;          // Puntero para acceder a la memoria compartida.
    sem_t *sem_punt;        // Puntero para el semáforo.

    // Generar una clave única para la memoria compartida.
    key_shm = ftok(".", 'S');
    // Obtener el identificador de la memoria compartida o crear una nueva.
    shmid = shmget(key_shm, sizeof(int), 0644 | IPC_CREAT);

    // Verificar si la operación de obtención o creación de la memoria compartida fue exitosa.
    if (shmid == -1)
    {
        perror("shmget"); // Imprime un mensaje de error si shmget falla.
        return 1;         // Termina el programa con un código de error.
    }

    // Adjuntar la memoria compartida al espacio de direcciones del proceso.
    shm_ptr = shmat(shmid, NULL, 0);
    // Verificar si la operación de adjuntar la memoria compartida fue exitosa.
    if (shm_ptr == (void *)-1)
    {
        perror("shmat"); // Imprime un mensaje de error si shmat falla.
        return 1;        // Termina el programa con un código de error.
    }

    // Crear un semáforo para sincronización
    // Generar una clave única para el semáforo.
    key_sem = ftok("/bin/ls", 1);
    // Abrir o crear un semáforo.
    sem_punt = sem_open(key_sem, O_CREAT, 0644, 1);
    // Verificar si la operación de abrir o crear el semáforo fue exitosa.
    if (sem_punt == SEM_FAILED)
    {
        perror("sem_open"); // Imprime un mensaje de error si sem_open falla.
        return 1;           // Termina el programa con un código de error.
    }

    return 0; // Indica que el programa terminó correctamente.
}
