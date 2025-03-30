#include "Scene.h"

class StartScene : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_background_texture_id;
    GLuint m_font_texture_id;
    glm::vec3 m_background_position;
    glm::vec3 m_background_scale;

    // ————— DESTRUCTOR ————— //
    ~StartScene();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
