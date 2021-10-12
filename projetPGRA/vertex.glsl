attribute highp vec4 posAttr;
attribute lowp  vec4 colAttr;
attribute highp vec3 norAttr;
attribute mediump vec4 texAttr;
varying mediump vec4 texc;
varying lowp  vec4 col;
varying highp vec3 nor;
uniform highp mat4 projMatrix;  // projection matrix
uniform highp mat4 mvMatrix;    // model-view matrix
uniform highp mat3 norMatrix;   // normal matrix 3x3

void main() {
   texc = texAttr;
   col = colAttr;
   gl_Position = projMatrix * mvMatrix * posAttr;
   nor = norMatrix * norAttr;
}

