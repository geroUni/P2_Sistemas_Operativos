# Makefile para activity3.c

# Compilador
CC = gcc

# Opciones del compilador
CFLAGS = -Wall -Wextra -std=c99

# Nombre del ejecutable
TARGET = activity3

# Archivos fuente
SRCS = activity3.c

all: $(TARGET)
	@echo "¡Finalizado!"

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
	@echo "Limpieza completada"
