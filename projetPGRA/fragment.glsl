varying lowp  vec4 col;
varying highp vec3 nor;
varying mediump vec4 texc;
uniform sampler2D texture;

void main() {

    // Direction de lumière normalisée
    vec3 light = normalize(vec3(0.0, 0.0, 10.0));

    // Normale du fragment, normalisée
    vec3 nor3 = normalize(nor);

    // Cosinus de l'angle entre la normale et la lumière
    // borné entre 0 et 1
    //float cosTheta = clamp( dot(nor3,light), 0.0 , 1.0 );
    float cosTheta = clamp( dot(nor3,light ), 0.3 , 1.0 ); // lumière ambiante

    // Couleur de la lumière
    vec3 lightColor = vec3 (1.0, 1.0, 1.0);

    //gl_FragColor = vec4(nor3, 1.0);
    //gl_FragColor = vec4(lightColor * cosTheta, 1.0);
    gl_FragColor = vec4(lightColor * cosTheta, 1.0) * col;
    // pour afficher les textures; mais remplace les couleurs
    //gl_FragColor = texture2D(texture, texc.st);

//    vec3 ambiantLight = vec3 (0.3, 0.3, 0.3);
//    vec3 sumLight = lightColor * cosTheta + ambiantLight;
//    gl_FragColor = vec4(clamp(sumLight, 0.0, 1.0), 1.0) * col;
}

