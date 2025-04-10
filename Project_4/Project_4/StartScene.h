#include "Scene.h"

class StartScene : public Scene {
public:
    // ����� STATIC ATTRIBUTES ����� //
    GLuint m_font_texture_id;

    // ����� DESTRUCTOR ����� //
    ~StartScene();

    // ����� METHODS ����� //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
