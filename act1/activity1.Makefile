# Makefile para activity1.c

# Compilador
CC = gcc

# Opciones del compilador
CFLAGS = -Wall -Wextra -std=c99

# Nombre del ejecutable
TARGET = activity1

# Archivos fuente
SRCS = activity1.c

all: $(TARGET)
	@echo "¡Finalizado!"

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
	@echo "Limpieza completada"
