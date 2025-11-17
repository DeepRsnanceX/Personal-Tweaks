#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct RenderTexture {
    GLuint fbo = 0;
    GLuint tex = 0;
    GLuint rbo = 0;
};

class BlurLayer : public CCLayerColor {
protected:
    bool m_visiting = false;
    float m_blurStrength = 1.0f;
    
    bool init() override;
    void visit() override;
    void draw() override;
    
    static void setupShaders();
    static void cleanupShaders();
    
public:
    static inline RenderTexture s_rt0;
    static inline RenderTexture s_rt1;
    
    static inline GLuint s_vao = 0;
    static inline GLuint s_vbo = 0;
    static inline GLuint s_program = 0;
    static inline GLint s_uniformFirst = 0;
    static inline GLint s_uniformRadius = 0;
    static inline bool s_initialized = false;
    
    static BlurLayer* create();
    
    void setBlurStrength(float strength) { m_blurStrength = strength; }
    float getBlurStrength() const { return m_blurStrength; }
    
    static void setupRenderTextures(GLsizei width, GLsizei height);
    static void cleanupRenderTextures();
    
    ~BlurLayer();
};