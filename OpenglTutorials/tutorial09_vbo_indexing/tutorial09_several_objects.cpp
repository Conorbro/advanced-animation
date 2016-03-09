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
void initMVP(Finger &finger, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix);
void playAnimation(Bone &bone,mat4 ProjectionMatrix, mat4 ViewMatrix, float deltaTime);
void calcIK(Finger finger1, Bone root2);

GLuint MatrixID, ViewMatrixID, ModelMatrixID;
GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;
std::vector<unsigned short> indices;

Skeleton skeleton;

glm::vec3 targetPosition = glm::vec3(6.0f, 6.0f, -0.5f);
glm::vec3 endPointPosition;
glm::vec3 currJointPosition;
float angle;
float rotate_angle = 0.01f;
float translateAmount = 0.1f;
bool rotatee = false;

int main( void )
{
    
    // Finger 1
    // Init root bone and child bone
    Bone root;
    Bone root2;
    Bone child;
    Bone child2;
    Bone endEffectorBone;

    Finger finger1(root); // root added as bone to finger on init of finger
    finger1.addBone(root);
    finger1.addBone(child);
    finger1.addBone(child2);
    finger1.addBone(endEffectorBone);
    
    root.addChild(&child);
    child.addChild(&child2);
    child2.addChild(&endEffectorBone);
    
    child.addParent(&root);
    child2.addParent(&child);
    endEffectorBone.addParent(&child2);

    skeleton.addFinger(finger1);
    
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
	GLuint Texture = loadDDS("texture2.DDS");
	
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
    
    initMVP(finger1, ProjectionMatrix, ViewMatrix);
    
    root2.position = targetPosition;
    root2.ModelMatrix = glm::translate(root2.ModelMatrix, root2.position);
    root2.MVP = root2.ModelMatrix * ViewMatrix * ProjectionMatrix;

    double xpos, ypos;
    // GAME LOOP //
    
    do{
        glfwGetCursorPos(window, &xpos, &ypos);
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
        root.MVP  = ProjectionMatrix * ViewMatrix * root.ModelMatrix;
        root2.MVP  = ProjectionMatrix * ViewMatrix * root2.ModelMatrix;
   
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &root.MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &root.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &root2.MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &root2.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
   
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);
        
        // Render Fingers
        render(finger1, ProjectionMatrix, ViewMatrix);
   
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
        }
        
        if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
            targetPosition.x += translateAmount;
            root2.ModelMatrix = glm::translate(mat4(), targetPosition);
            rotatee = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
            targetPosition.x -= translateAmount;
            root2.ModelMatrix = glm::translate(mat4(), targetPosition);
            rotatee = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
            targetPosition.y += translateAmount;
            root2.ModelMatrix = glm::translate(mat4(), targetPosition);
            rotatee = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
            targetPosition.y -= translateAmount;
            root2.ModelMatrix = glm::translate(mat4(), targetPosition);
            rotatee = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
          targetPosition.z += translateAmount;
          root2.ModelMatrix = glm::translate(mat4(), targetPosition);
          rotatee = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS){
            targetPosition.z -= translateAmount;
            root2.ModelMatrix = glm::translate(mat4(), targetPosition);
            rotatee = true;
        }
        
        if (glfwGetKey( window, GLFW_KEY_M ) == GLFW_PRESS ){
            rotatee = true;
        }
        
        if(rotatee) {
            calcIK(finger1, root2);
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

void calcIK(Finger finger1, Bone root2) {
    
    // IK - CCD
        for(int i=finger1.num_bones-2; i>=1 ; i--) {
            
            // Get end effector position from Model Matrix of End Effector Bone
            glm::vec3 endEffectorBonePosition = glm::vec3(finger1.bones[3]->ModelMatrixTemp[3]); //
            targetPosition = glm::vec3(root2.ModelMatrix[3]);
        
            // Get current bone's pivot position
            glm::vec3 currBonePosition = glm::vec3(finger1.bones[i]->ModelMatrixTemp[3]); //
            
            glm::vec3 targetVector = targetPosition - currBonePosition;
            glm::vec3 endEffector = endEffectorBonePosition - currBonePosition;
            endEffector = glm::normalize(endEffector);
            targetVector = glm::normalize(targetVector);
            
            // Calculate angle of rotation from dot product of these two
            float rotationAngle = glm::acos(glm::dot(targetVector, endEffector));
            
            // if angle between a certain threshold, stop rotating!
            if (glm::dot(targetVector, endEffector) > 1.0f)
            {
                rotatee = false;
            } else {
                rotatee = true;
            }
            
            if (targetPosition == endEffectorBonePosition) {
                rotatee = false;
            }
            
            // Get axis of rotation from cross product of targetVector and end effector vector
            glm::vec3 rotationAxis = glm::cross(targetVector, endEffector);
            // check if two lines are equal
            
            if(rotatee) {
                    finger1.bones[i]->update(rotationAngle, rotationAxis);
            }
            
        }
}

void render(Finger &finger, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    for(int i = 0; i<finger.bones.size(); i++) {
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &finger.bones[i]->MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &finger.bones[i]->ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
    }
}

void initMVP(Finger &finger, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    float offSetZ = 2.0f;
    for (int i = 0; i<finger.bones.size(); i++) {
        if (i==0) {
            finger.bones[i]->position = glm::vec3(0.0f, 0.0f, 0.0f);
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
    window = glfwCreateWindow( 1024, 768, "Hierarchical Hand Model", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    return 0;
}


