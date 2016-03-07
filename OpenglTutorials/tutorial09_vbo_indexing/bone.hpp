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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GL/glew.h>
#include <glfw3.h>
#include <vector>

using namespace std;

class Bone {
public:
    int id;
    int parentRef;
    
    std::vector<Bone*> childRefs;
    glm::vec3 mScale = glm::vec3(1.0, 1.0, 1.0);
    glm::vec3 position;
    glm::vec3 length;
    glm::quat orientation;
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 ModelMatrixTemp = glm::mat4(1.0);
    glm::mat4 TranslationMatrix = glm::mat4(1.0);
    glm::mat4 MVP = glm::mat4(1.0);
    glm::vec3 global_position();
    glm::vec3 end_effector_pos();
    
    Bone* child = NULL;
    Bone* parent = NULL;
    
    void addChild(Bone *bone);
    void addParent(Bone *bone);
    void update(float rotation, glm::vec3 rotationAxis);
    void updateChild(glm::mat4 ParentMVP, glm::mat4 ParentModelMatrix);
    bool hasChild();
    bool hasParent();
    Bone();
};



#endif /* bone_hpp */
