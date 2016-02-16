//
//  bone.hpp
//  Tutorials
//
//  Created by Conor Broderick on 08/02/2016.
//
//

#ifndef bone_hpp
#define bone_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glfw3.h>
#include <vector>

using namespace std;

class Bone {
public:
    int id;
    int parentRef;
    std::vector<Bone*> childRefs;
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = glm::mat4(1.0);
    Bone* child;
    void addChild(Bone *bone);
    void updateRoot(float rotation, glm::mat4 ParentModelMatrix);
    void updateChild(glm::mat4 ParentModelMatrix);
    bool hasChild();
    Bone();
};



#endif /* bone_hpp */
