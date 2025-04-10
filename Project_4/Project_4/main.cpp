/**
* Author: Lucy Zheng
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Effects.h"

#include "StartScene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "EndScene.h"

// ————— CONSTANTS ————— //
constexpr float WINDOW_MULT = 1.5f;
constexpr float WINDOW_WIDTH  = 640 * WINDOW_MULT,
                WINDOW_HEIGHT = 480 * WINDOW_MULT;

constexpr float BG_RED = 0.0f,
                BG_BLUE    = 0.0f,
                BG_GREEN   = 0.0f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene* g_current_scene;
StartScene* g_start_scene;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
EndScene* g_end_scene;
Scene* g_scenes[5];

int g_player_lives;

SDL_Window* g_display_window;

bool is_paused = false;
bool is_game_over = false;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;
float g_pause_ticks = 0.0f;

// ----- EFFECTS ----- //
Effects* g_effects;

// ----- MUSIC ----- //
constexpr int CD_QUAL_FREQ = 44100,
              AUDIO_CHAN_AMT = 2,
              AUDIO_BUFF_SIZE = 4096;

constexpr char BGM_FILEPATH[] = "assets/Music/bgm.mp3";
constexpr int LOOP_FOREVER = -1;
Mix_Music* g_music;


void switch_to_scene(Scene *scene)
{
    if (g_current_scene != nullptr && g_current_scene->get_scene_type() != START)
        g_player_lives = g_current_scene->get_player_lives();
    g_current_scene = scene;
    g_current_scene->initialise();
    g_current_scene->set_player_lives(g_player_lives);

    if (g_current_scene != nullptr && g_current_scene->get_scene_type() == LEVEL)
        g_effects->start(SHRINK, 4.0f);
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Start Audio
    Mix_OpenAudio(
        CD_QUAL_FREQ,       // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT, // audio format
        AUDIO_CHAN_AMT,     // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE     // audio buffer size in sample FRAMES (total samples divided by channel count)
    );

    // ----- MUSIC INITIALIZATION ----- //
    g_music = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_music, LOOP_FOREVER);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    // ----- WINDOW ----- //
    g_display_window = SDL_CreateWindow("Platformer",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // ----- SCENES SETUP ----- //
    g_start_scene = new StartScene();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    g_end_scene = new EndScene();

    g_scenes[0] = g_start_scene;
    g_scenes[1] = g_level_a;
    g_scenes[2] = g_level_b;
    g_scenes[3] = g_level_c;
    g_scenes[4] = g_end_scene;

    switch_to_scene(g_scenes[0]);

    g_player_lives = 3;

    // ----- EFFECTS ----- //
    g_effects = new Effects(g_projection_matrix, g_view_matrix);

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // Reset player movement
    if (g_current_scene->get_scene_type() == LEVEL) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                g_app_status = TERMINATED;
                break;

                // Restart Scene
            case SDLK_r:
                switch_to_scene(g_current_scene);
                break;

                // Press Enter at Start Screen
            case SDLK_RETURN:
                if (g_current_scene->get_scene_type() == START) {
                    switch_to_scene(g_level_a);
                }
                break;

                // Pause Screen
            case SDLK_p:
                if (g_current_scene->get_scene_type() == LEVEL)
                {
                    is_paused = !is_paused;
                    if (!is_paused)
                        g_previous_ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
                }
                break;

                // Player jumping
            case SDLK_SPACE:
                if (g_current_scene->get_scene_type() == LEVEL) {
                    if (g_current_scene->get_state().player->get_collided_bottom()) {
                        g_current_scene->get_state().player->jump();
                        Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                    }
                }
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    // ————— KEY HOLD ————— //
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    // Player movement
    if (g_current_scene->get_scene_type() == LEVEL) {

        if (key_state[SDL_SCANCODE_LEFT])
            g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT])
            g_current_scene->get_state().player->move_right();
        else
        {
            if (!g_current_scene->get_state().player->get_collided_bottom())
            {
                if (g_current_scene->get_state().player->get_velocity().y > 0.0f)
                    g_current_scene->get_state().player->set_player_state(JUMP);
                else
                    g_current_scene->get_state().player->set_player_state(FALL);
            }
            else
                g_current_scene->get_state().player->set_player_state(REST);
        }

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
}

void update()
{
    if (is_paused) return;

    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;

    // ————— PLAYER CAMERA ————— //
    if (g_current_scene->get_scene_type() == LEVEL)
    {
        g_view_matrix = glm::mat4(1.0f);

        if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, 
                glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }

        // ----- SCENE SWITCH ----- //
        if (g_current_scene->get_state().next_scene_id > 0)
            switch_to_scene(g_scenes[g_current_scene->get_state().next_scene_id]);
        if (g_current_scene == g_end_scene) g_view_matrix = glm::mat4(1.0f);

    }
}

void render()
{
    if (is_paused) return;

    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);
    g_effects->render();
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    Mix_FreeMusic(g_music);
    SDL_Quit();
    
    delete g_start_scene;
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
    
    delete g_effects;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
