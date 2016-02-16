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

using namespace std;

Bone::Bone()
{
    printf("Object is being created\n");
}

void Bone::updateRoot(float rotation, glm::mat4 ParentModelMatrix, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    
    // Rotate bone's model matrix
    this->ModelMatrix = glm::rotate(ParentModelMatrix, rotation, glm::vec3(1, 0, 0));
    
    // Apply new model matrix to bone's mvp
    this->MVP = ProjectionMatrix * ViewMatrix * this->ModelMatrix;
    
    // if the bone has a child, update the child with the current bone's mvp
    if(this->hasChild()) {
        this->updateChild(this->MVP);
    } else if (this->hasParent()) {
//        child.MVP = root.MVP*child.ModelMatrix;
        this->MVP = this->parent->MVP * this->ModelMatrix;
    }

}

void Bone::updateChild(glm::mat4 ParentMVP) {
    
//    this->child->MVP = ParentMVP * this->ModelMatrix;
    this->child->MVP = ParentMVP * this->child->ModelMatrix;

    
    if(this->child->hasChild()) {
        this->child->updateChild(this->ModelMatrix);
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
