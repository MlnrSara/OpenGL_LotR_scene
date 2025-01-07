#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtx/string_cast.hpp>

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "Skybox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.75f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
/*gps::Camera myCamera(
    glm::vec3(6.8549f, 0.75f, -38.671f),
    glm::vec3(7.0143f, 1.0f, -34.929f),
    glm::vec3(0.0f, 1.0f, 0.0f));*/

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D shireLandscape;
gps::Model3D shireObjects;
gps::Model3D screenQuad;
gps::Model3D shireAnimals;
gps::Model3D stingSword;
gps::Model3D goose;

gps::Model3D mordorLandscape;
gps::Model3D mordorEye;
GLfloat angle;

// shaders
gps::Shader myBasicShader;
gps::Shader depthMapShader;
gps::Shader screenQuadShader;
gps::Shader nightShader;
gps::Shader skyboxShader;
gps::Shader mordorShader;
gps::Shader lavaShader;

//misc
double mouseX;
double mouseY;

float pitch = 0.0f;
float yaw = 0.0f;

bool line = true;

//shadow params
const int numCascades = 4;
GLuint shadowMapFBOs[numCascades];
GLuint shadowMaps[numCascades];
const int shadowMapResolution = 2048;
GLuint depthMapTexture;
bool showDepthMap;

//ring coordinates -> x=-6.45 y=0.7 z=0.06 blender
const glm::vec3 shireRingPos = glm::vec3(-6.45f, 0.06f, -0.7f);
const glm::vec3 mordorRingPos = glm::vec3(-10.289f, 0.22796f, -9.9485f);

//houses locations
const glm::vec3 bilboHousePos = glm::vec3(7.8661f, 0.62342f, -10.776f);
const glm::vec3 otherHouse1Pos = glm::vec3(-10.168f, 0.45317f, -17.958f);
const glm::vec3 otherHouse2Pos = glm::vec3(-27.71f, 0.54547f, -7.9438f);
const glm::vec3 otherHouse3Pos = glm::vec3(-7.311f, 0.78819f, -3.6853f);

//positional lights
//bilbo house
const glm::vec3 bilboHouseL1 = glm::vec3(7.5744f, 0.75403f, -11.134f);
const glm::vec3 bilboHouseL2 = glm::vec3(8.0534f, 0.77755f, -10.399f);
const glm::vec3 bilboHouseL3 = glm::vec3(8.5849f, 0.70257f, -10.234f);
//other house
const glm::vec3 otherHouseL1 = glm::vec3(-10.572f, 0.64958f, -17.333f);
const glm::vec3 otherHouseL2 = glm::vec3(-9.7475f, 0.68888f, -18.351f);
const glm::vec3 otherHouseL3 = glm::vec3(-9.7503f, 0.55404f, -19.177f);
glm::vec3 eyeLightPos = glm::vec3(-14.784f, 19.94f, -16.68f);
//positional light color
const glm::vec3 positionalLightColor = glm::vec3(1.0f, 0.8f, 0.0f); //hopefully yellow
//locations
glm::vec3 eyeLightColor;
GLint bilboHouseL1Location;
GLint bilboHouseL2Location;
GLint bilboHouseL3Location;
GLint otherHouseL1Location;
GLint otherHouseL2Location;
GLint otherHouseL3Location;
GLint positionalLightColorLocation;
GLint eyeLightPosLocation;

bool night = false;

//translation vars
float hoverSpeed = 50.0f;
float currentYPos = 0.0f;

float rotationSpeed = 1600.0f;
float gooseAngle = 0.0f;
float shyRotationSpeed = 800.0f;
float gooseShyAngle = 0.0f;

float eyeTranslateSpeed = 0.005f;
float eyeCurrentPos = 0.0f;

//skyboxes
gps::SkyBox daySkybox;
gps::SkyBox nightSkybox;
gps::SkyBox mordorSkybox;

//camera animation
bool presenting = false;
int firstRotation = 0;
int transitionForwardFirst = 0;
int secondRotation = 0;
int lookDown = 0;
int thirdRotation = 0;
int transitionForwardSecond = 0;
int forthRotation = 0;
int transitionForwardThird = 0;
int fifthRotation = 0;
int transitionForwardForth = 0;
int sixthRotation = 0;
int transitionForwardFifth = 0;
int seventhRotation = 0;

//change scenes
bool shire = true;

//ashfall
float offsetY = 0.0f;
float speed = 0.1f; // Speed of the ashfall
unsigned int ashTexture;

// grid
const float GRID_WIDTH = 20.0f;
const float GRID_HEIGHT = 20.0f;
const int GRID_NUM_POINTS_WIDTH = 50;
const int GRID_NUM_POINTS_HEIGHT = 50;

//VBO, EBO and VAO
GLuint gridPointsVBO;
GLuint gridTrianglesEBO;
GLuint gridVAO;

//texture
const float textureRepeatU = 2.0f; //number of times to repeat seamless texture on u axis
const float textureRepeatV = 2.0f; //number of times to repeat seamless texture on v axis
GLuint gridTexture;
GLint gridTextureLoc;

//simulation time
float simTime;
GLint simTimeLoc;

double lastTime = glfwGetTime();

void initAngles() {
    
    glm::mat4 auxMatrix = myCamera.getViewMatrix();
    yaw = -glm::degrees(atan2(-auxMatrix[2][0], auxMatrix[2][2]));
    pitch = -glm::degrees(asin(auxMatrix[2][1]));
}

// Load the texture
GLuint initTexture(const char* file_name) {

    int x, y, n;
    int force_channels = 4;
    unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", file_name);
        return false;
    }
    // NPOT check
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf(
            stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
        );
    }

    int width_in_bytes = x * 4;
    unsigned char* top = NULL;
    unsigned char* bottom = NULL;
    unsigned char temp = 0;
    int half_height = y / 2;

    for (int row = 0; row < half_height; row++) {
        top = image_data + row * width_in_bytes;
        bottom = image_data + (y - row - 1) * width_in_bytes;
        for (int col = 0; col < width_in_bytes; col++) {
            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top++;
            bottom++;
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_SRGB, //GL_SRGB,//GL_RGBA,
        x,
        y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void initVBOs() {

    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);

    //prepare vertex data to send to shader
    static GLfloat vertexData[GRID_NUM_POINTS_WIDTH * GRID_NUM_POINTS_HEIGHT * 4];

    //for each vertex in grid
    for (unsigned int i = 0; i < GRID_NUM_POINTS_HEIGHT; i++) {
        for (unsigned int j = 0; j < GRID_NUM_POINTS_WIDTH; j++) {

            //tex coords
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 0] = j * textureRepeatU / (float)(GRID_NUM_POINTS_WIDTH - 1);
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 1] = textureRepeatV - i * textureRepeatV / (float)(GRID_NUM_POINTS_HEIGHT - 1);
            //xy position indices in grid (for computing sine function)
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 2] = (float)j;
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 3] = (float)i;
        }
    }

    glGenBuffers(1, &gridPointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridPointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    //prepare triangle indices to send to shader
    static GLuint triangleIndices[(GRID_NUM_POINTS_WIDTH - 1) * (GRID_NUM_POINTS_HEIGHT - 1) * 2 * 3];

    //for each square/rectangle in grid (each four neighboring points)
    for (unsigned int i = 0; i < GRID_NUM_POINTS_HEIGHT - 1; i++) {
        for (unsigned int j = 0; j < GRID_NUM_POINTS_WIDTH - 1; j++) {

            //lower triangle
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j)] = i * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 1] = (i + 1) * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 2] = (i + 1) * GRID_NUM_POINTS_WIDTH + j + 1;

            //upper triangle
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 3] = i * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 4] = (i + 1) * GRID_NUM_POINTS_WIDTH + j + 1;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 5] = i * GRID_NUM_POINTS_WIDTH + j + 1;
        }
    }

    glGenBuffers(1, &gridTrianglesEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridTrianglesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndices), triangleIndices, GL_STATIC_DRAW);

    //split vertex attributes

    //tex coords
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    //grid XY indices
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}

float getCameraDistance(glm::vec3 target) {
    glm::vec3 cameraPos = myCamera.getCameraPosition();
    float distance = sqrt((target.x - cameraPos.x) * (target.x - cameraPos.x) + (target.y - cameraPos.y) * (target.y - cameraPos.y) + (target.z - cameraPos.z) * (target.z - cameraPos.z));
    return distance;
}

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    //TODO
    WindowDimensions wd;
    glfwGetFramebufferSize(myWindow.getWindow(), &wd.width, &wd.height);
    myWindow.setWindowDimensions(wd);

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)wd.width / (float)wd.height,
        0.1f, 50.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glViewport(0, 0, wd.width, wd.height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        if (line) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            line = false;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            line = true;
        }

    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        if (showDepthMap) {
            showDepthMap = false;
        }
        else {
            showDepthMap = true;
        }

    }

    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        if (myCamera.getPlaneBindVar()) {
            myCamera.setPlaneBindVar(false);
        }
        else {
            myCamera.setPlaneBindVar(true);
        }

    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        if (!night) {
            nightShader.useShaderProgram();
            viewLoc = glGetUniformLocation(nightShader.shaderProgram, "view");
            modelLoc = glGetUniformLocation(nightShader.shaderProgram, "model");
            normalMatrixLoc = glGetUniformLocation(nightShader.shaderProgram, "normalMatrix");
            projectionLoc = glGetUniformLocation(nightShader.shaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            lightDirLoc = glGetUniformLocation(nightShader.shaderProgram, "lightDir");
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
            lightColor = glm::vec3(0.0f, 0.0f, 1.0f);
            lightColorLoc = glGetUniformLocation(nightShader.shaderProgram, "lightColor");
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

            glUniform3fv(positionalLightColorLocation, 1, glm::value_ptr(positionalLightColor));
            
            glUniform3fv(bilboHouseL1Location, 1, glm::value_ptr(bilboHouseL1));
            glUniform3fv(bilboHouseL2Location, 1, glm::value_ptr(bilboHouseL2));
            glUniform3fv(bilboHouseL3Location, 1, glm::value_ptr(bilboHouseL3));
            glUniform3fv(otherHouseL1Location, 1, glm::value_ptr(otherHouseL1));
            glUniform3fv(otherHouseL2Location, 1, glm::value_ptr(otherHouseL2));
            glUniform3fv(otherHouseL3Location, 1, glm::value_ptr(otherHouseL3));
        }
        else {
            myBasicShader.useShaderProgram();
            viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
            modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
            normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
            projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection"); 
            lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
            lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        }
        night = !night;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
    if (!presenting) {
        float yDisplacement = mouseY - ypos;
        float xDisplacement = xpos - mouseX;

        mouseX = xpos;
        mouseY = ypos;

        float sensitivity = 0.1f;
        yDisplacement *= sensitivity;
        xDisplacement *= sensitivity;

        yaw += xDisplacement;
        pitch += yDisplacement;

        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        else if (pitch < -89.0f) {
            pitch = -89.0f;
        }
        myCamera.rotate(pitch, yaw);
    }
}

void processMovement() {
    if (!presenting) {
        if (pressedKeys[GLFW_KEY_W]) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
            if (shire && (getCameraDistance(bilboHousePos) < 3.0f || getCameraDistance(otherHouse1Pos) < 3.0f || getCameraDistance(otherHouse2Pos) < 3.0f || getCameraDistance(otherHouse3Pos) < 3.0f)) {
                myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
            }
            //update view matrix
            view = myCamera.getViewMatrix();
            if (!night) {
                myBasicShader.useShaderProgram();
            }
            else {
                nightShader.useShaderProgram();
            }
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_S]) {
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
            if (shire && (getCameraDistance(bilboHousePos) < 3.0f || getCameraDistance(otherHouse1Pos) < 3.0f || getCameraDistance(otherHouse2Pos) < 3.0f || getCameraDistance(otherHouse3Pos) < 3.0f)) {
                myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
            }
            //update view matrix
            view = myCamera.getViewMatrix();
            if (!night) {
                myBasicShader.useShaderProgram();
            }
            else {
                nightShader.useShaderProgram();
            }
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_A]) {
            myCamera.move(gps::MOVE_LEFT, cameraSpeed);
            if (shire && (getCameraDistance(bilboHousePos) < 3.0f || getCameraDistance(otherHouse1Pos) < 3.0f || getCameraDistance(otherHouse2Pos) < 3.0f || getCameraDistance(otherHouse3Pos) < 3.0f)) {
                myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
            }
            //update view matrix
            view = myCamera.getViewMatrix();
            if (!night) {
                myBasicShader.useShaderProgram();
            }
            else {
                nightShader.useShaderProgram();
            }
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_D]) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
            if (shire && (getCameraDistance(bilboHousePos) < 3.0f || getCameraDistance(otherHouse1Pos) < 3.0f || getCameraDistance(otherHouse2Pos) < 3.0f || getCameraDistance(otherHouse3Pos) < 3.0f)) {
                myCamera.move(gps::MOVE_LEFT, cameraSpeed);
            }
            //update view matrix
            view = myCamera.getViewMatrix();
            if (!night) {
                myBasicShader.useShaderProgram();
            }
            else {
                nightShader.useShaderProgram();
            }
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_Q]) {
            angle -= 1.0f;
            // update model matrix for teapot
            model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
            // update normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_E]) {
            angle += 1.0f;
            // update model matrix for teapot
            model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
            // update normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }
    }
   
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "The Shire");
    glfwGetCursorPos(myWindow.getWindow(), &mouseX, &mouseY);
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    //glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    teapot.LoadModel("models/teapot/teapot20segUT.obj");
    shireLandscape.LoadModel("models/shire/shireLandscape.obj");
    shireObjects.LoadModel("models/shire/shireObjects.obj");
    shireAnimals.LoadModel("models/shire/shireAnimals.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    stingSword.LoadModel("models/shire/sting.obj");
    goose.LoadModel("models/shire/goose.obj");
    mordorLandscape.LoadModel("models/mordor/mordorLandscape.obj");
    mordorEye.LoadModel("models/mordor/mordorEye.obj");
}

void initShaders() {
    myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");

    depthMapShader.loadShader("shaders/depth.vert", "shaders/depth.frag");
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    nightShader.loadShader("shaders/night.vert", "shaders/night.frag");
    mordorShader.loadShader("shaders/mordorShader.vert", "shaders/mordorShader.frag");
    lavaShader.loadShader("shaders/basicWave.vert", "shaders/basicWave.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}

void initMordorUniforms() {
    /*screenQuadShader.useShaderProgram();
    ashTexture = loadTexture("models/mordor/dust_text.png");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ashTexture);
    glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
    mordorShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(mordorShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(mordorShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(mordorShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 40.0f);
    projectionLoc = glGetUniformLocation(mordorShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDir = glm::normalize(lightDir);
    lightDirLoc = glGetUniformLocation(mordorShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 0.2f, 0.1f); //redish light
    lightColorLoc = glGetUniformLocation(mordorShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    eyeLightPosLocation = glGetUniformLocation(mordorShader.shaderProgram, "eyeLightPos");
    glUniform3fv(eyeLightPosLocation, 1, glm::value_ptr(eyeLightPos));

    eyeLightColor = glm::vec3(1.0f, 0.8f, 0.0f);

    positionalLightColorLocation = glGetUniformLocation(mordorShader.shaderProgram, "positionalLightColor");
    glUniform3fv(positionalLightColorLocation, 1, glm::value_ptr(eyeLightColor));

    lavaShader.useShaderProgram();
    // create model matrix for grid
    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(lavaShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    // send view matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(lavaShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix grid
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 50.0f);
    // send projection matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(lavaShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // send light dir to shader
    glUniform3fv(glGetUniformLocation(lavaShader.shaderProgram, "lightDir"), 1, glm::value_ptr(glm::vec3(glm::inverseTranspose(view) * glm::vec4(lightDir, 1.0f))));

    // send light color to shader
    glUniform3fv(glGetUniformLocation(lavaShader.shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

    gridTextureLoc = glGetUniformLocation(lavaShader.shaderProgram, "diffuseTexture");
    std::cout << gridTextureLoc << std::endl;
    glm::vec2 gridSize{ GRID_NUM_POINTS_WIDTH, GRID_NUM_POINTS_HEIGHT };
    glUniform2fv(glGetUniformLocation(lavaShader.shaderProgram, "gridSize"), 1, glm::value_ptr(gridSize));
    glm::vec2 gridDimensions{ GRID_WIDTH, GRID_HEIGHT };
    glUniform2fv(glGetUniformLocation(lavaShader.shaderProgram, "gridDimensions"), 1, glm::value_ptr(gridDimensions));

    simTimeLoc = glGetUniformLocation(lavaShader.shaderProgram, "time");
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 40.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDir = glm::normalize(lightDir);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    nightShader.useShaderProgram();
    bilboHouseL1Location = glGetUniformLocation(nightShader.shaderProgram, "bilboL1Pos");
    bilboHouseL2Location = glGetUniformLocation(nightShader.shaderProgram, "bilboL2Pos");
    bilboHouseL3Location = glGetUniformLocation(nightShader.shaderProgram, "bilboL3Pos");
    otherHouseL1Location = glGetUniformLocation(nightShader.shaderProgram, "otherL1Pos");
    otherHouseL2Location = glGetUniformLocation(nightShader.shaderProgram, "otherL2Pos");
    otherHouseL3Location = glGetUniformLocation(nightShader.shaderProgram, "otherL3Pos");
    positionalLightColorLocation = glGetUniformLocation(nightShader.shaderProgram, "positionalLightColor");
}

void initSkybox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");
    daySkybox.Load(faces);

    std::vector<const GLchar*> nightFaces;
    nightFaces.push_back("nightsky/nightsky_rt.tga");
    nightFaces.push_back("nightsky/nightsky_lf.tga");
    nightFaces.push_back("nightsky/nightsky_up.tga");
    nightFaces.push_back("nightsky/nightsky_dn.tga");
    nightFaces.push_back("nightsky/nightsky_bk.tga");
    nightFaces.push_back("nightsky/nightsky_ft.tga");
    nightSkybox.Load(nightFaces);

    std::vector<const GLchar*> mordorFaces;
    mordorFaces.push_back("violentdays/violentdays_rt.tga");
    mordorFaces.push_back("violentdays/violentdays_lf.tga");
    mordorFaces.push_back("violentdays/violentdays_up.tga");
    mordorFaces.push_back("violentdays/violentdays_dn.tga");
    mordorFaces.push_back("violentdays/violentdays_bk.tga");
    mordorFaces.push_back("violentdays/violentdays_ft.tga");
    mordorSkybox.Load(mordorFaces);
}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
    shireLandscape.Draw(shader);
    //shireObjects.Draw(shader);
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    for (int i = 0; i < numCascades-1; i++) {
        glGenFramebuffers(1, &shadowMapFBOs[i]);
        glGenTextures(1, &shadowMaps[i]);

        glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBOs[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Error: Shadow map framebuffer not complete for cascade " << i << std::endl;
        }
    }
}

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    const auto inv = glm::inverse(proj * view);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt =
                    inv * glm::vec4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        2.0f * z - 1.0f,
                        1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

glm::mat4 getLightViewMatrix(std::vector<glm::vec4> corners) {
    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(
        center + lightDir,
        center,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    return lightView;
}

glm::mat4 getLightProjectionMatrix(std::vector<glm::vec4> corners, glm::mat4 lightView) {
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection;
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    
    std::vector<glm::vec4> corners = getFrustumCornersWorldSpace(projection, view);

    glm::mat4 lightView = getLightViewMatrix(corners);

    glm::mat4 lightProjection = getLightProjectionMatrix(corners, lightView);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

void calculateCascadeSplits(float near, float far, int numCascades, float* cascadeSplits) {
    float lambda = 0.5f; // Blend factor between linear and logarithmic splits
    for (int i = 0; i < numCascades; i++) {
        float linearSplit = near + (far - near) * (i + 1) / numCascades;
        float logSplit = near * pow(far / near, (i + 1) / float(numCascades));
        cascadeSplits[i] = lambda * logSplit + (1.0f - lambda) * linearSplit;
    }
}

std::vector<glm::mat4> calculateCascadeLightMatrices(const glm::mat4& cameraProj, const glm::mat4& cameraView, const glm::vec3& lightDir,const float* cascadeSplits) {
    std::vector<glm::mat4> lightMatrices;

    for (size_t i = 0; i < numCascades - 1; i++)
    {
        // Get the near and far planes for this cascade
        float nearPlane = cascadeSplits[i];
        float farPlane = cascadeSplits[i + 1];

        // Extract frustum corners in world space for this cascade
        glm::mat4 cascadeProj = glm::perspective(
            glm::radians(45.0f), // FOV
            (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,                // Aspect ratio
            nearPlane,
            farPlane);

        std::vector<glm::vec4> frustumCorners = getFrustumCornersWorldSpace(cascadeProj, cameraView);

        glm::mat4 lightView = getLightViewMatrix(frustumCorners);

        glm::mat4 lightProjection = getLightProjectionMatrix(frustumCorners, lightView);

        lightMatrices.push_back(lightProjection * lightView);
    }

    return lightMatrices;
}
void renderGrid() {

    // select active shader program
    lavaShader.useShaderProgram();

    //update view matrix
    view = myCamera.getViewMatrix();
 
    glUniformMatrix4fv(glGetUniformLocation(lavaShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    // send light dir to shader
    glUniform3fv(glGetUniformLocation(lavaShader.shaderProgram, "lightDir"), 1, glm::value_ptr(glm::vec3(glm::inverseTranspose(view) * glm::vec4(lightDir, 1.0f))));

    glm::mat4 lavaModel = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -1.0f, -5.0f));
    //send grid model matrix data to shader
    glUniformMatrix4fv(glGetUniformLocation(lavaShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lavaModel));

    // compute normal matrix for grid
    normalMatrix = glm::mat3(glm::inverseTranspose(view * lavaModel));

    //send grid normal matrix data to shader
    glUniformMatrix3fv(glGetUniformLocation(lavaShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glCheckError();

    //send light data to shader
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDir = glm::normalize(lightDir);
    // send light dir to shader
    glUniform3fv(glGetUniformLocation(lavaShader.shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
    //set light color
    lightColor = glm::vec3(1.0f, 0.2f, 0.1f); //redish light
    // send light color to shader
    glUniform3fv(glGetUniformLocation(lavaShader.shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

    //send texture to shader
    glActiveTexture(GL_TEXTURE0);
    glCheckError();
    glBindTexture(GL_TEXTURE_2D, gridTexture);
    glCheckError();
    glUniform1i(gridTextureLoc, 0);
    glCheckError();

    //send sim time
    glUniform1f(simTimeLoc, simTime);
    glCheckError();

    glBindVertexArray(gridVAO);
    glCheckError();

    // draw grid
    glDrawElements(GL_TRIANGLES, (GRID_NUM_POINTS_WIDTH - 1) * (GRID_NUM_POINTS_HEIGHT - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_POINTS, 0, GRID_NUM_POINTS_WIDTH * GRID_NUM_POINTS_HEIGHT);

}
void drawObjects(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (shire) {
        teapot.Draw(shader);
        if (!night) {
            shireAnimals.Draw(shader);
        }
        shireObjects.Draw(shader);
        shireLandscape.Draw(shader);

        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - lastTime;
        lastTime = currentTime;


        glm::mat4 swordModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, currentYPos, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(swordModel));
        stingSword.Draw(shader);

        if (!depthPass) {
            currentYPos += hoverSpeed * elapsedTime;
            if (currentYPos > 0.5f || currentYPos < 0.0f) {
                hoverSpeed = -hoverSpeed;
            }
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * swordModel))));
        }

        glm::mat4 gooseModel = glm::translate(glm::mat4(1.0f), glm::vec3(-7.8836f, 2.5526f, -1.15941f));
        gooseModel *= glm::rotate(glm::mat4(1.0f), glm::radians(gooseAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        gooseModel *= glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
        gooseModel *= glm::rotate(glm::mat4(1.0f), glm::radians(gooseShyAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        gooseModel *= glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.0f)));
        gooseModel *= glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(-7.8836f, 2.5526f, -1.15941f)));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(gooseModel));
        goose.Draw(shader);
        if (!depthPass) {
            gooseAngle += rotationSpeed * elapsedTime;
            gooseAngle += shyRotationSpeed * elapsedTime;
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * gooseModel))));
        }
    }
    else {
        //draw mordor
        glm::mat4 eyeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, eyeCurrentPos, 0.0f));

       

        mordorLandscape.Draw(shader);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(eyeModel));
        mordorEye.Draw(shader);
        if (!depthPass) {
            eyeCurrentPos += eyeTranslateSpeed;
            if (eyeCurrentPos > 0.5f || eyeCurrentPos < 0.0f) {
                eyeTranslateSpeed = -eyeTranslateSpeed;
            }
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * eyeModel))));
            //eyeLightPos = glm::inverseTranspose(glm::mat3(view * eyeModel)) * eyeLightPos;
            //glUniform3fv(glGetUniformLocation(shader.shaderProgram, "eyeLightPos"), 1, glm::value_ptr(eyeLightPos));
        }
        screenQuadShader.useShaderProgram();
        float currentTime = glfwGetTime();
        offsetY = fmod(currentTime * speed, 1.0f);
        glUniform1f(glGetUniformLocation(screenQuadShader.shaderProgram, "offsetY"), offsetY);
        screenQuad.Draw(screenQuadShader);
    }
    
}

void renderShadowMaps(const std::vector<glm::mat4>& lightMatrices) {
    for (int i = 0; i < numCascades-1; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBOs[i]);
        glViewport(0, 0, shadowMapResolution, shadowMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Bind appropriate shader for depth rendering
        depthMapShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));

        // Render the scene using the light's perspective
        drawObjects(depthMapShader, true);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderWithShadows(gps::Shader shader, const std::vector<glm::mat4>& lightMatrices, const float* cascadeSplits) {
    // Bind main shader
    shader.useShaderProgram();

    // Pass cascade data
    for (int i = 0; i < numCascades-1; ++i) {
        std::string uniformName = "lightSpaceMatrices[" + std::to_string(i) + "]";
        GLint location = glGetUniformLocation(shader.shaderProgram, uniformName.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
    }

    // Send cascade splits
    GLint splitsLocation = glGetUniformLocation(shader.shaderProgram, "cascadeSplits");
    glUniform1fv(splitsLocation, numCascades, cascadeSplits);

    // Render the scene
    drawObjects(shader, false);
}

void cameraAnimation(){
    if (firstRotation < 15) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        pitch++;
        myCamera.rotate(yaw, pitch);
        firstRotation++;
    }
    else {
        if (transitionForwardFirst < 20) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
            transitionForwardFirst++;
        }
        else {
            if (secondRotation < 40) {
                yaw -= 0.1f;
                pitch--;
                myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
                myCamera.rotate(yaw, pitch);
                secondRotation++;
            }
            else {
                if (lookDown < 10) {
                    yaw += 0.5f;
                    myCamera.rotate(yaw, pitch);
                    lookDown++;
                }
                else {
                    //would like a wait here, a noop
                    if (thirdRotation < 35) {
                        yaw -= 0.1f;
                        pitch++;
                        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
                        myCamera.rotate(yaw, pitch);
                        thirdRotation++;
                    }
                    else {
                        if (transitionForwardSecond < 30) {
                            transitionForwardSecond++;
                            myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 2);
                        }
                        else {
                            if (forthRotation < 12) {
                                myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
                                pitch++;
                                myCamera.rotate(yaw, pitch);
                                forthRotation++;
                            }
                            else {
                                if (transitionForwardThird < 40) {
                                    myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 2);
                                    transitionForwardThird++;
                                }
								else {
									if (transitionForwardForth < 45) {
										myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 2);
										transitionForwardForth++;
									}
									else {
										if (sixthRotation < 80) {
											myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
											pitch--;
											yaw -= 0.1f;
											myCamera.rotate(yaw, pitch);
											sixthRotation++;
										}
										else {
											if (transitionForwardFifth < 15) {
												myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 2);
												transitionForwardFifth++;
											}
											else {
												if (seventhRotation < 90) {
													pitch--;
													myCamera.rotate(yaw, pitch);
													seventhRotation++;
												}
												else {
													presenting = false;
												}
											}
										}
									}
								}
                            }
                        }
                    }
                }
            }
        }
    }
}


void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render the scene

    // render the teapot
    //renderTeapot(myBasicShader);
    float cascadeSplits[numCascades];
    calculateCascadeSplits(0.1f, 40.0f, numCascades, cascadeSplits);

    std::vector<glm::mat4> lightMatrices = calculateCascadeLightMatrices(projection, view, lightDir, cascadeSplits);
    /*std::cout << "Light Matrices Size: " << lightMatrices.size() << std::endl;
    for (size_t i = 0; i < lightMatrices.size(); ++i) {
        std::cout << "Matrix " << i << ": " << glm::to_string(lightMatrices[i]) << std::endl;
    }*/
    renderShadowMaps(lightMatrices);
    if (shire) {
        if (!night) {
            renderWithShadows(myBasicShader, lightMatrices, cascadeSplits);
            daySkybox.Draw(skyboxShader, view, projection);
        }
        else {
            renderWithShadows(nightShader, lightMatrices, cascadeSplits);
            nightSkybox.Draw(skyboxShader, view, projection);
        }

        if (getCameraDistance(shireRingPos) < 2.0f) {
            std::cout << "Near ring!" << std::endl;
            shire = false;
            initMordorUniforms();
            myCamera.setPlaneBindVar(false);
        }
    }
    else {
        renderWithShadows(mordorShader, lightMatrices, cascadeSplits);
        glCheckError();
        renderGrid();
        glCheckError();
        mordorSkybox.Draw(skyboxShader, view, projection);
        if (getCameraDistance(mordorRingPos) < 2.0f) {
            std::cout << "Near ring!" << std::endl;
            shire = true;
            initUniforms();
            myCamera.setPlaneBindVar(true);
        }
    }
    if (presenting) {
        cameraAnimation();
    }

}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(numCascades - 1, shadowMapFBOs);
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    initFBO();
    initVBOs();
    gridTexture = initTexture("models/mordor/liquidLava_text.jpg");
    setWindowCallbacks();
    initSkybox();
    initAngles();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();
        glCheckError();
        simTime += 0.007f;

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
