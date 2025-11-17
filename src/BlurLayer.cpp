#include "BlurLayer.hpp"

// Sarah cuando le preguntan si ella hizo todo esto:
//          no             xdxdxd   xd
// credits to TheSillyDoggo's Blur-BG mod (More specifically Naxrin's fork)
// i realize this mod is just stolen shaders galore
// eh whatever
// it's not released officially for a reason /shrug

static const char* VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    TexCoords = aTexCoords;
}
)";

static const char* FRAGMENT_SHADER = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screen;
uniform vec2 screenSize;
uniform bool first;
uniform float radius;

// Optimized Gaussian weights for 5-tap blur
const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 texelSize = 1.0 / screenSize;
    vec3 result = texture(screen, TexCoords).rgb * weights[0];
    
    // Separable blur - only blur in one direction per pass
    if (first) {
        // Horizontal pass
        for (int i = 1; i < 5; i++) {
            result += texture(screen, TexCoords + vec2(texelSize.x * float(i) * radius, 0.0)).rgb * weights[i];
            result += texture(screen, TexCoords - vec2(texelSize.x * float(i) * radius, 0.0)).rgb * weights[i];
        }
    } else {
        // Vertical pass
        for (int i = 1; i < 5; i++) {
            result += texture(screen, TexCoords + vec2(0.0, texelSize.y * float(i) * radius)).rgb * weights[i];
            result += texture(screen, TexCoords - vec2(0.0, texelSize.y * float(i) * radius)).rgb * weights[i];
        }
    }
    
    FragColor = vec4(result, 1.0);
}
)";

BlurLayer* BlurLayer::create() {
    auto ret = new BlurLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool BlurLayer::init() {
    if (!CCLayerColor::init())
        return false;
    
    if (!s_initialized) {
        auto size = CCDirector::get()->getOpenGLView()->getFrameSize();
        setupRenderTextures((GLsizei)size.width, (GLsizei)size.height);
        setupShaders();
        s_initialized = true;
    }
    
    this->setOpacity(0);
    
    return true;
}

void BlurLayer::visit() {
    if (this->getOpacity() > 0) {
        m_blurStrength = this->getOpacity() / 255.0f;
    } else {
        m_blurStrength = 0.0f;
    }
    
    CCLayerColor::visit();
}

void BlurLayer::draw() {
    if (m_blurStrength == 0.0f || !s_initialized)
        return CCLayerColor::draw();
    
    GLint drawFbo = 0;
    GLint readFbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFbo);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFbo);
    
    glBindFramebuffer(GL_FRAMEBUFFER, s_rt0.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    CCLayerColor::draw();
    
    getParent()->setVisible(false);
    
    CCNode* scene = CCScene::get();
    if (scene && !m_visiting) {
        m_visiting = true;
        
        kmGLPushMatrix();
        scene->transform();
        scene->visit();
        kmGLPopMatrix();
        
        m_visiting = false;
    }
    
    getParent()->setVisible(true);
    
    glBindVertexArray(s_vao);
    ccGLUseProgram(s_program);
    
    float adjustedStrength = m_blurStrength * 3.5f; // STRENGTH
    glUniform1f(s_uniformRadius, adjustedStrength);
    
    glBindFramebuffer(GL_FRAMEBUFFER, s_rt1.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glUniform1i(s_uniformFirst, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, s_rt0.tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
    glUniform1i(s_uniformFirst, GL_FALSE);
    glBindTexture(GL_TEXTURE_2D, s_rt1.tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindVertexArray(0);
}

void BlurLayer::setupShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VERTEX_SHADER, nullptr);
    glCompileShader(vertexShader);
    
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        log::error("Vertex shader compilation failed: {}", infoLog);
        return;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER, nullptr);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        log::error("Fragment shader compilation failed: {}", infoLog);
        return;
    }
    
    s_program = glCreateProgram();
    glAttachShader(s_program, vertexShader);
    glAttachShader(s_program, fragmentShader);
    glBindAttribLocation(s_program, 0, "aPosition");
    glBindAttribLocation(s_program, 1, "aTexCoords");
    glLinkProgram(s_program);
    
    glGetProgramiv(s_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(s_program, 512, nullptr, infoLog);
        log::error("Shader program linking failed: {}", infoLog);
        return;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    auto size = CCDirector::get()->getOpenGLView()->getFrameSize();
    ccGLUseProgram(s_program);
    glUniform1i(glGetUniformLocation(s_program, "screen"), 0);
    glUniform2f(glGetUniformLocation(s_program, "screenSize"), size.width, size.height);
    s_uniformFast = glGetUniformLocation(s_program, "fast");
    s_uniformFirst = glGetUniformLocation(s_program, "first");
    s_uniformRadius = glGetUniformLocation(s_program, "radius");
    
    GLfloat vertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &s_vao);
    glGenBuffers(1, &s_vbo);
    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);
    
    log::info("Blur shaders initialized successfully");
}

void BlurLayer::setupRenderTextures(GLsizei width, GLsizei height) {
    auto setupRT = [](auto& rt, GLsizei w, GLsizei h) {
        GLint oldFbo = 0;
        GLint oldRbo = 0;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldFbo);
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRbo);
        
        glGenFramebuffers(1, &rt.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, rt.fbo);
        
        glGenTextures(1, &rt.tex);
        glBindTexture(GL_TEXTURE_2D, rt.tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt.tex, 0);
        
        glGenRenderbuffers(1, &rt.rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rt.rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rt.rbo);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            log::error("Framebuffer not complete!");
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
        glBindRenderbuffer(GL_RENDERBUFFER, oldRbo);
    };
    
    setupRT(s_rt0, width, height);
    setupRT(s_rt1, width, height);
}

void BlurLayer::cleanupShaders() {
    if (s_vao) glDeleteVertexArrays(1, &s_vao);
    if (s_vbo) glDeleteBuffers(1, &s_vbo);
    if (s_program) glDeleteProgram(s_program);
    
    s_vao = 0;
    s_vbo = 0;
    s_program = 0;
    s_uniformFast = 0;
    s_uniformFirst = 0;
    s_uniformRadius = 0;
}

void BlurLayer::cleanupRenderTextures() {
    auto cleanupRT = [](auto& rt) {
        if (rt.fbo) glDeleteFramebuffers(1, &rt.fbo);
        if (rt.tex) glDeleteTextures(1, &rt.tex);
        if (rt.rbo) glDeleteRenderbuffers(1, &rt.rbo);
        rt.fbo = 0;
        rt.tex = 0;
        rt.rbo = 0;
    };
    
    cleanupRT(s_rt0);
    cleanupRT(s_rt1);
}

BlurLayer::~BlurLayer() {
    // si
}