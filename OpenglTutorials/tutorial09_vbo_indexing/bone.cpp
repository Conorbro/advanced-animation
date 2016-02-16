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

void Bone::update(float rotation, glm::mat4 ModelMatrix, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    
    // Rotate bone's model matrix
    this->ModelMatrix = glm::rotate(ModelMatrix, rotation, glm::vec3(1, 0, 0));
    
    // Apply new model matrix to bone's mvp
    
    if(this->hasParent()) {
        this->MVP = this->parent->MVP * this->ModelMatrix;
    }
    
    //    this->MVP = ProjectionMatrix * ViewMatrix * this->ModelMatrix;
    
    //            child.ModelMatrix = glm::rotate(child.ModelMatrix, -0.1f, glm::vec3(1, 0, 0));
    //            child.MVP = root.MVP*child.ModelMatrix; ******
    //            child2.MVP = child.MVP*child2.ModelMatrix;
    
    // If the bone has a child, update the child with the current bone's mvp
    if(this->hasChild()) {
        this->updateChild(this->MVP);
    }
//    else if (this->hasParent()) {
//        this->MVP = this->parent->MVP * this->ModelMatrix;
//    }

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
