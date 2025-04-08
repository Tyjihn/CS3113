#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
enum EntityType  { PLAYER, ENEMY, BACKGROUND };
enum AIType      { WALKER, GUARD, FLYER      };
enum AIState     { IDLE, MOVING, ATTACKING   };
enum PlayerState { REST, RUN, JUMP, FALL     };

enum Direction   { LEFT, RIGHT, UP, DOWN     };

class Entity
{
private:
    bool m_is_active = true;

    std::vector<std::vector<int>> m_animations;
    
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;
    PlayerState m_player_state;
    Direction m_direction;

    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    float     m_speed,
              m_jumping_power;
    
    bool m_is_jumping = false;

    // ————— TEXTURES ————— //
    GLuint              m_texture_id;
    std::vector<GLuint> m_texture_ids;

    // ————— ANIMATION ————— //
    int m_animation_cols;
    int m_animation_frames,
        m_animation_index,
        m_animation_rows;

    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;

    float m_width = 1.0f,
          m_height = 1.0f;

    float m_flyer_upper_bound = 0.0f,
          m_flyer_lower_bound = 0.0f;

    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;
    bool m_pit_detected_left  = false;
    bool m_pit_detected_right = false;

    float m_pit_detection_forward = 0.5f;
    float m_pit_detection_down = 0.5f;
    float m_pit_sensor_side_offset = 0.3f;

public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 14;

    // ————— METHODS ————— //
    Entity();
    Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration, float jump_power, std::vector<std::vector<int>> animations,
            float animation_time, int animation_frames, int animation_index, int animation_cols, int animation_rows, 
            float width, float height, EntityType EntityType, PlayerState PlayerState); // Player constructor
    Entity(GLuint texture_id, EntityType EntityType); // Simple constructor
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState); // AI constructor
    Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration, std::vector<std::vector<int>> animations, float animation_time, 
            int animation_frames, int animation_index, int animation_cols, int animation_rows, float width, float height,
            EntityType EntityType, AIType AIType, AIState AIState); // AI constructor + animation
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map);
    void render(ShaderProgram* program);

    void ai_activate(Entity *player);
    void ai_walk();
    void ai_guard(Entity *player);
    void ai_flyer();

    void const check_pit_detection(Map *map);
    void set_pit_detection(float forward, float down, float side_offset)
    {
        m_pit_detection_forward = forward;
        m_pit_detection_down = down;
        m_pit_sensor_side_offset = side_offset;
    }
    
    void normalise_movement() { m_movement = glm::normalize(m_movement); }

    void face_left()  { m_scale.x = -std::abs(m_scale.x); }
    void face_right() { m_scale.x =  std::abs(m_scale.x); }

    void move_left() { 
        m_movement.x = -1.0f;
        face_left();
        if (m_entity_type == PLAYER) set_player_state(RUN);
    }

    void move_right() {
        m_movement.x = 1.0f; 
        face_right();
        if (m_entity_type == PLAYER) set_player_state(RUN);
    }

    void move_up()   { m_movement.y = 1.0f;  }
    void move_down() { m_movement.y = -1.0f; }
    
    void jumping() { set_player_state(JUMP); }
    void falling() { set_player_state(FALL); }
    void resting() { set_player_state(REST); }
    
    void const jump() { m_is_jumping = true; }

    // ————— GETTERS ————— //
    EntityType          const get_entity_type()      const { return m_entity_type;    };
    AIType              const get_ai_type()          const { return m_ai_type;        };
    AIState             const get_ai_state()         const { return m_ai_state;       };
    Direction           const get_direction()        const { return m_direction;      };
    PlayerState         const get_player_state()     const { return m_player_state;   }
    float               const get_jumping_power()    const { return m_jumping_power;  }
    glm::vec3           const get_position()         const { return m_position;       }
    glm::vec3           const get_velocity()         const { return m_velocity;       }
    glm::vec3           const get_acceleration()     const { return m_acceleration;   }
    glm::vec3           const get_movement()         const { return m_movement;       }
    glm::vec3           const get_scale()            const { return m_scale;          }
    GLuint              const get_texture_id()       const { return m_texture_id;     }
    std::vector<GLuint> const get_texture_ids()      const { return m_texture_ids;    }
    float               const get_speed()            const { return m_speed;          }
    bool                const get_collided_top()     const { return m_collided_top;   }
    bool                const get_collided_bottom()  const { return m_collided_bottom;}
    bool                const get_collided_right()   const { return m_collided_right; }
    bool                const get_collided_left()    const { return m_collided_left;  }
    float               const get_width()            const { return m_width;          }
    float               const get_height()           const { return m_height;         }
    bool                const get_pit_detect_left()  const { return m_pit_detected_left; }
    bool                const get_pit_detect_right() const { return m_pit_detected_right; }
    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };

    // ————— SETTERS ————— //
    void const set_pit_detect_left(bool new_pit_detection) { m_pit_detected_left = new_pit_detection; }
    void const set_pit_detect_right(bool new_pit_detection) { m_pit_detected_right = new_pit_detection; }
    void const set_flyer_upper_bound(float new_upper_bound) { m_flyer_upper_bound = new_upper_bound; }
    void const set_flyer_lower_bound(float new_lower_bound) { m_flyer_lower_bound = new_lower_bound; }
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type) { m_ai_type = new_ai_type;};
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
    void const set_jumping_power(float new_jumping_power) { m_jumping_power = new_jumping_power;}
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }
    void const set_animations(std::vector<std::vector<int>> new_animations) { m_animations = new_animations; }
    void const set_direction(Direction new_direction)
    {
        m_direction = new_direction;
        if (new_direction == LEFT) face_left();
        else if (new_direction == RIGHT) face_right();
        else if (new_direction == UP) move_up();
        else move_down();
    };
    void const set_player_state(PlayerState new_player_state)
    {
        if (m_player_state == new_player_state) return;

        m_player_state = new_player_state;

        if (!m_animations.empty() && m_animations.size() > m_player_state)
        {
            m_animation_indices = m_animations[m_player_state].data();
        }
        else
        {
            m_animation_indices = nullptr;
            return;
        }

        m_animation_cols = (int)m_animations[m_player_state].size();
        m_animation_frames = (int)m_animations[m_player_state].size();
    }

    void const set_ai_state(AIState new_ai_state)
    {
        if (m_ai_state == new_ai_state) return;

        m_ai_state = new_ai_state;

        if (!m_animations.empty() && m_animations.size() > m_ai_state)
        {
            m_animation_indices = m_animations[m_ai_state].data();
        }
        else
        {
            m_animation_indices = nullptr;
            return;
        }

        m_animation_cols = (int)m_animations[m_ai_state].size();
        m_animation_frames = (int)m_animations[m_ai_state].size();
    }
};

#endif // ENTITY_H
