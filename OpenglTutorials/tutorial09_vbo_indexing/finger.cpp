//
//  Finger.cpp
//  Tutorials
//
//  Created by Conor Broderick on 08/02/2016.
//
//
#include <iostream>
#include "finger.hpp"
#include "bone.hpp"

//Graphics Imports
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

// classes example
using namespace std;

Finger::Finger(Bone root_bone) {
    num_bones = 0;
    root = root_bone;
    addBone(root);
    root.id = num_bones;
}

void Finger::addBone(Bone &b) {
    num_bones += 1;
    b.id = num_bones;
    if (num_bones <= 1){
        b.parentRef = num_bones - 1; // assign -1 as parent ref for root bone
    } else b.parentRef = -1;
    bones.push_back(&b);
//    printf("Bone id = %i\n", b.id);
//    printf("Bone Parent = %i\n", b.parentRef);
    
}

void Finger::queryFinger() {
    printf("Number of Bones in Finger = %i\n", num_bones);
    printf("Root bone id = %i\n", root.id);
    for(int i = 0; i<bones.size(); i++) {
//        printf("Bone List Item: %i\n", bones[i].id);
    }
}

void Finger::bindDraw(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, GLuint elementbuffer,std::vector<unsigned short> indices) {
    
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
