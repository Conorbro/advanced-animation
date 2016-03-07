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

void Bone::update(float rotation, glm::vec3 translation) {
    
    // Rotate bone's model matrix
    this->position += translation;
    this->ModelMatrix = glm::rotate(this->ModelMatrix, rotation, glm::vec3(0, 0, 1));
    if(this->hasParent()) {
        this->MVP = this->parent->MVP * this->ModelMatrix;
    }
    
    // If the bone has a child, update the child with the current bone's mvp
    if(this->hasChild()) {
        this->updateChild(this->MVP);
    }

}

void Bone::updateChild(glm::mat4 ParentMVP) {
    
    this->child->MVP = ParentMVP * this->child->ModelMatrix;
    
    if(this->child->hasChild()) {
        this->child->updateChild(this->child->MVP);
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
