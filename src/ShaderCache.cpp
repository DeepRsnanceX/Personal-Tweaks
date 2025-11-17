#include "ShaderCache.h"

const char* BLUR_FRAGMENT_SHADER = R"(
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D CC_Texture0;
uniform vec2 resolution;
uniform float blurRadius;

void main() {
    vec4 sum = vec4(0.0);
    vec2 tc = v_texCoord;
    
    float blur = blurRadius / resolution.x;
    
    // Gaussian blur
    sum += texture2D(CC_Texture0, vec2(tc.x - 4.0*blur, tc.y)) * 0.05;
    sum += texture2D(CC_Texture0, vec2(tc.x - 3.0*blur, tc.y)) * 0.09;
    sum += texture2D(CC_Texture0, vec2(tc.x - 2.0*blur, tc.y)) * 0.12;
    sum += texture2D(CC_Texture0, vec2(tc.x - blur, tc.y)) * 0.15;
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y)) * 0.16;
    sum += texture2D(CC_Texture0, vec2(tc.x + blur, tc.y)) * 0.15;
    sum += texture2D(CC_Texture0, vec2(tc.x + 2.0*blur, tc.y)) * 0.12;
    sum += texture2D(CC_Texture0, vec2(tc.x + 3.0*blur, tc.y)) * 0.09;
    sum += texture2D(CC_Texture0, vec2(tc.x + 4.0*blur, tc.y)) * 0.05;
    
    // Apply vertical blur
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y - 4.0*blur)) * 0.05;
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y - 3.0*blur)) * 0.09;
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y - 2.0*blur)) * 0.12;
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y - blur)) * 0.15;
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y + blur)) * 0.15;
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y + 2.0*blur)) * 0.12;
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y + 3.0*blur)) * 0.09;
    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y + 4.0*blur)) * 0.05;
    
    gl_FragColor = sum * 0.5 * v_fragmentColor;
}
)";

ShaderCache* ShaderCache::instance = nullptr;

void ShaderCache::init() {
    m_shaders = CCDictionary::create();
}

void ShaderCache::add(std::string name, CCGLProgram* program) {
    m_shaders->setObject(program, name);
}

CCGLProgram* ShaderCache::getProgram(std::string name) {
    return static_cast<CCGLProgram*>(m_shaders->objectForKey(name));
}
std::string ShaderCache::getVertex() {
    return m_vertex;
}

void ShaderCache::createShader(std::string name, std::string frag) {
    CCGLProgram* prg = new CCGLProgram();
    prg->initWithVertexShaderByteArray(getVertex().c_str(), frag.c_str());
    prg->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
    prg->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
    prg->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
    
    prg->autorelease();
    prg->link();
    prg->updateUniforms();
    add(name, prg);
}

