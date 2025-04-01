/**
* Author: Lucy Zheng
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 10


//constexpr char PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png",
//               ENEMY_FILEPATH[] = "assets/soph.png";

static unsigned int LEVEL_DATA[] =
{
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelB::~LevelB()
{
    //delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    //Mix_FreeChunk(m_game_state.jump_sfx);
    //Mix_FreeMusic(m_game_state.bgm);
}

void LevelB::initialise()
{
    m_scene_type = LEVEL;
    m_game_state.next_scene_id = -1;

    m_font_texture_id = Utility::load_texture("assets/font2.png");
    GLuint map_texture_id = Utility::load_texture("assets/tileset00.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 5, 1);

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

    glm::vec3 acceleration = glm::vec3(0.0f, -22.0f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_ids,        // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        7.0f,                      // jumping power
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

    m_game_state.player->set_position(glm::vec3(2.0f, 0.0f, 0.0f));

    /**
     Enemies' stuff */
    //GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    //m_game_state.enemies = new Entity[m_number_of_enemies];

    //for (int i = 0; i < m_number_of_enemies; i++)
    //{
    //    m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    //}

    //m_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    //m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    //m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    ///**
    // BGM and SFX
    // */
    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    //
    //m_game_state.bgm = Mix_LoadMUS("assets/dooblydoo.mp3");
    //Mix_PlayMusic(m_game_state.bgm, -1);
    //Mix_VolumeMusic(0.0f);
    //
    //m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
}

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);

    //m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, m_number_of_enemies, m_game_state.map);

    //for (int i = 0; i < ENEMY_COUNT; i++)
    //{
    //    m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    //}

    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 3;

    // ----- Player Lives ----- //
    for (int i = 0; i < m_number_of_enemies; i++) {
        // Collide with enemy
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

    if (player_bottom >= -9.0f && player_bottom < -6.5f) {
        float player_left = m_game_state.player->get_position().x - (m_game_state.player->get_width() / 2.0f);
        float player_right = m_game_state.player->get_position().x + (m_game_state.player->get_width() / 2.0f);

        if (player_right > 9.0f && player_left < 12.0f) {
            player_death();
        }
    }

}

void LevelB::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    //for (int i = 0; i < m_number_of_enemies; i++)
    //    m_game_state.enemies[i].render(g_shader_program);

    // ----- Render Player Lives ----- //
    std::string lives_text = "Lives: " + std::to_string(get_player_lives());

    Utility::draw_text(g_shader_program, m_font_texture_id, lives_text, 0.2f, 0.0f,
        glm::vec3(m_game_state.player->get_position().x + 3.0f, -0.4f, 0.0f));
}

void LevelB::player_death() {
    set_player_lives(get_player_lives() - 1);  // Lose 1 life

    if (get_player_lives() <= 0) {
        m_game_state.next_scene_id = 4;  // Game Over Scene
    }
    else {
        initialise(); // Reset Level
    }
}