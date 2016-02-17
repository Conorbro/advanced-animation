//
//  skeleton.cpp
//  Tutorials
//
//  Created by Conor Broderick on 08/02/2016.
//
//
#include <iostream>
#include "skeleton.hpp"
#include "bone.hpp"

//Graphics Imports
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

// classes example
using namespace std;

Skeleton::Skeleton(Bone root_bone) {
    root = root_bone;
}

void Skeleton::addFinger(Finger &f) {
    num_fingers += 1;
    fingers.push_back(&f);
}

void Skeleton::querySkeleton() {
    printf("Number of Fingers in Skeleton = %i\n", num_fingers);
    printf("Root bone id = %i\n", root.id);
}

void Skeleton::rotateSkeleton(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    for(int i=0; i<fingers.size(); i++) {
        this->fingers[i]->bones[0]->update(0.1f, fingers[i]->bones[0]->ModelMatrix, ProjectionMatrix, ViewMatrix);
    }
}

void Skeleton::bindDraw(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, GLuint elementbuffer,std::vector<unsigned short> indices) {
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    
    // Draw the triangles !
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
}
