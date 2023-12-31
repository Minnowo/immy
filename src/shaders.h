
#ifndef DOKO_SHADERS_H
#define DOKO_SHADERS_H

#include "config.h"

#if(ENABLE_SHADERS == 1)

#define VALUE_TO_STRING(value) #value
#define STRINGIFY_MACRO(macro) VALUE_TO_STRING(macro)


#if (GLSL_VERSION == 330)

#define INVERT_AND_GRAYSCALE_SHADER_CODE                                       \
    "#version 330\n"                                                           \
    "in vec2 fragTexCoord;"                                                    \
    "in vec4 fragColor;"                                                       \
    "uniform sampler2D texture0;"                                              \
    "uniform vec4 colDiffuse;"                                                 \
    "uniform bool applyInvert;"                                                \
    "uniform bool applyGrayscale;"                                             \
    "out vec4 finalColor;"                                                     \
    "void main()"                                                              \
    "{"                                                                        \
    "vec4 tColor = texture(texture0, fragTexCoord) * colDiffuse * fragColor;"  \
    "vec3 color = tColor.rgb;"                                                 \
    "color = mix(color, 1.0 - color, float(applyInvert));"                     \
    "float gray = dot(color, vec3("                                            \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_R) ","                         \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_G) ","                         \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_B) " ));"                      \
    "color = mix(color, vec3(gray), float(applyGrayscale));"                   \
    "finalColor = vec4(color, tColor.a);"                                      \
    "}"

#elif(GLSL_VERSION == 120)

#define INVERT_AND_GRAYSCALE_SHADER_CODE                                       \
    "#version 120\n"                                                           \
    "varying vec2 fragTexCoord;"                                               \
    "varying vec4 fragColor;"                                                  \
    "uniform sampler2D texture0;"                                              \
    "uniform vec4 colDiffuse;"                                                 \
    "uniform bool applyInvert;"                                                \
    "uniform bool applyGrayscale;"                                             \
    "void main()"                                                              \
    "{"                                                                        \
    "vec4 tColor = texture2D(texture0, fragTexCoord)*colDiffuse*fragColor;"    \
    "vec3 color = tColor.rgb;"                                                 \
    "color = mix(color, 1.0 - color, float(applyInvert));"                     \
    "float gray = dot(color, vec3("                                            \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_R) ","                         \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_G) ","                         \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_B) " ));"                      \
    "color = mix(color, vec3(gray), float(applyGrayscale));"                   \
    "gl_FragColor = vec4(color, tColor.a);"                                    \
    "}"

#elif(GLSL_VERSION == 100)

#define INVERT_AND_GRAYSCALE_SHADER_CODE                                       \
    "#version 100\n"                                                           \
    "precision mediump float;"                                                 \
    "varying vec2 fragTexCoord;"                                               \
    "varying vec4 fragColor;"                                                  \
    "uniform sampler2D texture0;"                                              \
    "uniform vec4 colDiffuse;"                                                 \
    "uniform bool applyInvert;"                                                \
    "uniform bool applyGrayscale;"                                             \
    "void main()"                                                              \
    "{"                                                                        \
    "vec4 tColor = texture2D(texture0, fragTexCoord)*colDiffuse*fragColor;"    \
    "vec3 color = tColor.rgb;"                                                 \
    "color = mix(color, 1.0 - color, float(applyInvert));"                     \
    "float gray = dot(color, vec3("                                            \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_R) ","                         \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_G) ","                         \
                 STRINGIFY_MACRO(GRAYSCALE_COEF_B) " ));"                      \
    "color = mix(color, vec3(gray), float(applyGrayscale));"                   \
    "gl_FragColor = vec4(color, tColor.a);"                                    \
    "}" 

#endif

#endif

#endif