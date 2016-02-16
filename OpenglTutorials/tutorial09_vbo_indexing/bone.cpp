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

void Bone::updateRoot(float rotation, glm::mat4 ParentModelMatrix) {
    
    this->ModelMatrix = glm::rotate(ParentModelMatrix, rotation, glm::vec3(1, 0, 0));
    
    if(this->hasChild()) {
        this->child->updateChild(this->MVP);
    }

}

void Bone::updateChild(glm::mat4 ParentModelMatrix) {
    
    this->MVP = ParentModelMatrix * this->ModelMatrix;
    
    if(this->hasChild()) {
        this->child->updateChild(this->ModelMatrix);
    }
}

bool Bone::hasChild() {
    if (this->child) {
        return true;
    } else return false;
}

void Bone::addChild(Bone *bone) {
    this->child = bone;
    childRefs.push_back(bone);
}
