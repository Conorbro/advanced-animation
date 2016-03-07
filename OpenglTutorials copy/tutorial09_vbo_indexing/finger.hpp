//
//  finger.hpp
//  Tutorials
//
//  Created by Conor Broderick on 08/02/2016.
//
//

#ifndef finger_hpp
#define finger_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glfw3.h>
#include <vector>

#include "bone.hpp"

class Finger {
public:
    int num_bones;
    Bone root;
    std::vector<Bone*> bones;
    Finger(Bone root_bone);
    void addBone(Bone &b);
    void queryFinger();
    void bindDraw(GLuint vertexbuffer,
                  GLuint uvbuffer,
                  GLuint normalbuffer,
                  GLuint elementbuffer,
                  std::vector<unsigned short> indices);
};

#endif /* Finger_hpp */
