/**
* Author: Lucy Zheng
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 11

static unsigned int LEVEL_DATA[] =
{
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
}

void LevelA::initialise()
{
    m_scene_type = LEVEL;
    m_game_state.next_scene_id = -1;

    m_font_texture_id = Utility::load_texture("assets/font2.png");
    GLuint map_texture_id = Utility::load_texture("assets/tileset00.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 5, 1);

    // ----- Background Initialization ----- //
    constexpr float bg_mult = 15.0f;
    GLuint background_texture_id = Utility::load_texture("assets/background.png");

    m_game_state.background = new Entity(background_texture_id, BACKGROUND);

    m_game_state.background->set_position(glm::vec3(9.2f, 0.75f, 0.0f));
    m_game_state.background->set_scale(glm::vec3(1.373f * bg_mult, 1.0f * bg_mult, 1.0f));

    // ----- Player Initialization ----- //
    std::vector<std::vector<int>> player_animations =
    {
        { 0, 1, 2, 3, 4, 5, 6, 7 },         // Rest
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },   // Run
        { 0, 1, 2, 3 },                     // Jump
        { 0, 1, 2, 3 }                      // Fall
    };

    std::vector<GLuint> player_texture_ids =
    {
        Utility::load_texture("assets/player/rest.png"),
        Utility::load_texture("assets/player/run.png"),
        Utility::load_texture("assets/player/jump.png"),
        Utility::load_texture("assets/player/fall.png")
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -17.0f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_ids,        // texture id
        4.0f,                      // speed
        acceleration,              // acceleration
        7.25f,                     // jumping power
        player_animations,         // animation index sets
        0.0f,                      // animation time
        8,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        1,                         // animation row amount
        0.8f,                      // width
        0.8f,                      // height
        PLAYER,                    // entity type
        REST                       // entity state
    );

    m_game_state.player->set_position(glm::vec3(2.0f, -4.0f, 0.0f));
    
    // ----- Enemies Initialization ----- //
    std::vector<std::vector<int>> enemy_animations =
    {
        { 0, 1, 2, 3, 4, 5 },                           // Idle
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },   // Moving
        { 0, 1, 2, 3, 4, 5, 6, 7 }                      // Attacking
    };

    std::vector<GLuint> enemy_texture_ids =
    {
        Utility::load_texture("assets/slime/idle.png"),
        Utility::load_texture("assets/slime/hop.png"),
        Utility::load_texture("assets/slime/attack.png")
    };

    glm::vec3 enemy_acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(
            enemy_texture_ids,      // texture ids
            2.0f,                   // speed
            enemy_acceleration,     // acceleration
            enemy_animations,       // animation index sets
            0.0f,                   // animation time
            6,                      // animation frame amount
            0,                      // current animation index
            6,                      // animation column amount
            1,                      // animation row amount
            0.85f,                  // width
            0.7f,                   // height
            ENEMY,                  // entity type
            WALKER,                 // AI type
            IDLE                    // AI state
        );
    }

    m_game_state.enemies[0].set_position(glm::vec3(11.0f, -5.0f, 0.0f));
    m_game_state.enemies[0].set_direction(LEFT);

    // ----- BGM and SFX ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.jump_sfx = Mix_LoadWAV("assets/music/jump.wav");
}

void LevelA::update(float delta_time)
{
    m_game_state.background->update(delta_time, NULL, NULL, NULL, NULL);
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, m_number_of_enemies, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }

    // ----- Switch Scene ----- //
    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 2;

    // ----- Player Lives ----- //
    for (int i = 0; i < m_number_of_enemies; i++) {
        if (m_game_state.player->check_collision(&m_game_state.enemies[i])) {
            player_death();
            break;
        }
    }
}

void LevelA::render(ShaderProgram *g_shader_program)
{
    m_game_state.background->render(g_shader_program);
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].render(g_shader_program);
    }

    // ----- Render Player Lives ----- //
    std::string lives_text = "Lives: " + std::to_string(get_player_lives());

    Utility::draw_text(g_shader_program, m_font_texture_id, lives_text, 0.2f, 0.0f,
        glm::vec3(m_game_state.player->get_position().x + 3.2f, -0.4f, 0.0f));
}

void LevelA::player_death() {
    set_player_lives(get_player_lives() - 1);  // Lose 1 life

    if (get_player_lives() <= 0) {
        m_game_state.next_scene_id = 4;  // Game Over Scene
    }
    else {
        initialise();  // Reset Level
    }
}