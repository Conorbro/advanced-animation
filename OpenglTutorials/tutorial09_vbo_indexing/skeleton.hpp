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

#include "bone.hpp"

class Skeleton {
public:
    int num_bones;
    Bone root;
    std::vector<Bone> bones;
    Skeleton(Bone root_bone);
    void addBone(Bone &b);
    void querySkeleton();
    void bindDraw(GLuint vertexbuffer,
                  GLuint uvbuffer,
                  GLuint normalbuffer,
                  GLuint elementbuffer,
                  std::vector<unsigned short> indices);
};

#endif /* skeleton_hpp */
