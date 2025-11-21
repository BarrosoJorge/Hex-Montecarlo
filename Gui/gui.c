#include "gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> // Para filledPolygonColor
#include <math.h>
#include "../utils/utils.h" // Para to_xy

// Configuración Visual
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define HEX_RADIUS 25
#define HEX_OFFSET 10 // Espacio entre hexágonos

// Colores (Formato 0xRRGGBBAA)
#define COLOR_BG      0xFFFFFFFF // Blanco
#define COLOR_GRID    0xCCCCCCFF // Gris claro
#define COLOR_EMPTY   0xEEEEEEFF // Gris muy claro
#define COLOR_P1      0xAA0000FF // Rojo (Jugador 1)
#define COLOR_P2      0x0000AAFF // Azul (Jugador 2)
#define COLOR_HOVER   0x88FF88FF // Verde claro (Mouse encima)
#define COLOR_TEXT    0x000000FF // Negro

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static int hex_radius = HEX_RADIUS;

// --- Funciones Auxiliares Privadas ---

// Calcula el centro (pixel x, y) de una casilla lógica (grid_x, grid_y)
void get_hex_center(int grid_x, int grid_y, int* cx, int* cy) {
    // Matemáticas de Hexagonos (Offset coordinates)
    // Desplazamos cada fila a la derecha (grid_y * radius)
    int start_x = 50 + (grid_y * hex_radius); 
    int start_y = 50;

    // Ancho de un hexágono = sqrt(3) * radius
    // Alto = 2 * radius (pero se solapan, usaremos 1.5 * radius para altura vertical)
    float width = sqrt(3.0) * hex_radius;
    float height = 1.5 * hex_radius; // Distancia vertical entre centros

    *cx = start_x + (grid_x * width);
    *cy = start_y + (grid_y * height);
}

// Dibuja un solo hexágono
void draw_hex(int grid_x, int grid_y, Uint32 color) {
    int cx, cy;
    get_hex_center(grid_x, grid_y, &cx, &cy);

    // Coordenadas de los 6 vértices
    Sint16 vx[6], vy[6];
    for (int i = 0; i < 6; i++) {
        float angle_deg = 60 * i - 30; // -30 para que apunte hacia arriba (flat top)
        float angle_rad = angle_deg * (M_PI / 180.0);
        vx[i] = cx + (hex_radius - 2) * cos(angle_rad); // -2 para dejar un borde
        vy[i] = cy + (hex_radius - 2) * sin(angle_rad);
    }

    // Usamos SDL2_gfx para dibujar polígono relleno
    filledPolygonColor(renderer, vx, vy, 6, color);
    // Dibuja el borde (anti-aliased)
    aapolygonColor(renderer, vx, vy, 6, COLOR_TEXT);
}

// --- Funciones Públicas ---

void gui_init(int board_size_pixels, const char* title) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error SDL: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(title, 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              WINDOW_WIDTH, WINDOW_HEIGHT, 
                              SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Error Ventana: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void gui_close() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void gui_draw(const Board* board) {
    // 1. Limpiar pantalla
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    int mx, my;
    SDL_GetMouseState(&mx, &my);

    // 2. Dibujar Tablero
    for (int i = 0; i < board->size * board->size; i++) {
        int x, y;
        to_xy(board->size, i, &x, &y);

        Uint32 color = COLOR_EMPTY;
        if (board->squares[i].simbolo == 'X') color = COLOR_P1;
        else if (board->squares[i].simbolo == 'O') color = COLOR_P2;

        // Lógica simple de "Hover" (Mouse encima)
        // Si está vacío y el mouse está cerca del centro
        int cx, cy;
        get_hex_center(x, y, &cx, &cy);
        double dist = sqrt(pow(mx - cx, 2) + pow(my - cy, 2));
        
        if (board->squares[i].simbolo == '+' && dist < hex_radius * 0.8) {
            color = COLOR_HOVER;
        }

        draw_hex(x, y, color);
    }

    // 3. Mostrar
    SDL_RenderPresent(renderer);
}

void gui_set_status(const char* status) {
    char title[256];
    snprintf(title, sizeof(title), "Hex Game - %s", status);
    SDL_SetWindowTitle(window, title);
}

void gui_process_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            gui_close();
            exit(0);
        }
    }
}

int gui_get_human_move(const Board* board) {
    SDL_Event e;
    int move_index = -1;

    // Bucle local de interfaz (esperando click)
    while (1) {
        // Mientras no haya evento, dibujamos para mantener la UI viva
        gui_draw(board);
        
        // Esperamos evento
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                gui_close();
                exit(0);
            }
            
            // Si hacen clic izquierdo
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;

                // Buscar en qué hexágono hizo clic (Fuerza bruta es rápida aquí)
                // Revisamos distancia a todos los centros
                for (int i = 0; i < board->size * board->size; i++) {
                    int x, y;
                    to_xy(board->size, i, &x, &y);
                    int cx, cy;
                    get_hex_center(x, y, &cx, &cy);

                    double dist = sqrt(pow(mx - cx, 2) + pow(my - cy, 2));
                    
                    // Si el click está dentro del radio del hexágono y está vacío
                    if (dist < hex_radius * 0.9) { // 0.9 para margen de error
                         if (board->squares[i].simbolo == '+') {
                             return i; // ¡Clic válido!
                         }
                    }
                }
            }
        }
        // Pequeña pausa para no quemar CPU
        SDL_Delay(10);
    }
    return -1;
}