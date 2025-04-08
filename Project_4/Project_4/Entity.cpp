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
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

// ------------ DEBUG -----------
#include <windows.h>
// --------------

void Entity::ai_activate(Entity *player)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_walk();
            break;
            
        case GUARD:
            ai_guard(player);
            break;

        case FLYER:
            ai_flyer();
            break;
            
        default:
            set_ai_state(IDLE);
            break;
    }
}

void Entity::ai_walk()
{
    set_ai_state(MOVING);
    if (m_direction == LEFT)
    {
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        face_left();
    }
    else if (m_direction == RIGHT)
    {
        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        face_right();
    }
}

void Entity::ai_guard(Entity *player)
{
    switch (m_ai_state) {
        case IDLE:
            if (glm::distance(m_position, player->get_position()) < 3.0f) {
                set_ai_state(MOVING);
            }
            break;
            
        case MOVING:
            if (m_position.x > player->get_position().x) {
                m_movement = glm::vec3(-1.0f, 0.0f, 0.0f); 
                face_left();
            } else {
                m_movement = glm::vec3(1.0f, 0.0f, 0.0f); 
                face_right();
            }
            break;
            
        case ATTACKING:
            break;
            
        default:
            break;
    }
}

void Entity::ai_flyer() { }

//Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
}

// Player constructor
Entity::Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration,
    float jump_power, std::vector<std::vector<int>> animations, float animation_time, 
    int animation_frames, int animation_index, int animation_cols, int animation_rows, 
    float width, float height, EntityType EntityType, PlayerState player_state)
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power),
    m_animations(animations), m_animation_cols(animation_cols), m_animation_frames(animation_frames),
    m_animation_index(animation_index), m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_ids(texture_ids), m_velocity(0.0f), m_width(width), 
    m_height(height), m_entity_type(EntityType), m_player_state(player_state)
{
    set_player_state(player_state);
}

// Simple constructor (Background)
Entity::Entity(GLuint texture_id, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f), m_texture_id(texture_id), 
    m_velocity(0.0f), m_acceleration(0.0f), m_width(1.0f), m_height(1.0f), m_entity_type(EntityType)
{
}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState)
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0), m_animation_rows(0), 
    m_animation_indices(nullptr), m_animation_time(0.0f), m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), 
    m_width(width), m_height(height),m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
}

// Enemy constructor
Entity::Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration, std::vector<std::vector<int>> animations,
    float animation_time, int animation_frames, int animation_index, int animation_cols, int animation_rows, 
    float width, float height, EntityType EntityType, AIType AIType, AIState AIState)
    : m_position(glm::vec3(0.0f)), m_movement(glm::vec3(0.0f)), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f), m_acceleration(acceleration),
    m_speed(speed), m_animations(animations), m_animation_cols(animation_cols), m_animation_frames(animation_frames),
    m_animation_index(animation_index), m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_ids(texture_ids), m_velocity(glm::vec3(0.0f)),
    m_width(width), m_height(height), m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
    set_ai_state(AIState);
}

Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                m_position.y   -= y_overlap;
                m_velocity.y    = 0;

                m_collided_top  = true;
            } else if (m_velocity.y < 0)
            {
                m_position.y      += y_overlap;
                m_velocity.y       = 0;

                m_collided_bottom  = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x     -= x_overlap;
                m_velocity.x      = 0;

                m_collided_right  = true;
                
            } else if (m_velocity.x < 0)
            {
                m_position.x    += x_overlap;
                m_velocity.x     = 0;
 
                m_collided_left  = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map *map)
{
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
            m_position.y += penetration_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        
    }
}

void const Entity::check_collision_x(Map *map)
{
    glm::vec3 left  = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void const Entity::check_pit_detection(Map *map)
{
    if (m_entity_type != ENEMY || !m_is_active) return;

    // Calculate sensor positions based on facing direction
    float direction_multiplier = (m_direction == RIGHT) ? 1.0f : -1.0f;

    // Main front sensor (aligned with movement direction)
    glm::vec3 front_sensor = m_position;
    front_sensor.x += m_pit_detection_forward * direction_multiplier;
    front_sensor.y -= m_pit_detection_down;

    // Secondary sensors (left/right of front sensor for edge cases)
    glm::vec3 left_sensor = front_sensor;
    left_sensor.x -= m_pit_sensor_side_offset;

    glm::vec3 right_sensor = front_sensor;
    right_sensor.x += m_pit_sensor_side_offset;

    // Dummy variables for penetration (not used for pit detection)
    float pen_x, pen_y;

    // Check ground existence at sensor points
    bool front_ground = map->is_solid(front_sensor, &pen_x, &pen_y);
    bool left_ground = map->is_solid(left_sensor, &pen_x, &pen_y);
    bool right_ground = map->is_solid(right_sensor, &pen_x, &pen_y);

    // Update detection flags
    m_pit_detected_left = !left_ground;
    m_pit_detected_right = !right_ground;

    // Determine if we should turn around
    bool should_turn = false;

    if (m_movement.x < 0) { // Moving left
        should_turn = !front_ground || !left_ground;
    }
    else if (m_movement.x > 0) { // Moving right
        should_turn = !front_ground || !right_ground;
    }

    // Execute turn if needed
    if (should_turn) {
        m_movement.x *= -1;
        m_direction = (m_direction == RIGHT) ? LEFT : RIGHT;
    }
}

void Entity::update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map)
{
    if (!m_is_active) return;

    if (m_entity_type == BACKGROUND)
    {
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
        m_model_matrix = glm::scale(m_model_matrix, m_scale);
        return;
    }

    if (m_entity_type == ENEMY)
    {
        if (m_ai_type == WALKER) {
            if (m_collided_left) set_direction(RIGHT);
            else if (m_collided_right) set_direction(LEFT);
        }
        else if (m_ai_type == FLYER) {
            if (m_position.y >= m_flyer_upper_bound) set_direction(DOWN);
            else if (m_position.y <= m_flyer_lower_bound) set_direction(UP);
        }
        ai_activate(player);
    }
 
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;

    if (m_animation_indices != NULL)
    {
        if (m_animation_frames > 1)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float) 1 / SECONDS_PER_FRAME;
            
            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }
    
    if (m_ai_type == FLYER) { m_velocity.y = m_movement.y * m_speed; }
    else { m_velocity.x = m_movement.x * m_speed; }
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;
    
    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}


void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) return;

    program->set_model_matrix(m_model_matrix);

    GLuint current_texture = m_texture_id;

    if (m_entity_type == PLAYER)
        current_texture = m_texture_ids[m_player_state];
    else if (m_entity_type == ENEMY)
        current_texture = m_texture_ids[m_ai_state];

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, current_texture, 
            m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, current_texture);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}