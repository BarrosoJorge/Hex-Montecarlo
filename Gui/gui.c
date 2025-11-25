#include "gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include "../utils/utils.h"
#include <stdio.h>

// --- COLORES ---
#define COL_BG_MENU     0xFFFFFFFF 
#define COL_BG_GAME     0xF0F0F0FF 

#define COL_BTN_NORMAL  0x87CEFAFF 
#define COL_BTN_HOVER   0xB0E0E6FF 
#define COL_BTN_TEXT    0x00008BFF 
#define COL_MENU_TITLE  0x00008BFF 

#define COL_HEX_FILL    0xFFFFFFFF 
#define COL_HEX_BORDER  0xFF000000 

// JUGADORES
#define COL_P1          0xFF0000FF // Rojo (X) - Opaque
// Ponemos FF al principio para asegurar opacidad (Alpha)
#define COL_P2          0xFF8B0000
#define COL_HOVER_GAME  0xDDDDDDFF 

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static int win_w = 0, win_h = 0;
static float hex_rad = 30.0f; 

void gui_init(const char* title) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);
    
    // Creamos ventana centrada y redimensionable
    window = SDL_CreateWindow(title, 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              1024, 768, 
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                              
    // Renderer acelerado por hardware
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void gui_close() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void gui_set_status(const char* status) {
    SDL_SetWindowTitle(window, status);
}

void gui_process_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) exit(0);
    }
}

void recalculate_layout(int board_size) {
    SDL_GetWindowSize(window, &win_w, &win_h);
    
    // Margenes estimados
    float needed_w = board_size * 3.0f; 
    float needed_h = board_size * 2.0f; 
    
    // Escalar para ajustar
    float rw = (win_w - 250) / needed_w; 
    float rh = (win_h - 200) / needed_h;
    
    hex_rad = (rw < rh) ? rw : rh;
    
    // Limites 
    if (hex_rad > 60) hex_rad = 60;
    if (hex_rad < 15) hex_rad = 15;
}

void get_hex_center(int grid_x, int grid_y, int board_size, int* cx, int* cy) {
    float width = sqrt(3.0f) * hex_rad;
    float height = 1.5f * hex_rad;
    
    // Centrar todo el tablero en la pantalla
    float total_w = width * board_size + (height * (board_size/2.0f));
    float total_h = height * board_size;
    
    int off_x = (win_w - (int)total_w) / 2;
    int off_y = (win_h - (int)total_h) / 2 + 20; 
    
    // Desplazamiento de filas (Skew) típico de Hex
    float row_shift = grid_y * (width / 2.0f);
    
    *cx = off_x + (int)(row_shift + grid_x * width);
    *cy = off_y + (int)(grid_y * height);
}

void draw_button(int x, int y, int w, int h, const char* text, int mx, int my, Uint32 bg_col, Uint32 text_col) {
    int hover = (mx >= x && mx <= x+w && my >= y && my <= y+h);
    
    // Sombra
    boxColor(renderer, x+4, y+4, x+w+4, y+h+4, 0x00000033);
    // Relleno
    boxColor(renderer, x, y, x+w, y+h, hover ? COL_BTN_HOVER : bg_col);
    // Borde
    rectangleColor(renderer, x, y, x+w, y+h, text_col); 
    
    // Texto centrado
    int text_w = strlen(text) * 8; // Fuente por defecto es ~8px ancho
    stringRGBA(renderer, x + (w - text_w)/2, y + (h/2) - 4, text, 0, 0, 100, 255);
}

// Menú Principal
int gui_main_menu() {
    SDL_Event e;
    while(1) {
        SDL_GetWindowSize(window, &win_w, &win_h);
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        stringRGBA(renderer, win_w/2 - 40, win_h/4, "HEX GAME", 0, 0, 139, 255);

        int bw = 220, bh = 50;
        int cx = win_w/2 - bw/2;
        int cy = win_h/2 - 60;

        draw_button(cx, cy, bw, bh, "JUGAR", mx, my, COL_BTN_NORMAL, COL_BTN_TEXT);
        draw_button(cx, cy + 70, bw, bh, "CONFIGURAR", mx, my, COL_BTN_NORMAL, COL_BTN_TEXT);
        draw_button(cx, cy + 140, bw, bh, "SALIR", mx, my, COL_BTN_NORMAL, COL_BTN_TEXT);

        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) return 0;
            if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                if (mx > cx && mx < cx+bw) {
                    if (my > cy && my < cy+bh) return 1;
                    if (my > cy+70 && my < cy+70+bh) return 2;
                    if (my > cy+140 && my < cy+140+bh) return 0;
                }
            }
        }
        SDL_Delay(16);
    }
}

// Pantalla de Configuración (Con Tamaño de Tablero)
void gui_config_screen(GameConfig* config) {
    SDL_Event e;
    int back = 0;
    while(!back) {
        SDL_GetWindowSize(window, &win_w, &win_h); // Actualizar tamaño ventana
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        int cx = win_w/2 - 180;
        int cy = win_h/4; 

        //  DIFICULTAD 
        stringRGBA(renderer, cx + 120, cy - 40, "DIFICULTAD (SIMULACIONES)", 0, 0, 139, 255);
        Uint32 c1 = (config->num_sims == 100) ? COL_BTN_HOVER : COL_BTN_NORMAL;
        Uint32 c2 = (config->num_sims == 1000) ? COL_BTN_HOVER : COL_BTN_NORMAL;
        Uint32 c3 = (config->num_sims == 10000) ? COL_BTN_HOVER : COL_BTN_NORMAL;

        draw_button(cx, cy, 100, 50, "FACIL", mx, my, c1, COL_BTN_TEXT);
        draw_button(cx + 130, cy, 100, 50, "MEDIO", mx, my, c2, COL_BTN_TEXT);
        draw_button(cx + 260, cy, 100, 50, "DIFICIL", mx, my, c3, COL_BTN_TEXT);

        //  TAMAÑO TABLERO 
        int cy2 = cy + 120; // Más abajo
        stringRGBA(renderer, cx + 140, cy2 - 40, "TAMANO DEL TABLERO", 0, 0, 139, 255);
        
        Uint32 s7 = (config->board_size == 7) ? COL_BTN_HOVER : COL_BTN_NORMAL;
        Uint32 s9 = (config->board_size == 9) ? COL_BTN_HOVER : COL_BTN_NORMAL;
        Uint32 s11 = (config->board_size == 11) ? COL_BTN_HOVER : COL_BTN_NORMAL;
        Uint32 s13 = (config->board_size == 13) ? COL_BTN_HOVER : COL_BTN_NORMAL;

        draw_button(cx, cy2, 80, 50, "7x7", mx, my, s7, COL_BTN_TEXT);
        draw_button(cx + 90, cy2, 80, 50, "9x9", mx, my, s9, COL_BTN_TEXT);
        draw_button(cx + 180, cy2, 80, 50, "11x11", mx, my, s11, COL_BTN_TEXT);
        draw_button(cx + 270, cy2, 80, 50, "13x13", mx, my, s13, COL_BTN_TEXT);

        // INFO ACTUAL
        char buf[128];
        sprintf(buf, "Sims: %ld  |  Tamano: %d", config->num_sims, config->board_size);
        stringRGBA(renderer, cx + 100, cy2 + 80, buf, 0, 0, 0, 255);

        // VOLVER
        draw_button(cx + 80, cy2 + 150, 200, 50, "VOLVER", mx, my, COL_BTN_NORMAL, COL_BTN_TEXT);

        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) exit(0);
            if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                // Dificultad
                if(my > cy && my < cy+50) {
                    if(mx > cx && mx < cx+100) config->num_sims = 100;
                    if(mx > cx+130 && mx < cx+230) config->num_sims = 1000;
                    if(mx > cx+260 && mx < cx+360) config->num_sims = 10000;
                }
                // Tamaño Tablero
                if(my > cy2 && my < cy2+50) {
                    if(mx > cx && mx < cx+80) config->board_size = 7;
                    if(mx > cx+90 && mx < cx+170) config->board_size = 9;
                    if(mx > cx+180 && mx < cx+260) config->board_size = 11;
                    if(mx > cx+270 && mx < cx+350) config->board_size = 13;
                }
                // Volver
                if(mx > cx+80 && mx < cx+280 && my > cy2+150 && my < cy2+200) back = 1;
            }
        }
        SDL_Delay(16);
    }
}

void gui_draw_board_internal(const Board* board) {
    recalculate_layout(board->size);
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);

    // Dibujar etiquetas de bordes
    int top_x, top_y, bot_x, bot_y, left_x, left_y, right_x, right_y;
    get_hex_center(board->size/2, -1, board->size, &top_x, &top_y);
    get_hex_center(board->size/2, board->size, board->size, &bot_x, &bot_y);
    get_hex_center(-1, board->size/2, board->size, &left_x, &left_y);
    get_hex_center(board->size, board->size/2, board->size, &right_x, &right_y);

    stringRGBA(renderer, top_x, top_y - 10, "RED", 255, 0, 0, 255);
    stringRGBA(renderer, bot_x, bot_y + 10, "RED", 255, 0, 0, 255);
    stringRGBA(renderer, left_x - 40, left_y, "BLUE", 0, 0, 255, 255);
    stringRGBA(renderer, right_x + 10, right_y, "BLUE", 0, 0, 255, 255);

    int mx, my;
    SDL_GetMouseState(&mx, &my);

    for (int i = 0; i < board->size * board->size; i++) {
        int x, y, cx, cy;
        to_xy(board->size, i, &x, &y);
        get_hex_center(x, y, board->size, &cx, &cy);

        Uint32 color = COL_HEX_FILL;
        char sym = board->squares[i].simbolo;


        if (sym == 'X') {
            color = COL_P1; // Rojo
        }
        else if (sym == 'O') {
            color = COL_P2; // Azul
        }
        else {
            // Check Hover
            double d = sqrt(pow(mx-cx, 2) + pow(my-cy, 2));
            if (d < hex_rad * 0.9) color = COL_HOVER_GAME;
        }

        Sint16 vx[6], vy[6];
        for(int k=0; k<6; k++) {
            float rad = (60*k - 30) * (M_PI/180.0f);
            vx[k] = cx + (int)((hex_rad-2) * cos(rad));
            vy[k] = cy + (int)((hex_rad-2) * sin(rad));
        }

        filledPolygonColor(renderer, vx, vy, 6, color);
        aapolygonColor(renderer, vx, vy, 6, COL_HEX_BORDER);
    }
}

void gui_draw(const Board* board) {
    gui_draw_board_internal(board);
    SDL_RenderPresent(renderer); // Presentamos aquí
}

int gui_get_human_move(const Board* board) {
    SDL_Event e;
    while(1) {
        gui_draw(board);
        while (SDL_PollEvent(&e)) { 
            if (e.type == SDL_QUIT) exit(0);
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;
                // Detectar colision con hexágonos
                for (int i=0; i < board->size*board->size; i++) {
                    int x, y, cx, cy;
                    to_xy(board->size, i, &x, &y);
                    get_hex_center(x, y, board->size, &cx, &cy);
                    double d = sqrt(pow(mx-cx, 2) + pow(my-cy, 2));
                    // 0.9 deja un margen para no clicar entre dos
                    if (d < hex_rad * 0.9) {
                        if(board->squares[i].simbolo == '+') return i;
                    }
                }
            }
        }
        SDL_Delay(16);
    }
    return -1;
}

int gui_game_over_menu(const Board* board, const char* winner) {
    SDL_Event e;
    
    // Posición inicial centrada
    int win_w, win_h;
    SDL_GetWindowSize(window, &win_w, &win_h);
    int menu_w = 340;
    int menu_h = 240;
    int menu_x = win_w/2 - menu_w/2;
    int menu_y = win_h/2 - menu_h/2;

    // Variables para lógica de arrastre
    int dragging = 0;
    int drag_offset_x = 0;
    int drag_offset_y = 0;

    while(1) {
        gui_draw_board_internal(board); 
        // Dibujar Dialogo Flotante
        boxColor(renderer, menu_x-4, menu_y-4, menu_x+menu_w+4, menu_y+menu_h+4, 0x00000033); // Sombra
        boxColor(renderer, menu_x, menu_y, menu_x+menu_w, menu_y+menu_h, 0xFFFFFFEE);       // Fondo
        rectangleColor(renderer, menu_x, menu_y, menu_x+menu_w, menu_y+menu_h, 0x000000FF); // Borde

        // Barra de titulo para arrastrar
        boxColor(renderer, menu_x, menu_y, menu_x+menu_w, menu_y+30, 0xDDDDDDFF); 
        rectangleColor(renderer, menu_x, menu_y, menu_x+menu_w, menu_y+30, 0x999999FF);
        stringRGBA(renderer, menu_x + 10, menu_y + 10, "Arrastrar para ver el tablero", 0, 0, 0, 150);

        char msg[64];
        sprintf(msg, "GANADOR: %s", winner);
        stringRGBA(renderer, menu_x + 100, menu_y + 50, msg, 0, 0, 0, 255);

        int mx, my;
        SDL_GetMouseState(&mx, &my);

        int btn_x = menu_x + 70;
        int btn1_y = menu_y + 90;
        int btn2_y = menu_y + 160;

        draw_button(btn_x, btn1_y, 200, 50, "JUGAR DE NUEVO", mx, my, COL_BTN_NORMAL, COL_BTN_TEXT);
        draw_button(btn_x, btn2_y, 200, 50, "IR AL MENU", mx, my, COL_BTN_NORMAL, COL_BTN_TEXT);


        SDL_RenderPresent(renderer);


        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) exit(0);


            if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                // Verificar botones
                if (mx > btn_x && mx < btn_x+200) {
                    if (my > btn1_y && my < btn1_y+50) return 1; // Reiniciar
                    if (my > btn2_y && my < btn2_y+50) return 0; // Menu
                }
                
                // Si no es botón, verificar si es dentro de la ventana para arrastrar
                if (mx > menu_x && mx < menu_x + menu_w && my > menu_y && my < menu_y + menu_h) {
                    dragging = 1;
                    drag_offset_x = mx - menu_x;
                    drag_offset_y = my - menu_y;
                }
            }


            if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                dragging = 0;
            }


            if(e.type == SDL_MOUSEMOTION) {
                if (dragging) {
                    menu_x = e.motion.x - drag_offset_x;
                    menu_y = e.motion.y - drag_offset_y;
                }
            }
        }
        SDL_Delay(16);
    }
}

