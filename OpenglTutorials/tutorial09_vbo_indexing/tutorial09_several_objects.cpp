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

// Include Eigen
#include <eigen3/Eigen/Jacobi>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_access.hpp>
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
void initMVP(Finger &finger, float offSetX, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix);
void playAnimation(Bone &bone,mat4 ProjectionMatrix, mat4 ViewMatrix, float deltaTime);

GLuint MatrixID, ViewMatrixID, ModelMatrixID;
GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;
std::vector<unsigned short> indices;
Bone wrist;
Skeleton skeleton(wrist);

// Would be preferable not to have to hard code this - mouse click would be nice :)
glm::vec3 targetPosition = glm::vec3(2.5f, 0.0f, 2.0f);
glm::vec3 endPointPosition;
glm::vec3 currJointPosition;
float angle;
bool test = true;

int main( void )
{
    
    // Finger 1
    // Init root bone and child bone
    Bone root;
    Bone child;
    Bone child2;
    Finger finger1(root);
    
    finger1.addBone(child);
    finger1.addBone(child2);
    
    root.addChild(&child);
    child.addChild(&child2);
    
    child.addParent(&root);
    child2.addParent(&child);

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
	GLuint Texture = loadDDS("uvmap.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	loadOBJ("cat.obj", vertices, uvs, normals);

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
    
    initMVP(finger1, 0.0f, ProjectionMatrix, ViewMatrix);
    
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::rotate(root.ModelMatrix, 20.0f, glm::vec3(1, 0, 0));
    root.ModelMatrix = glm::translate(root.ModelMatrix, position);
    root.MVP = ProjectionMatrix * ViewMatrix * root.ModelMatrix;
    
    position = glm::vec3(2.0f, 0.0f, 0.0f);
    endPointPosition = glm::vec3(child2.position.x + 1.0f, child2.position.y + 1.0f, 0.0f);
    double xpos, ypos;
    
    // GAME LOOP //
    do{        
        glfwGetCursorPos(window, &xpos, &ypos);
        targetPosition = glm::vec3(xpos, 0.0f, ypos);
        ProjectionMatrix = getProjectionMatrix();
        ViewMatrix = getViewMatrix();
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
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
   
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &root.MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &root.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
   
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
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
            root.update(-0.1f, root.ModelMatrix, ProjectionMatrix, ViewMatrix);
//            cout << "Child Position = " << child.position.x << " " << child.position.y << " " << child.position.z << endl;
        }
        if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
            root.update(0.1f, root.ModelMatrix, ProjectionMatrix, ViewMatrix);
        }
        if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
            child.update(-0.1f, child.ModelMatrix, ProjectionMatrix, ViewMatrix);
        }
        if (glfwGetKey( window, GLFW_KEY_L ) == GLFW_PRESS){
            child.update(0.1f, child.ModelMatrix, ProjectionMatrix, ViewMatrix);
        }
        if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
            child2.update(-0.1f, child2.ModelMatrix, ProjectionMatrix, ViewMatrix);
        }
        if (glfwGetKey( window, GLFW_KEY_K ) == GLFW_PRESS){
            child2.update(0.1f, child2.ModelMatrix, ProjectionMatrix, ViewMatrix);
        }
        if (glfwGetKey( window, GLFW_KEY_M ) == GLFW_PRESS && test == true){
            // Do CCD :P
            test = false;
//            while (endPointPosition != targetPosition) {
                for(int i=finger1.num_bones-1; i>=0; i--) {
                    
                    // update position todo - not updating in model matrix for some reason ???
                    
                    glm::vec3 currBonePosition = glm::vec3(glm::column(finger1.bones[i]->ModelMatrix, 3).x, glm::column(finger1.bones[i]->ModelMatrix, 3).y, glm::column(finger1.bones[i]->ModelMatrix, 3).z);
                    
                    glm::vec3 targetVector = glm::vec3(targetPosition.x - currBonePosition.x, 0.0f, targetPosition.z - currBonePosition.z);
                    targetVector = glm::normalize(targetVector);
                    
                    glm::vec3 outerMostJointVector = glm::vec3(endPointPosition.x - currBonePosition.x, 0.0f, endPointPosition.z - currBonePosition.z);
                    outerMostJointVector = glm::normalize(outerMostJointVector);
                    
                    // extract from the model matrix the forward vector todo maybe?
                    
                    cout << "Bone " << i << endl;
                    cout << "End Point Position " << endPointPosition.x << " " << endPointPosition.z << endl;
                    cout << "Curr Joint Position " << finger1.bones[i]->position.x << " " << finger1.bones[i]->position.y << " " << finger1.bones[i]->position.z << endl;
                    cout << "Curr Joint Position from MM " << currBonePosition.x << " " << currBonePosition.y << " "  << currBonePosition.z << endl;
                    cout << "Target Vector " << targetVector.x << " " << targetVector.y << " " << targetVector.z << endl;
                    cout << "Outermost joint vector " << outerMostJointVector.x << " " << outerMostJointVector.y << " " << outerMostJointVector.z << endl;
                    
                    glm::dot(targetVector, outerMostJointVector);
                    
                    float vecLengthProduct = glm::length(targetVector) * glm::length(outerMostJointVector);
                    float angle = glm::acos(glm::dot(targetVector, outerMostJointVector)/vecLengthProduct);
                    
                    endPointPosition.x = currBonePosition.x + 2 * sin(angle) * -1;
                    endPointPosition.z = currBonePosition.z + 2 * cos(angle);
                    
                    cout << "Angle = " << angle << endl;
                    
                    finger1.bones[i]->update(angle, finger1.bones[i]->ModelMatrix, ProjectionMatrix, ViewMatrix);
                    
                }
//            }
        }
        if (glfwGetKey( window, GLFW_KEY_N ) == GLFW_PRESS) {
            test = true;
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

void render(Finger &finger, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    for(int i = 0; i<finger.bones.size(); i++) {
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &finger.bones[i]->MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &finger.bones[i]->ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
    }
}

void initMVP(Finger &finger, float offSetX, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    float offSetZ = 2.0f;
    for (int i = 0; i<finger.bones.size(); i++) {
        if (i==0) {
            finger.bones[i]->position = glm::vec3(offSetX, 0.0f, offSetZ);
            finger.bones[i]->ModelMatrix = glm::translate(finger.bones[i]->ModelMatrix, finger.bones[i]->position);
            finger.bones[i]->MVP = ProjectionMatrix * ViewMatrix * finger.bones[i]->ModelMatrix;
        } else {
            finger.bones[i]->position = glm::vec3(0.0f, offSetZ, 0.0f);
            finger.bones[i]->ModelMatrix = glm::translate(finger.bones[i]->ModelMatrix, finger.bones[i]->position);
            finger.bones[i]->MVP = ProjectionMatrix * ViewMatrix * finger.bones[i]->ModelMatrix;
        }
    }
}


void playAnimation(Bone &bone,mat4 ProjectionMatrix, mat4 ViewMatrix, float deltaTime)
{
    float time = glfwGetTime();
    float timeElapsed = 0.0f;
    bone.update(sin(time*15)/5, bone.ModelMatrix, ProjectionMatrix, ViewMatrix);
    timeElapsed += deltaTime;
    
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
    
    // Set the mouse at the center of the screen
//    glfwPollEvents();
//    glfwSetCursorPos(window, 1024/2, 768/2);
    
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


