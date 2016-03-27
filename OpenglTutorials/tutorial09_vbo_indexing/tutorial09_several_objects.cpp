// Include standard headers
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include "glm/ext.hpp"
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

// Include Bone Class

#include "bone.hpp"
#include "skeleton.hpp"
#include "finger.hpp"

int initStuff();
void render(Finger &finger, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix);
void initMVP(Finger &finger, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, float offSetZ, float rootOffset);
void playAnimation(Bone &bone,mat4 ProjectionMatrix, mat4 ViewMatrix, float deltaTime);
void calcIK(Finger finger, Bone target);
glm::vec3 curve(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
GLuint MatrixID, ViewMatrixID, ModelMatrixID;
GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;
std::vector<unsigned short> indices;

Skeleton skeleton;
Skeleton skeleton2;

glm::vec3 targetPosition = glm::vec3(6.0f, 6.0f, -0.5f);
glm::vec3 endPointPosition;
glm::vec3 currJointPosition;
float angle;
float rotate_angle = 0.01f;
float translateAmount = 0.1f;
bool targetNotFound = false;
bool targetNotFoundFinger2 = false;
bool followCurve = false;
bool path1 = false;
glm::vec3 a = glm::vec3(7.0f, 3.0f, 1.0f);
glm::vec3 d = glm::vec3(1.0f, 10.0f, 1.0f);
glm::vec3 c = glm::vec3(-6.0f, 6.0f, 6.0f);
glm::vec3 b = glm::vec3(9.0f, 6.0f, 6.0f);

glm::vec3 ee = glm::vec3(9.0f, 6.0f, 6.0f);
glm::vec3 f = glm::vec3(1.0f, 10.0f, 1.0f);
glm::vec3 g = glm::vec3(-6.0f, 6.0f, 6.0f);
glm::vec3 h = glm::vec3(7.0f, 2.0f, 0.0f);

int main( void )
{
    // Target object
    Bone target;
    
    // Finger 1
    Bone root;
    Bone child;
    Bone child2;
    Bone endEffectorBone;
    
    // Finger 2
    Bone root2;
    Bone child3;
    Bone child4;
    Bone endEffectorBone2;
    
    Finger finger1(root); // root added as bone to finger on init of finger
    finger1.addBone(root);
    finger1.addBone(child);
    finger1.addBone(child2);
    finger1.addBone(endEffectorBone);
    
    Finger finger2(root2);
    finger2.addBone(root2);
    finger2.addBone(child3);
    finger2.addBone(child4);
    finger2.addBone(endEffectorBone2);
    
    root.addChild(&child);
    child.addChild(&child2);
    child2.addChild(&endEffectorBone);
    
    root2.addChild(&child3);
    child3.addChild(&child4);
    child4.addChild(&endEffectorBone2);
    
    child.addParent(&root);
    child2.addParent(&child);
    endEffectorBone.addParent(&child2);
    
    child3.addParent(&root2);
    child4.addParent(&child3);
    endEffectorBone2.addParent(&child4);

    skeleton.addFinger(finger1);
    endEffectorBone.scaleBone(glm::vec3(0.3f, 0.8f, 0.3f));
    
    skeleton2.addFinger(finger2);
    endEffectorBone2.scaleBone(glm::vec3(0.3f, 0.8f, 0.3f));

    initStuff();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint Texture = loadDDS("red.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	loadOBJ("sphere.obj", vertices, uvs, normals);

	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VBO

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
    
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    
    initMVP(finger1, ProjectionMatrix, ViewMatrix, 2.0f, 0.0f);
    initMVP(finger2, ProjectionMatrix, ViewMatrix, 2.0f, 5.0f);
    
    target.position = targetPosition;
    target.ModelMatrix = glm::translate(target.ModelMatrix, target.position);
    target.MVP = target.ModelMatrix * ViewMatrix * ProjectionMatrix;

    // GAME LOOP //
    
    do{
        ProjectionMatrix = getProjectionMatrix();
        ViewMatrix = getViewMatrix();
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		
		////// Start of the rendering of the Light object //////
		
		// Use our shader
		glUseProgram(programID);
	
		glm::vec3 lightPos = glm::vec3(2, 2, 2);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
        root.MVP    = ProjectionMatrix * ViewMatrix * root.ModelMatrix;
        root2.MVP   = ProjectionMatrix * ViewMatrix * root2.ModelMatrix;
        target.MVP  = ProjectionMatrix * ViewMatrix * target.ModelMatrix;
   
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &root.MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &root.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &root2.MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &root2.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &target.MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &target.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
   
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);
        
        // Render Fingers
        render(finger1, ProjectionMatrix, ViewMatrix);
        render(finger2, ProjectionMatrix, ViewMatrix);
   
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
        
        // Controls
        // Move Parent Cat Up and Down, update Child Bone relatively
        if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
            root.update(0.0f, glm::vec3(0, 0, 1));
            root2.update(0.0f, glm::vec3(0, 0, 1));
        }
        
        if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS && glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) != GLFW_PRESS){
            targetPosition.x += translateAmount;
            target.ModelMatrix = glm::translate(mat4(), targetPosition);
            targetNotFound = true;
            targetNotFoundFinger2 = true;
            
        }
        
        if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS && glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) != GLFW_PRESS){
            targetPosition.x -= translateAmount;
            target.ModelMatrix = glm::translate(mat4(), targetPosition);
            targetNotFound = true;
            targetNotFoundFinger2 = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS && glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) != GLFW_PRESS){
            targetPosition.y += translateAmount;
            target.ModelMatrix = glm::translate(mat4(), targetPosition);
            targetNotFound = true;
            targetNotFoundFinger2 = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS && glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) != GLFW_PRESS){
            targetPosition.y -= translateAmount;
            target.ModelMatrix = glm::translate(mat4(), targetPosition);
            targetNotFound = true;
            targetNotFoundFinger2 = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
          targetPosition.z += translateAmount;
          target.ModelMatrix = glm::translate(mat4(), targetPosition);
          targetNotFound = true;
          targetNotFoundFinger2 = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS){
            targetPosition.z -= translateAmount;
            target.ModelMatrix = glm::translate(mat4(), targetPosition);
            targetNotFound = true;
            targetNotFoundFinger2 = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_M ) == GLFW_PRESS ){
            targetNotFound = true;
            targetNotFoundFinger2 = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_B ) == GLFW_PRESS ){
            followCurve = !followCurve;
        }
        
        if(followCurve) {
            float t = glm::mod( (float)currentTime, 5.0f ) / (5.0f);
            t = t*2;
            if(t<1) {
                targetPosition = curve(t, a, b, c, d);
                cout << " Curve 1" << endl;
            } else {
                t = 2 - t;
                targetPosition = curve(t, d, c, b, a);
                cout << " Curve 2" << endl;
            }
//            targetPosition = curve(t, a, b, c, d);
            target.ModelMatrix = glm::translate(mat4(), targetPosition);
            targetNotFound = true;
            targetNotFoundFinger2 = true;
            
            if(targetPosition == glm::vec3(-0.85f, 7.9f, 3.5f)) {
                a = glm::vec3(14.0f, 3.0f, 1.0f);
                d = glm::vec3(1.0f, 10.0f, 1.0f);
                c = glm::vec3(-6.0f, 6.0f, 6.0f);
                b = glm::vec3(9.0f, 6.0f, 6.0f);

            }
            
        }
        
        if(targetNotFound) {
            calcIK(finger1, target);
        }
        if(targetNotFoundFinger2) {
            calcIK(finger2, target);
        }
        
       	
    } // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
    
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void calcIK(Finger finger, Bone target) {
    
    // IK - CCD
    glm::vec3 endEffectorBonePosition = glm::vec3(finger.bones[finger.bones.size()-1]->ModelMatrixTemp[3]); //
    targetPosition = glm::vec3(target.ModelMatrix[3]);
    
    if (targetNotFound || targetNotFoundFinger2) {
        for(int i=finger.bones.size()-2; i>=0 ; i--) {
            glm::vec3 currBonePosition = glm::vec3(finger.bones[i]->ModelMatrixTemp[3]); //
            
            glm::vec3 targetVector = targetPosition - currBonePosition;
            glm::vec3 endEffector = endEffectorBonePosition - currBonePosition;
            endEffector = glm::normalize(endEffector);
            targetVector = glm::normalize(targetVector);
            
            float rotationAngle = glm::acos(glm::dot(targetVector, endEffector));
            
            glm::vec3 rotationAxis = glm::cross(endEffector, targetVector);       //targetVector, endEffector);
            rotationAxis = glm::mat3(glm::inverse(finger.bones[i]->ModelMatrixTemp)) * rotationAxis;
            rotationAxis = glm::normalize(rotationAxis);
            float distance = glm::distance(endEffectorBonePosition, targetPosition);
            if((rotationAngle > 0 || rotationAngle < 0) ) {
                finger.bones[i]->update(rotationAngle, rotationAxis);
            }
            endEffectorBonePosition = glm::vec3(finger.bones[finger.bones.size()-1]->ModelMatrixTemp[3]); //
            
            if (glm::dot(targetVector, endEffector) > 1.0f) {
                targetNotFound = false;
                targetNotFoundFinger2 = false;
            }
            if (distance < 0.01f) {
                targetNotFound = false;
                targetNotFoundFinger2 = false;
            }
            if (targetPosition == endEffectorBonePosition) {
                targetNotFound = false;
                targetNotFoundFinger2 = false;
            }
            else {
                targetNotFound = true;
                targetNotFoundFinger2 = true;
            }
        }
    }
}

glm::vec3 curve(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    
    float u = 1-t;
    float tt = t*t;
    float uu = u*u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    glm::vec3 p = uuu * p0; //first term
    p += 3 * uu * t * p1; //second term
    p += 3 * u * tt * p2; //third term
    p += ttt * p3; //fourth term
    
    return p;
}

void render(Finger &finger, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    for(int i = 0; i<finger.bones.size(); i++) {
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &finger.bones[i]->MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &finger.bones[i]->ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
    }
}

void initMVP(Finger &finger, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, float offSetZ, float rootOffset) {
//    float offSetZ = 2.0f;
    for (int i = 0; i<finger.bones.size(); i++) {
        if (i==0) {
            finger.bones[i]->position = glm::vec3(rootOffset, 0.0f, 0.0f);
            finger.bones[i]->ModelMatrix = glm::translate(finger.bones[i]->ModelMatrix, finger.bones[i]->position);
            finger.bones[i]->MVP = finger.bones[i]->ModelMatrix  * ViewMatrix * ProjectionMatrix;
        } else {
            finger.bones[i]->position = glm::vec3(0.0f, offSetZ, 0.0f);
            finger.bones[i]->ModelMatrix = glm::translate(finger.bones[i]->ModelMatrix, finger.bones[i]->position);
            finger.bones[i]->MVP = finger.bones[i]->ModelMatrix  * ViewMatrix * ProjectionMatrix;
        }
    }
}

int initStuff() {
    
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Inverse Kinematics - Cyclid Coordinate Descent", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);
    
    // Dark blue background
    glClearColor(0.1f, 1.0f, 1.f, 1.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    return 0;
}


