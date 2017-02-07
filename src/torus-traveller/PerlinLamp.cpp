//
// Created by mattw on 2/5/17.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PerlinLamp.h"
#include "../core/loaders/loadObjIndexed.h"
#include "../tube-traveller/texture-generators/Noise.h"
#include "../core/vboIndexer.h"

PerlinLamp::PerlinLamp(GLuint shaderID_, GLuint fboID_) {

    // program ID associated with shader that implements light
    shaderID = shaderID_;

    // ID of framebuffer that light will get rendered to
    fboID = fboID_;

    // -------------------------------------------------------------------------
    //                  Create light properties struct
    // -------------------------------------------------------------------------

    changeIntensity = false;
    changeHue = true;

    // define light properties for ALL types - directional, point, spotlight
    lightProps.type = LIGHTTYPE_DIR;
    lightProps.lampColorMax = glm::vec3(1.f, 1.f, 1.f);
    lightProps.cameraPosition = glm::vec3(1.f);

    // for all types
    lightProps.position = glm::vec3(1.f);
    lightProps.direction = glm::vec3(0.f);
    lightProps.ambientMax = glm::vec3(0.8) * lightProps.lampColorMax;
    lightProps.diffuseMax = glm::vec3(1.0) * lightProps.lampColorMax;
    lightProps.specularMax = glm::vec3(0.05);

    // for point and spotlight attenuation
    lightProps.attLin = 0.022f;  // vals from https://learnopengl.com/#!Lighting/Materials
    lightProps.attQuad = 0.0019f;

    // for spotlight
    lightProps.innerTheta = 0.f; // cos of angle
    lightProps.outerTheta = 1.f; // cos of angle
    lightProps.intensity = glm::vec3(1.f);

    // -------------------------------------------------------------------------
    //                        Create lamp
    // -------------------------------------------------------------------------

    // create and compile our GLSL program from the shaders
    lampShader = new Shader("src/torus-traveller/PerlinLampShader.vert",
                            "src/torus-traveller/PerlinLampShader.frag");

    // give the MVP matrix to GLSL; get a handle on our uniforms
    mMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 5.f, 0.f));
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(lampShader->programID, "mMatrix");
    vpMatrixID = glGetUniformLocation(lampShader->programID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(lampShader->programID, "mvpMatrix");

    threshold = 1.0;
    thresholdID = glGetUniformLocation(lampShader->programID, "threshold");
    lampColorID = glGetUniformLocation(lampShader->programID, "lampColor");

    // load sphere model
    useIndexing = true;
    bool res = loadObjIndexed("data/obj/ico_sphere_4.obj",
                              vertices, uvs, normals);

    if (useIndexing) {
        // switch to indexing
        std::vector<GLushort> indxs;

        // copy data into temp vectors
        std::vector<glm::vec3> tempVertices = vertices;
        std::vector<glm::vec2> tempUVs = uvs;
        std::vector<glm::vec3> tempNormals = normals;

        // clear original vectors
        std::vector<glm::vec3>().swap(vertices);
        std::vector<glm::vec2>().swap(uvs);
        std::vector<glm::vec3>().swap(normals);

        // refill original vectors
        indexVBO(tempVertices, tempUVs, tempNormals,
                 indxs, vertices, uvs, normals);

        glGenBuffers(1, &elementBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxs.size() * sizeof(GLushort),
                     &indxs[0], GL_STATIC_DRAW);

        numIndxsPerElement = static_cast<GLint>(indxs.size());
    }

    numVerticesPerInstance = static_cast<GLuint>(vertices.size());

    // get updated uv coordinates
    for (int i = 0; i < numVerticesPerInstance; ++i) {

        GLfloat radius, theta, phi;
        radius = 1.f;
        theta = atan2(vertices[i].y, vertices[i].x) / (2 * PI) + 0.5f;
        phi = acos(vertices[i].z / radius) / (PI);
        uvs[i] = glm::vec2(theta, phi);

        std::cout << theta << " " << phi << std::endl;

    }

    // -------------------------------------------------------------------------
    //                   Create perlin noise texture
    // -------------------------------------------------------------------------

    // get an ID for our texture uniform
    samplerID = glGetUniformLocation(shaderID, "loadedTexture");

    // get perlin noise mapping
    generateTexture();


    // -------------------------------------------------------------------------
    //                        Create Lamp VAO
    // -------------------------------------------------------------------------
    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // LAMP MODEL COORDINATES
    // generate 1 buffer, put the resulting identifier in LvertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 &vertices[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,         // attribute 0; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // LAMP UVs
    // generate 1 buffer, put the resulting identifier in LvertexBufferID
    glGenBuffers(1, &uvBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numVerticesPerInstance,
                 &uvs[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 1; must match "layout" in shader
            2,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // free up memory by swapping vectors with empty temporary vectors
    std::vector<glm::vec3>().swap(vertices);
    std::vector<glm::vec2>().swap(uvs);
    std::vector<glm::vec3>().swap(normals);

}

// update
void PerlinLamp::update(Camera &cam) {

    // update lamp position
    lightProps.cameraPosition = cam.getPosition();
    lightProps.position = glm::vec3(0.f, 10.f, 0.f);   // !!!!needs updating
    lightProps.direction = glm::vec3(1.f, -1.f, -1.f);

    // update light uniforms
//    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;
    threshold = 0.5f + 0.5*sin(0.5 * glfwGetTime());


    // update light properties
    if (changeIntensity) {
        lightProps.intensity = glm::vec3(0.7f + 0.3*sin(glfwGetTime()));
    } else {
        lightProps.intensity = glm::vec3(1.f);
    }
    if (changeHue) {
        float r, g, b;
        Noise::hsvToRgb(static_cast<float>(0.5+0.5*sin(0.3*glfwGetTime())),
                        1.f, 1.f, &r, &g, &b);
        lightProps.lampColorMax = glm::vec3(r, g, b);
        lightProps.ambientMax = glm::vec3(1.0) * lightProps.lampColorMax;
        lightProps.diffuseMax = glm::vec3(1.0) * lightProps.lampColorMax;
        lightProps.specularMax = glm::vec3(1.0) * lightProps.lampColorMax;
    }

    lightProps.lampColor = lightProps.lampColorMax * lightProps.intensity;
    lightProps.ambient = lightProps.ambientMax * lightProps.intensity;
    lightProps.diffuse = lightProps.diffuseMax * lightProps.intensity;
    lightProps.specular = lightProps.specularMax * lightProps.intensity;

}

void PerlinLamp::setUniforms() {

    // material properties loaded by PerlinBlockIsland object

    // send properites to shader program
    glUniform3fv(glGetUniformLocation(shaderID, "cameraPosition"),
                 1, &lightProps.cameraPosition[0]);

    switch (lightProps.type) {
        case LIGHTTYPE_DIR:
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.direction"),
                         1, &lightProps.direction[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.ambient"),
                         1, &lightProps.ambient[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.diffuse"),
                         1, &lightProps.diffuse[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.specular"),
                         1, &lightProps.specular[0]);
            break;
        case LIGHTTYPE_POINT:
            glUniform3fv(glGetUniformLocation(shaderID, "pointLight.position"),
                         1, &lightProps.position[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "pointLight.ambient"),
                         1, &lightProps.ambient[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "pointLight.diffuse"),
                         1, &lightProps.diffuse[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "pointLight.specular"),
                         1, &lightProps.specular[0]);
            glUniform1f(glGetUniformLocation(shaderID, "pointLight.lin"),
                        lightProps.attLin);
            glUniform1f(glGetUniformLocation(shaderID, "pointLight.quad"),
                        lightProps.attQuad);
            break;
        case LIGHTTYPE_SPOT:
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.position"),
                         1, &lightProps.position[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.direction"),
                         1, &lightProps.direction[0]);
            glUniform1f(glGetUniformLocation(shaderID, "spotLight.innerTheta"),
                        lightProps.innerTheta);
            glUniform1f(glGetUniformLocation(shaderID, "spotLight.outerTheta"),
                        lightProps.outerTheta);
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.ambient"),
                         1, &lightProps.ambient[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.diffuse"),
                         1, &lightProps.diffuse[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.specular"),
                         1, &lightProps.specular[0]);
            glUniform1f(glGetUniformLocation(shaderID, "spotLight.lin"),
                        lightProps.attLin);
            glUniform1f(glGetUniformLocation(shaderID, "spotLight.quad"),
                        lightProps.attQuad);
            break;
        default: // directional light
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.direction"),
                         1, &lightProps.direction[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.ambient"),
                         1, &lightProps.ambient[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.diffuse"),
                         1, &lightProps.diffuse[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.specular"),
                         1, &lightProps.specular[0]);
    }

}

// draw lamp
void PerlinLamp::draw() {

    // light model properties
    // use our shader (makes programID "currently bound" shader?)
    lampShader->use();

    // send uniforms to the currently bound shader
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);

    glUniform1f(thresholdID, threshold);
    glUniform3fv(lampColorID, 1, &lightProps.lampColor[0]);

    // bind vertex array
    glBindVertexArray(vertexArrayID);

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0);
    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, textureID);
    // puts the texture in texture unit 0
    glUniform1i(samplerID, 0);

    // draw arrays using currently active shaders
    if (useIndexing) {
        // bind element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
        glDrawElements(GL_TRIANGLES, numIndxsPerElement, GL_UNSIGNED_SHORT,
                       (void*)0);
        // break element array buffer binding
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, numVerticesPerInstance);
    }
    // break vertex array object binding
    glBindVertexArray(0);

}

// clean up VAOs, VBOs, etc.
void PerlinLamp::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &elementBufferID);
    glDeleteBuffers(1, &textureID);

    lampShader->clean();
    delete lampShader;

}

void PerlinLamp::generateTexture() {

    int height = 128;
    int width = 128;

    GLubyte image[height][width][4];

    float hue = 1;
    float sat = 1;
    float val = 1;
    float r, g, b;
    GLubyte red, green, blue;
    GLubyte alpha = 255;

    // set up perlin noise
    int seed = 236;
    PerlinNoise pn = PerlinNoise(seed);
    float pnScale = 10.f;

    bool woodGrain = false;

    double x, y;
    float maxHue = 0.f;
    float minHue = 1.f;
    for (int j = 0; j < height; ++j) {

        // get current image coordinates
        x = ((double) j) / ((double) width);

        for (int k = 0; k < width; ++k) {

            // get current image coordinates
            y = ((double) k) / ((double) width);

            // get corresponding perlin noise value
            hue = static_cast<float>(pn.noise(pnScale*x, pnScale*y, 1.8));
            if (woodGrain) {
                // first val must be >= second val
                // first val controls how much detail there is
                hue = 10.f * hue - floor(10.f * hue); // wood grain
            }

            if (hue > maxHue) {
                maxHue = hue;
            }
            if (hue < minHue) {
                minHue = hue;
            }

            // get rgb values
            red   = (GLubyte) floor(255 * hue);
            green = (GLubyte) floor(255 * hue);
            blue  = (GLubyte) floor(255 * hue);

            // fill image
            image[j][k][0] = red;
            image[j][k][1] = green;
            image[j][k][2] = blue;
            image[j][k][3] = alpha;

        }
    }

    // renormalize
    maxHue = 255 * (maxHue - minHue);
    minHue = 255 * minHue;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < 4; ++k) {
                image[i][j][k] = static_cast<GLubyte>(
                        ((image[i][j][k] - minHue) / maxHue) * 255);
            }
        }
    }

    // generate 1 texture ID, put the resutling identifier in textureID
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    // bind newly created texture to GL_TEXTURE_2D target; subsequent texture
    // commands will configure the currently bound texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // generate the texture by using the previously loaded image data
    glTexImage2D(
            GL_TEXTURE_2D,     // texture target; will gen texture on textureIDs[i]
            0,                 // mipmap level; use base of 0
            GL_RGBA,           // type of format we want to store the texture
            width,
            height,
            0,                 // legacy bs
            GL_RGBA,           // format of source image
            GL_UNSIGNED_BYTE,  // format of source image
            image              // source image
    );
    // textureID now has texture image attached to it
    // glGenerateMipmap here if desired

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // unbind the texture object
    glBindTexture(GL_TEXTURE_2D, 0);

}