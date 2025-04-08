/**
* Author: Lucy Zheng
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 11

static unsigned int LEVEL_DATA[] =
{
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 2, 2, 2,
    2, 1, 1, 1, 0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 0, 0, 2, 2, 2, 2,
    2, 2, 2, 2, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 2, 2, 2, 2,
    2, 2, 2, 2, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 2, 2, 2, 2,
    2, 2, 2, 2, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 2, 2, 2, 2,
    2, 2, 2, 2, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 2, 2, 2, 2
};

LevelC::~LevelC()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
}

void LevelC::initialise()
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

    m_game_state.player->set_position(glm::vec3(2.0f, -3.0f, 0.0f));

    // ----- Enemies Initialization ----- //
    std::vector<std::vector<int>> flyer_animations =
    {
        { 0, 1, 2, 3, 4, 5, 6 },        // Idle
        { 0, 1, 2, 3, 4, 5, 6 },        // Flying
        { 0, 1, 2, 3, 4, 5, 6, 7 }      // Attacking
    };

    std::vector<GLuint> flyer_texture_ids =
    {
        Utility::load_texture("assets/bat/idle.png"),
        Utility::load_texture("assets/bat/fly.png"),
        Utility::load_texture("assets/bat/attack.png")
    };

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT - 1; i++)
    {
        m_game_state.enemies[i] = Entity(
            flyer_texture_ids,      // texture ids
            5.0f,                  // speed
            glm::vec3(0.0f),        // acceleration
            flyer_animations,       // animation index sets
            0.0f,                   // animation time
            0,                      // animation frame amount
            0,                      // current animation index
            7,                      // animation column amount
            1,                      // animation row amount
            0.8f,                   // width
            0.8f,                   // height
            ENEMY,                  // entity type
            FLYER,                  // AI type
            IDLE                    // AI state
        );
    }
    m_game_state.enemies[0].set_ai_state(MOVING);
    m_game_state.enemies[0].set_direction(UP);
    m_game_state.enemies[0].set_position(glm::vec3(4.5f, -4.0f, 0.0f));
    m_game_state.enemies[0].set_scale(glm::vec3(1.2f, 1.3f, 0.0f));
    m_game_state.enemies[0].set_flyer_upper_bound(
                m_game_state.enemies[0].get_position().y + 3.0f);
    m_game_state.enemies[0].set_flyer_lower_bound(
                m_game_state.enemies[0].get_position().y - 3.0f);

    m_game_state.enemies[1].set_ai_state(MOVING);
    m_game_state.enemies[1].set_direction(UP);
    m_game_state.enemies[1].set_position(glm::vec3(14.5f, -3.0f, 0.0f));
    m_game_state.enemies[1].set_scale(glm::vec3(1.2f, 1.3f, 0.0f));
    m_game_state.enemies[1].set_flyer_upper_bound(
        m_game_state.enemies[1].get_position().y + 2.5f);
    m_game_state.enemies[1].set_flyer_lower_bound(
        m_game_state.enemies[1].get_position().y - 2.5f);
    m_game_state.enemies[1].set_speed(4.0f);

    // ----- GUARD Initialization ----- //
    std::vector<std::vector<int>> guard_animations =
    {
        { 0 },                                              // Idle
        { 0, 1, 2, 3, 4, 5, 6 },                            // Moving
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }    // Attacking
    };

    std::vector<GLuint> guard_texture_ids =
    {
        Utility::load_texture("assets/knight/idle.png"),
        Utility::load_texture("assets/knight/run.png"),
        Utility::load_texture("assets/knight/attack.png")
    };

    glm::vec3 guard_acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

    m_game_state.enemies[2] = Entity(
        guard_texture_ids,      // texture ids
        1.5f,                   // speed
        guard_acceleration,     // acceleration
        guard_animations,       // animation index sets
        0.0f,                   // animation time
        0,                      // animation frame amount
        0,                      // current animation index
        7,                      // animation column amount
        1,                      // animation row amount
        0.8f,                   // width
        0.8f,                   // height
        ENEMY,                  // entity type
        GUARD,                  // AI type
        IDLE                    // AI state
    );

    m_game_state.enemies[2].set_position(glm::vec3(18.0f, -2.0f, 0.0f));
    m_game_state.enemies[2].set_scale(glm::vec3(1.2f, 0.8f, 0.0f));
    m_game_state.enemies[2].set_direction(LEFT);

    // ----- BGM and SFX ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.jump_sfx = Mix_LoadWAV("assets/music/jump.wav");
}

void LevelC::update(float delta_time)
{
    m_game_state.background->update(delta_time, NULL, NULL, NULL, NULL);
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, m_number_of_enemies, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }

    // ----- PLAYER DEATH ----- //
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.player->check_collision(&m_game_state.enemies[i])) {
            set_player_lives(get_player_lives() - 1);  // Lose 1 life
            if (get_player_lives() <= 0) {
                m_game_state.next_scene_id = 4;  // Game Over Scene
            }
            else {
                initialise(); // Reset Level
            }
            break;
        }
    }

    float player_bottom = m_game_state.player->get_position().y - (m_game_state.player->get_height() / 2.0f);

    if (player_bottom >= -9.0f && player_bottom < -7.5f) {
        float player_left = m_game_state.player->get_position().x - (m_game_state.player->get_width() / 2.0f);
        float player_right = m_game_state.player->get_position().x + (m_game_state.player->get_width() / 2.0f);

        if ((player_right > 4.0f && player_left < 6.0f) || (player_right > 14.0f && player_left < 16.0f)) {
            player_death();
        }
    }

    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 4;
}

void LevelC::render(ShaderProgram* g_shader_program)
{
    m_game_state.background->render(g_shader_program);
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++)
        m_game_state.enemies[i].render(g_shader_program);

    // ----- Render Player Lives ----- //
    std::string lives_text = "Lives: " + std::to_string(get_player_lives());

    Utility::draw_text(g_shader_program, m_font_texture_id, lives_text, 0.2f, 0.0f,
        glm::vec3(m_game_state.player->get_position().x + 3.2f, -0.4f, 0.0f));
}

void LevelC::player_death() {
    set_player_lives(get_player_lives() - 1);  // Lose 1 life

    if (get_player_lives() <= 0) {
        m_game_state.next_scene_id = 4;  // Game Over Scene
    }
    else {
        initialise();  // Reset Level
    }
}