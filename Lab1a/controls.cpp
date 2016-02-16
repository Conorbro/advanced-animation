#include "stdlib.h"

// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;
float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;
bool thirdPersonView = true;
vec3 gOrientationOld;

void computeMatricesFromInputs(glm::mat4 ModelMatrix, vec3 gOrientation1){
    
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );
    
    
//	// Move forward
//	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
//		position += direction * deltaTime * speed;
//	}
//	// Move backward
//	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
//		position -= direction * deltaTime * speed;
//	}
//	// Strafe right
//	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
//		position += right * deltaTime * speed;
//	}
//	// Strafe left
//	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
//		position -= right * deltaTime * speed;
//	}
    
    
    // Make camera follow the model like a character in a video game
    
    if (glfwGetKey( window, GLFW_KEY_J ) == GLFW_PRESS){
        // Set Camera Orientation to that of the head of the cat
        horizontalAngle = gOrientation1.y;
        verticalAngle = gOrientation1.x;
        thirdPersonView = false;

    }
    
    if (glfwGetKey( window, GLFW_KEY_K ) == GLFW_PRESS){
        // Reset Camera Orientation to that of its orignial position to regain 3rd person perspective of the cat
        horizontalAngle = 3.14f;
        verticalAngle = 0.0f;
        thirdPersonView = true;
    }
    
    // Accesss the 3D world coordinates of the object and set camera to these plus an offset in the Z-Direction to have a following camera
    
    if (thirdPersonView) {
        position = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1] + 1, ModelMatrix[3][2] + 2);
    } else {
        position = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1] + 0.8f, ModelMatrix[3][2]);
        
        if(gOrientationOld.y > gOrientation1.y) {
            horizontalAngle -= 0.1f;
        }
        if(gOrientationOld.y < gOrientation1.y) {
            horizontalAngle += 0.1f;
        }
        if(gOrientationOld.x > gOrientation1.x) {
            verticalAngle += 0.2f;
        }
        if(gOrientationOld.x < gOrientation1.x) {
            verticalAngle -= 0.2f;
        }
    }
    

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
    gOrientationOld = gOrientation1;

}
