//
//  skeleton.hpp
//  Tutorials
//
//  Created by Conor Broderick on 08/02/2016.
//
//

#ifndef skeleton_hpp
#define skeleton_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glfw3.h>
#include <vector>

#include "finger.hpp"
#include "bone.hpp"

class Skeleton {
public:
    int num_fingers;
    Bone root;
    Skeleton();
    std::vector<Finger*> fingers;
    void addFinger(Finger &finger);
    void querySkeleton();
    void rotateSkeleton(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix);
    void bindDraw(GLuint vertexbuffer,
                  GLuint uvbuffer,
                  GLuint normalbuffer,
                  GLuint elementbuffer,
                  std::vector<unsigned short> indices);
};

#endif /* skeleton_hpp */
