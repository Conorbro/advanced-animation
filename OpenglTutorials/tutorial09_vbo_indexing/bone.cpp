//
//  bone.cpp
//  Tutorials
//
//  Created by Conor Broderick on 08/02/2016.
//
//

#include "bone.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_access.hpp>
using namespace glm;

using namespace std;

Bone::Bone()
{
//    printf("Object is being created\n");
}

void Bone::update(float rotation, glm::vec3 rotationAxis) {

    // Rotate bone's model matrix
    ModelMatrix = glm::rotate(ModelMatrix, rotation, rotationAxis);
    if(hasParent()) {
        MVP = parent->MVP * ModelMatrix;
        ModelMatrixTemp = parent->ModelMatrix * ModelMatrix;
        
//        glm::vec3 currBonePosition = glm::vec3(ModelMatrixTemp[3]);
//          cout << "Curr Joint Position from MM Bone" << this->id << " " << currBonePosition[0] << " " << currBonePosition[1] << " "  << currBonePosition[2] << endl;
    
    }
    
    // If the bone has a child, update the child with the current bone's mvp
    if(hasChild()) {
        updateChild(MVP, ModelMatrix);
    }

}

void Bone::updateChild(glm::mat4 ParentMVP, glm::mat4 ParentModelMatrix) {
    
    child->MVP = ParentMVP*child->ModelMatrix;
    
    child->ModelMatrixTemp = ParentModelMatrix * child->ModelMatrix;

//    glm::vec3 currBonePosition = glm::vec3(child->ModelMatrixTemp[3]);
//    cout << "Curr Joint Position from MM Child" << child->id << " " << currBonePosition[0] << " " << currBonePosition[1] << " "  << currBonePosition[2] << endl;
    
    if(child->hasChild()) {
        child->updateChild(child->MVP, child->ModelMatrixTemp);
    }
}

bool Bone::hasChild() {
    if (this->child) {
        return true;
    } else return false;
}

bool Bone::hasParent() {
    if (this->parent) {
        return true;
    } else return false;
}

void Bone::addChild(Bone *bone) {
    this->child = bone;
    childRefs.push_back(bone);
}

void Bone::addParent(Bone *bone) {
    this->parent = bone;
}
