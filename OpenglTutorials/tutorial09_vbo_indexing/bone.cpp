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
    printf("Bone created\n");
}

void Bone::update(float rotation, glm::vec3 rotationAxis) {

    // Rotate bone's model matrix
    ModelMatrix = glm::rotate(ModelMatrix, rotation, rotationAxis);
    assert(ModelMatrix == ModelMatrix);
    if(hasParent()) {
        MVP = parent->MVP * ModelMatrix;
        ModelMatrixTemp = parent->ModelMatrix * ModelMatrix;
    }
    
    // If the bone has a child, update the child with the current bone's mvp
    if(hasChild()) {
        updateChild(MVP, ModelMatrixTemp);
    }

}

void Bone::updateChild(glm::mat4 ParentMVP, glm::mat4 ParentModelMatrix) {
    
    child->MVP = ParentMVP*child->ModelMatrix;
    
    child->ModelMatrixTemp = ParentModelMatrix * child->ModelMatrix;
    
    if(child->hasChild()) {
        child->updateChild(child->MVP, child->ModelMatrixTemp);
    }
}

void Bone::scaleBone(glm::vec3 scaleAmount) {
    this->ModelMatrix = glm::scale(this->ModelMatrix, scaleAmount);
}

bool Bone::hasChild() {
    if (child) {
        return true;
    } else return false;
}

bool Bone::hasParent() {
    if (parent) {
        return true;
    } else return false;
}

void Bone::addChild(Bone *bone) {
    child = bone;
    childRefs.push_back(bone);
}

void Bone::addParent(Bone *bone) {
    parent = bone;
}
