#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BlurLayer : public CCLayerColor {
protected:
    bool m_visiting = false;
    float m_blurStrength = 1.0f;
    
    static inline struct {
        GLuint fbo = 0;
        GLuint tex = 0;
        GLuint rbo = 0;
    } s_rt0, s_rt1;
    
    static inline GLuint s_vao = 0;
    static inline GLuint s_vbo = 0;
    static inline GLuint s_program = 0;
    static inline GLint s_uniformFast = 0;
    static inline GLint s_uniformFirst = 0;
    static inline GLint s_uniformRadius = 0;
    static inline bool s_initialized = false;
    
    bool init() override;
    void visit() override;
    void draw() override;
    
    static void setupShaders();
    static void cleanupShaders();
    static void setupRenderTextures(GLsizei width, GLsizei height);
    static void cleanupRenderTextures();
    
public:
    static BlurLayer* create();
    
    void setBlurStrength(float strength) { m_blurStrength = strength; }
    float getBlurStrength() const { return m_blurStrength; }
    
    ~BlurLayer();
};