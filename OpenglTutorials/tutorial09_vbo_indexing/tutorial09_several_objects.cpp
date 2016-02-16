// Include standard headers
#include <stdio.h>
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
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

// Include Bone Class

#include "bone.hpp"
#include "skeleton.hpp"

int initStuff();

void drawBone(Bone bone, mat4 ProjectionMatrix, mat4 ViewMatrix) {
    
}

void drawSkeleton(Bone root, mat4 ProjectionMatrix, mat4 ViewMatrix) {
    
}

int main( void )
{
    // Init root bone and child bone
    Bone root;
    Bone child;
    Bone child2;
//    Bone child3;
    
    // init skeleton and pass root bone to set root, root automatically added as bone
    Skeleton skeleton(root);
    
    // add child bone to skeleton
    skeleton.addBone(child);
    skeleton.addBone(child2);
//    skeleton.addBone(child3);
    
    // Associate child bones
    root.addChild(&child);
    child.addChild(&child2);
//    child2.addChild(&child3);
    
    // Associated parent bones
    child.addParent(&root);
    child2.addParent(&child);
//    child3.addParent(&child2);
    
    skeleton.querySkeleton();
    
    initStuff();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint Texture = loadDDS("uvmap.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool catModel = loadOBJ("cat.obj", vertices, uvs, normals);

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

    // Init root model matrix 1
    root.position = glm::vec3(0.0f, 0.0f, 0.0f);
    root.ModelMatrix = glm::translate(root.ModelMatrix, root.position);
    
    // Init model matrix 2
    child.position = glm::vec3(0.0f, 0.0f, 1.0f);
    child.ModelMatrix = glm::translate(child.ModelMatrix, child.position);
    
    // Init model matrix 3
    child2.position = glm::vec3(0.0f, 0.0f, 1.2f);
    child2.ModelMatrix = glm::translate(child2.ModelMatrix, child2.position);
    
    // Init model matrix 4
//    child3.position = glm::vec3(0.0f, 0.0f, 2.0f);
//    child3.ModelMatrix = glm::translate(child3.ModelMatrix, child3.position);
    
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    
//    child3.MVP = ProjectionMatrix * ViewMatrix * child3.ModelMatrix;
    
    // GAME LOOP /////
    root.MVP = ProjectionMatrix * ViewMatrix * root.ModelMatrix;
    child.MVP = ProjectionMatrix * ViewMatrix * child.ModelMatrix;
    child2.MVP = ProjectionMatrix * ViewMatrix * child2.ModelMatrix;
    
    do{
      root.MVP = ProjectionMatrix * ViewMatrix * root.ModelMatrix;
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
	
		glm::vec3 lightPos = glm::vec3(4,4,4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
	

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

        // FIRST OBJECT
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &root.MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &root.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);

		////// End of rendering of the first object //////
		
		//SECOND OBJECT

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &child.MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &child.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);

		////// End of rendering of the second object //////

        //THIRD OBJECT
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &child2.MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &child2.ModelMatrix[0][0]);
        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
        
        ////// End of rendering of the third object //////
        
//        //FOURTH OBJECT
//        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &child3.MVP[0][0]);
//        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &child3.ModelMatrix[0][0]);
//        skeleton.bindDraw(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices);
//        
//        ////// End of rendering of the fourth object //////
        
        
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
        
        // Controls
        // Move Parent Cat Up and Down, update Child Cat relatively
        if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
            root.update(-0.1f, root.ModelMatrix, ProjectionMatrix, ViewMatrix);
//            root.ModelMatrix = glm::rotate(root.ModelMatrix, -0.1f, glm::vec3(1, 0, 0));
//            child.MVP = root.MVP*child.ModelMatrix;
//            child2.MVP = child.MVP*child2.ModelMatrix;
        }
        if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
            root.update(0.1f, root.ModelMatrix, ProjectionMatrix, ViewMatrix);
            
            // Rotate 'root' bone
            // rotate children
            
//            root.ModelMatrix = glm::rotate(root.ModelMatrix, 0.1f, glm::vec3(1, 0, 0));
//            child.MVP = root.MVP*child.ModelMatrix;
//            child2.MVP = child.MVP*child2.ModelMatrix;
        }
        if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
            child.update(-0.1f, child.ModelMatrix, ProjectionMatrix, ViewMatrix);
//            child.ModelMatrix = glm::rotate(child.ModelMatrix, -0.1f, glm::vec3(1, 0, 0));
//            child.MVP = root.MVP*child.ModelMatrix;
//            child2.MVP = child.MVP*child2.ModelMatrix;
        }
        if (glfwGetKey( window, GLFW_KEY_L ) == GLFW_PRESS){
            child.update(0.1f, child.ModelMatrix, ProjectionMatrix, ViewMatrix);
//            child.ModelMatrix = glm::rotate(child.ModelMatrix, 0.1f, glm::vec3(1, 0, 0));
//            child.MVP = root.MVP*child.ModelMatrix;
//            child2.MVP = child.MVP*child2.ModelMatrix;
        }
        
        if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
            child2.update(-0.1f, child2.ModelMatrix, ProjectionMatrix, ViewMatrix);
//            child2.MVP = child.MVP*child2.ModelMatrix;

        }
        if (glfwGetKey( window, GLFW_KEY_K ) == GLFW_PRESS){
            child2.update(0.1f, child2.ModelMatrix, ProjectionMatrix, ViewMatrix);
//            child2.MVP = child.MVP*child2.ModelMatrix;

        }
//        if (glfwGetKey( window, GLFW_KEY_I ) == GLFW_PRESS){
//            child3.updateRoot(-0.1f, child3.ModelMatrix, ProjectionMatrix, ViewMatrix);
//        }
//        if (glfwGetKey( window, GLFW_KEY_J ) == GLFW_PRESS){
//            child3.updateRoot(0.1f, child3.ModelMatrix, ProjectionMatrix, ViewMatrix);
//        }
        
        

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
    window = glfwCreateWindow( 1024, 768, "Hand Model", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);
    
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


