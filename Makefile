# --- Variables de Compilación ---
CC = gcc
# --- Variables de Compilación ---
# Agregamos `sdl2-config --cflags` para los includes de SDL
CFLAGS = -Wall -O2 -g $(shell sdl2-config --cflags)

# Agregamos las librerías: -lSDL2 y -lSDL2_gfx
# `sdl2-config --libs` pone automáticamente las rutas correctas
LIBS = -lm $(shell sdl2-config --libs) -lSDL2_gfx

# --- Directorios de Cabeceras ---
# Agregamos la carpeta Gui
INCLUDES = -I./Board -I./Player -I./Rules -I./utils -I./Processes -I./Gui

# --- Archivos Fuente ---
# Agregamos Gui/gui.c
SRCS = main.c \
       Board/board.c \
       Board/square.c \
       Player/player.c \
       Rules/rules.c \
       utils/utils.c \
       utils/pcg_basic.c \
       Processes/process_manager.c \
       Gui/gui.c

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
#@echo "Compilación exitosa Ejecuta con: ./$(TARGET) 7 4 1000"

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