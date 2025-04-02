#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>

const char NAME[] = "Snake";
const int WIDTH = 360, HEIGHT = 640;
const int SQUARE_SIZE = 20;
const int DEFAULT_POSITION = 0;
const int OBJECT_CENTER[2] = {WIDTH / 2 - 65, HEIGHT / 2 - 40};
const int GAME_LOOP_FRAME_RATE = 100;
const int DEFAULT_SNAKE_VELOCITY = 1;
const int SNAKE_MAX_LENGTH = WIDTH / SQUARE_SIZE * (HEIGHT / SQUARE_SIZE);
const int SNAKE_LENGTH_INCREASE = 4;
const int INITIAL_Y_POSITION = HEIGHT / SQUARE_SIZE / 2;
const int SNAKE_INITIAL_POSITION[2] = {SQUARE_SIZE * 4, SQUARE_SIZE * INITIAL_Y_POSITION};
const int APPLE_INITIAL_POSITION[2] = {SQUARE_SIZE * 14, SQUARE_SIZE * INITIAL_Y_POSITION};
const int SCORE_DISPLAY_POSITION[2] = {10, 10};
const bool SQUARE_FILLED = true;
const SDL_Color BACKGROUND_COLOR = {10, 10, 10, 255};
const SDL_Color colors[] = {
    {255, 255, 255, 255}, //white
    {255, 255, 0, 255}, //yellow
    {0, 255, 0, 255}, //green
    {255, 0, 0, 255}, //red
    {10, 10, 10, 255} //grey
};

//render_square keyword struct
typedef struct {
    int x;
    int y;
    int width;
    int height;
    SDL_Color color;
    bool filled;
    SDL_Renderer *renderer;
} RenderSquareParams;

//snake movement
typedef struct {
    enum Direction {UP, DOWN, LEFT, RIGHT} direction;
    int velocity;
} SnakeVector;

void render_square(SDL_Renderer *_renderer, RenderSquareParams overrides);
void render_grid(SDL_Renderer *renderer);
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char* text, int x, int y, SDL_Color color);
void render_pause_screen(SDL_Renderer *renderer, TTF_Font *font);
void render_game_over_screen(SDL_Renderer *renderer, TTF_Font *font);
int* generate_apple_position();
bool snake_collided_with_screen_edge(int x_position, int y_position);

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL));

    //initialize window
    SDL_Window *window = SDL_CreateWindow(NAME, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {return 1;}

    //initialize renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {return 1;}

    //initialize text & font
    if (TTF_Init() < 0) {return 1;}
    TTF_Font *font = TTF_OpenFont("src/font/PixelifySans.ttf", 28);
    if (font == NULL) {return 1;}

    //set up window loop
    bool running = true;
    SDL_Event window_event;

    //set up game loop
    SnakeVector snake = {RIGHT, DEFAULT_SNAKE_VELOCITY};
    int last_move_time = 0;
    bool is_paused = false;
    bool game_over = false;

    //set up snake head
    const int SNAKE_MOVEMENT = SQUARE_SIZE * snake.velocity;
    int snake_length = 1;
    int snake_x_position = SNAKE_INITIAL_POSITION[0];
    int snake_y_position = SNAKE_INITIAL_POSITION[1];

    //set up snake body
    SDL_Point snake_body[SNAKE_MAX_LENGTH];
    int pending_growth = 0;
    snake_body[0].x = snake_x_position;
    snake_body[0].y = snake_y_position;

    //set up apple
    int apple_x_position = APPLE_INITIAL_POSITION[0];
    int apple_y_position = APPLE_INITIAL_POSITION[1];

    //set up text
    char score_text[10];
    sprintf(score_text, "Score: %d", snake_length);

    while (running) {
        int current_time = SDL_GetTicks(); 

        //handle events
        if (SDL_PollEvent(&window_event)) {
            switch (window_event.type) {
                //user clicks X on top right of screen
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                //keyboard input
                case SDL_EVENT_KEY_DOWN:
                    switch (window_event.key.scancode) {
                        case SDL_SCANCODE_W:
                            if (snake.direction != DOWN && !is_paused) {
                                snake.direction = UP;
                            }

                            break;
                        case SDL_SCANCODE_S:
                            if (snake.direction != UP && !is_paused) {
                                snake.direction = DOWN;
                            }
                            
                            break;
                        case SDL_SCANCODE_A:
                            if (snake.direction != RIGHT && !is_paused) {
                                snake.direction = LEFT;
                            }

                            break;
                        case SDL_SCANCODE_D:
                            if (snake.direction != LEFT && !is_paused) {
                                snake.direction = RIGHT;
                            }
                            
                            break;
                        case SDL_SCANCODE_P:
                            is_paused = !is_paused;
                            break;
                    }

                    break;
            }
        }

        //game over'd
        if (game_over) {
            render_game_over_screen(renderer, font);
            SDL_RenderPresent(renderer);
            continue;
        }

        //game paused
        if (is_paused) {
            render_pause_screen(renderer, font);
            SDL_RenderPresent(renderer);
            continue;
        }

        //game loop
        if (current_time - last_move_time >= GAME_LOOP_FRAME_RATE) {
            last_move_time = current_time;

            //shift snake body segments
            for (int i = snake_length - 1; i > 0; i--) {
                snake_body[i] = snake_body[i - 1];
            }

            //check snake movement
            switch (snake.direction) {
                case UP: snake_y_position -= SNAKE_MOVEMENT; break;
                case DOWN: snake_y_position += SNAKE_MOVEMENT; break;
                case LEFT: snake_x_position -= SNAKE_MOVEMENT; break;
                case RIGHT: snake_x_position += SNAKE_MOVEMENT; break;
            }

            //store new head position
            snake_body[0] = (SDL_Point){snake_x_position, snake_y_position};

            //if snake collides with apple
            if (snake_x_position == apple_x_position && snake_y_position == apple_y_position) {
                generate_apple_position(&apple_x_position, &apple_y_position);
                pending_growth += SNAKE_LENGTH_INCREASE; //pend growth
                sprintf(score_text, "Score: %d", snake_length); //update score text
            }

            //if snake collides with itself
            for (int i = 1; i < snake_length - 1; i++) { 
                if (snake_body[i].x == snake_x_position && snake_body[i].y == snake_y_position) {
                    game_over = true;
                }
            }

            //if snake head moves out of frame
            if (snake_collided_with_screen_edge(snake_x_position, snake_y_position)) {
                game_over = true;
            }

            //gradually grow snake
            if (pending_growth > 0 && snake_length < SNAKE_MAX_LENGTH) {
                snake_length++;
                pending_growth--;
                snake_body[snake_length - 1] = snake_body[snake_length - 2];
            }
        }

        //color background and clear it from renders
        SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
        SDL_RenderClear(renderer);

        //RENDER HERE

        //render snake
        for (int i = 0; i < snake_length; i++) {
            render_square(renderer, (RenderSquareParams){.x=snake_body[i].x, .y=snake_body[i].y, .color=colors[2]});
        }

        //render apple
        render_square(renderer, (RenderSquareParams){.x=apple_x_position, .y=apple_y_position, .color=colors[3]});

        //render score text
        render_text(renderer, font, score_text, SCORE_DISPLAY_POSITION[0], SCORE_DISPLAY_POSITION[1], colors[1]);
        
        //uncomment this AND the function if you want to visualize grid
        /*render_grid(renderer);*/

        //RENDER HERE

        //renders present
        SDL_RenderPresent(renderer);
    }

    //destroys everything
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

void render_square(SDL_Renderer *renderer, RenderSquareParams overrides) {
    RenderSquareParams p = {
        .x=DEFAULT_POSITION,
        .y=DEFAULT_POSITION,
        .width=SQUARE_SIZE,
        .height=SQUARE_SIZE,
        .color=colors[0],
        .filled=SQUARE_FILLED,
        .renderer=renderer
    };

    if (overrides.x) p.x = overrides.x;
    if (overrides.y) p.y = overrides.y;
    if (overrides.width) p.width = overrides.width;
    if (overrides.height) p.height = overrides.height;
    p.color = overrides.color;
    if (overrides.filled) p.filled = overrides.filled;

    SDL_FRect square_rect = {p.x, p.y, p.width, p.height};
    SDL_SetRenderDrawColor(p.renderer, p.color.r, p.color.g, p.color.b, p.color.a);

    //filled meaning the square will be colored in
    if (p.filled) { 
        SDL_RenderFillRect(p.renderer, &square_rect);
    }
    else {
        SDL_RenderRect(p.renderer, &square_rect);
    }
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text, 0, color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FRect destination_rect = {
        (float)x,
        (float)y,
        (float)text_surface->w,
        (float)text_surface->h
    };

    SDL_RenderTexture(renderer, text_texture, NULL, &destination_rect);
    SDL_DestroySurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void render_pause_screen(SDL_Renderer *renderer, TTF_Font *font) {
    //render background
    render_square(renderer, (RenderSquareParams){.width=WIDTH, .height=HEIGHT, .color=colors[4]});
    //render "Paused" text
    render_text(renderer, font, "PAUSED", OBJECT_CENTER[0], OBJECT_CENTER[1], colors[3]);
    
    //more stuff adding soon!!!!!!
}

void render_game_over_screen(SDL_Renderer *renderer, TTF_Font *font) {
    //render background
    render_square(renderer, (RenderSquareParams){.width=WIDTH, .height=HEIGHT, .color=colors[4]});
    //render "Game Over" text
    render_text(renderer, font, "GAME OVER", OBJECT_CENTER[0], OBJECT_CENTER[1], colors[3]);

    //more stuff adding soon!!!!!
}

int* generate_apple_position(int *x, int *y) {
    *x = (rand() % (WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
    *y = (rand() % (WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
}

bool snake_collided_with_screen_edge(int x_position, int y_position) {
    return x_position < 0 || x_position >= WIDTH || y_position < 0 || y_position >= HEIGHT;
}

/*
void render_grid(SDL_Renderer *renderer) {
    for (int i = 0; i <= WIDTH; i+=SQUARE_SIZE) {
        SDL_RenderLine(renderer, i, 0, i, HEIGHT);
    }

    for (int i = 0; i <= HEIGHT; i+=SQUARE_SIZE) {
        SDL_RenderLine(renderer, 0, i, WIDTH, i);
    }
}
*/