#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"


enum SceneType { START, LEVEL, END };

struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map *map = nullptr;
    Entity *player = nullptr;
    Entity *enemies = nullptr;
    Entity *background = nullptr;
    
    // ————— AUDIO ————— //
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    
    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;
};


class Scene {
protected:
    GameState m_game_state;
    SceneType m_scene_type;

    // ----- PLAYER LIVES ----- //
    int m_player_lives;
    
public:
    // ————— ATTRIBUTES ————— //
    int m_number_of_enemies = 1;
    
    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    // ————— GETTERS ————— //
    SceneType const get_scene_type() const { return m_scene_type;         }
    GameState const get_state() const { return m_game_state;              }
    int const get_number_of_enemies() const { return m_number_of_enemies; }
    int const get_player_lives() const { return m_player_lives; }

    // ----- SETTER ----- //
    void const set_player_lives(int new_player_lives) { m_player_lives = new_player_lives; }
};
