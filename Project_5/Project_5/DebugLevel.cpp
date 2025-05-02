#include "DebugLevel.h"
#include "Utility.h"

#define LEVEL_WIDTH 13
#define LEVEL_HEIGHT 9

static unsigned int LEVEL_DATA[] =
{
    99, 59, 59, 59, 61, 66, 67, 68, 61, 59, 59, 59, 56,
    57,  8, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 87,
    57, 22, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57, 22, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57, 22, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57, 22, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57, 22, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57, 50, 51, 51, 51, 51, 51, 51, 51, 51, 51, 53, 87,
    84, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 101
};

DebugLevel::~DebugLevel()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.clone;
    delete    m_game_state.map;
    //Mix_FreeChunk(m_game_state.jump_sfx);
}

void DebugLevel::initialise()
{
    m_scene_type = LEVEL;
    m_game_state.next_scene_id = -1;

    m_font_texture_id = Utility::load_texture("assets/font2.png");

    GLuint map_texture_id = Utility::load_texture("assets/tileset/library_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 14, 13);

    // ----- Character ----- //
    std::vector<std::vector<std::vector<int>>> character_animations =
    {
        {                               // Idle
            { 0, 1, 2, 3 },                 // left
            { 0, 1, 2, 3 },                 // right
            { 0, 1, 2, 3 },                 // up
            { 0, 1, 2, 3 },                 // down
        },
        {                               // Walk
            { 0, 1, 2, 3, 4, 5, 6, 7 },     // left
            { 0, 1, 2, 3, 4, 5, 6, 7 },     // right
            { 0, 1, 2, 3, 4, 5, 6, 7 },     // up
            { 0, 1, 2, 3, 4, 5, 6, 7 }      // down
        },
        {                               // Hurt
            { 0,  1,  2,  3 },              // left
            { 0,  1,  2,  3 },              // right
            { 0,  1,  2,  3 },              // up
            { 0,  1,  2,  3 }               // down
        }
    };

    std::vector<std::vector<GLuint>> character_texture_ids =
    {
        {
            Utility::load_texture("assets/character/idle/left.png"),
            Utility::load_texture("assets/character/idle/right.png"),
            Utility::load_texture("assets/character/idle/up.png"),
            Utility::load_texture("assets/character/idle/down.png")
        },
        {
            Utility::load_texture("assets/character/walk/left.png"),
            Utility::load_texture("assets/character/walk/right.png"),
            Utility::load_texture("assets/character/walk/up.png"),
            Utility::load_texture("assets/character/walk/down.png")
        },
        {
            Utility::load_texture("assets/character/hurt/left.png"),
            Utility::load_texture("assets/character/hurt/right.png"),
            Utility::load_texture("assets/character/hurt/up.png"),
            Utility::load_texture("assets/character/hurt/down.png")
        }
    };

    // ----- Player Initialization ----- //
    m_game_state.player = new Entity(
        character_texture_ids,     // texture id
        2.0f,                      // speed
        glm::vec3(0.0f),           // acceleration
        character_animations,      // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        1,                         // animation row amount
        1.0f,                     // width
        1.0f,                     // height
        CHARACTER,                 // entity type
        PLAYER,                    // character type
        REST,                      // character state
        DOWN                       // direction
    );

    m_game_state.player->set_position(glm::vec3(-2.0f, -1.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(1.0f, 1.0f, 0.0f));

    // ----- Clone Initialization ----- //
    m_game_state.clone = new Entity(
        character_texture_ids,     // texture id
        2.0f,                      // speed
        glm::vec3(0.0f),           // acceleration
        character_animations,      // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        1,                         // animation row amount
        1.0f,                     // width
        1.0f,                     // height
        CHARACTER,                 // entity type
        CLONE,                     // character type
        REST,                      // character state
        DOWN                       // direction
    );

    m_game_state.clone->set_position(glm::vec3(2.0f, -1.0f, 0.0f));
    m_game_state.clone->set_scale(glm::vec3(1.0f, 1.0f, 0.0f));

    // ----- Enemies Initialization ----- //
    std::vector<std::vector<int>> enemy_animations =
    {
        { 0, 1, 2, 3, 4, 5 },                           // Idle
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },   // Moving
        { 0, 1, 2, 3, 4, 5, 6, 7 }                      // Attacking
    };

    //std::vector<GLuint> enemy_texture_ids =
    //{
    //    Utility::load_texture("assets/slime/idle.png"),
    //    Utility::load_texture("assets/slime/hop.png"),
    //    Utility::load_texture("assets/slime/attack.png")
    //};

    glm::vec3 enemy_acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

    //m_game_state.enemies = new Entity[ENEMY_COUNT];

    //for (int i = 0; i < ENEMY_COUNT; i++)
    //{
    //    m_game_state.enemies[i] = Entity(
    //        enemy_texture_ids,      // texture ids
    //        2.0f,                   // speed
    //        enemy_acceleration,     // acceleration
    //        enemy_animations,       // animation index sets
    //        0.0f,                   // animation time
    //        6,                      // animation frame amount
    //        0,                      // current animation index
    //        6,                      // animation column amount
    //        1,                      // animation row amount
    //        0.85f,                  // width
    //        0.7f,                   // height
    //        ENEMY,                  // entity type
    //        WALKER,                 // AI type
    //        IDLE                    // AI state
    //    );
    //}

    //m_game_state.enemies[0].set_position(glm::vec3(11.0f, -5.0f, 0.0f));
    //m_game_state.enemies[0].set_direction(LEFT);

    // ----- BGM and SFX ----- //
    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    //m_game_state.jump_sfx = Mix_LoadWAV("assets/music/jump.wav");

    m_game_state.player->move_down(false);
    m_game_state.clone->move_down(false);
}

void DebugLevel::update(float delta_time)
{
    //m_game_state.background->update(delta_time, NULL, NULL, NULL, NULL);
    //m_game_state.player->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map); // TEST
    //m_game_state.clone->update(delta_time, m_game_state.clone, NULL, NULL, m_game_state.map);

    //m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, m_number_of_enemies, m_game_state.map);
    //m_game_state.clone->update(delta_time, m_game_state.clone, m_game_state.enemies, m_number_of_enemies, m_game_state.map);
    //for (int i = 0; i < ENEMY_COUNT; i++) {
    //    m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    //}

    // ----- Switch Scene ----- //
    //if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 2;

    // ----- Player Lives ----- //
    //for (int i = 0; i < m_number_of_enemies; i++) {
    //    if (m_game_state.player->check_collision(&m_game_state.enemies[i])) {
    //        player_death();
    //        break;
    //    }
    //}
}

void DebugLevel::render(ShaderProgram* g_shader_program)
{
    //m_game_state.background->render(g_shader_program);
    m_game_state.map->render(g_shader_program);
    //m_game_state.player->render(g_shader_program);
    //m_game_state.clone->render(g_shader_program);
    //for (int i = 0; i < ENEMY_COUNT; i++) {
    //    m_game_state.enemies[i].render(g_shader_program);
    //}

    // ----- Render Player Lives ----- //
    //std::string lives_text = "Lives: " + std::to_string(get_player_lives());

    //Utility::draw_text(g_shader_program, m_font_texture_id, lives_text, 0.2f, 0.0f,
    //    glm::vec3(m_game_state.player->get_position().x + 3.2f, -0.4f, 0.0f));
}

void DebugLevel::player_death() {
    set_player_lives(get_player_lives() - 1);  // Lose 1 life

    if (get_player_lives() <= 0) {
        m_game_state.next_scene_id = 4;  // Game Over Scene
    }
    else {
        initialise();  // Reset Level
    }
}