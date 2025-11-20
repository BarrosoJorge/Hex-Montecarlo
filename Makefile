# --- Variables de Compilación ---
CC = gcc
# Flags: -Wall (Advertencias), -O2 (Optimización para Montecarlo), -g (Para debuggear si falla)
CFLAGS = -Wall -O2 -g 
LIBS = -lm

# --- Directorios de Cabeceras (.h) ---
INCLUDES = -I./Board -I./Player -I./Rules -I./utils -I./Processes

# --- Archivos Fuente (.c) ---
# Listamos explícitamente todos los archivos de tu proyecto actual
SRCS = main.c \
       Board/board.c \
       Board/square.c \
       Player/player.c \
       Rules/rules.c \
       utils/utils.c \
       utils/pcg_basic.c \
       Processes/process_manager.c

# --- Archivos Objeto (.o) ---
# Convierte la lista de .c en .o automáticamente
OBJS = $(SRCS:.c=.o)

# --- Nombre del Ejecutable ---
TARGET = hex_game

# --- Reglas ---

# Regla por defecto (al escribir solo 'make')
all: $(TARGET)

# Linkeo: Crea el ejecutable final uniendo los objetos
$(TARGET): $(OBJS)
	@echo "Enlazando $(TARGET)..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LIBS)
	@echo "Compilación exitosa Ejecuta con: ./$(TARGET) 7 4 1000"

# Compilación: Crea los .o a partir de los .c
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Limpieza: Borra ejecutables y objetos (útil para recompilar desde cero)
clean:
	@echo "Limpiando archivos compilados..."
	rm -f $(OBJS) $(TARGET)

# Regla 'run': Compila y ejecuta un ejemplo rápido
run: $(TARGET)
	./$(TARGET) 7 4 1000