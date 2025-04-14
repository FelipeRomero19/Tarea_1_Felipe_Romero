# Definimos el compilador
CC = gcc

# Definimos el nombre del ejecutable
TARGET = tarea1

# Definimos los archivos fuente
SRCS = tarea1.c tdas/list.c tdas/extra.c

# Regla por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)