#include <iostream>
#include <string>
#include <cstdlib>

#include <cmath>
#include "Tree.h"

// const float M_PI = 3.14159265358979323846; // Uncomment if you're using vs

Tree::Tree(std::string inputAxiom): axiom(inputAxiom){}

void Tree::MakeAxiom(int depth){
    while (depth--){
        for (int i = 0; i < axiom.length(); ++i) {
            if (axiom[i] == 'F'){
                axiom.replace(i, 1, "FF");
                i++;
            }
            if (axiom[i] == 'X'){
                axiom.replace(i, 1, "F+[-F-XF-X][+FF][--XF[+X]][++F-X]");
                i += 32;
            }
        }
    }
}


int Tree::CountParts(std::string& axiom, char symbol){
    int count(0);
    for (auto action: axiom)
        if (action == symbol)
            count++;
    return count;
}


void Tree::MakeBranchesCoordsArray(){
    branchIndex = 0;
    numOfBranches = CountParts(axiom, 'F');
    branchesCoords = new glm::vec2 [numOfBranches * 2];
}


void Tree::MakeLeavesCoordsArray(){
    leafIndex = 0;
    numOfLeaves = CountParts(axiom, ']') + 1;
    leavesCoords = new glm::vec2 [numOfLeaves * 6];
}


void Tree::MakeBranch(glm::vec2& coords, float angle){

    float x2 = coords.x + cos(angle * M_PI / 180.f);
    float y2 = coords.y + sin(angle * M_PI / 180.f);
    

    branchesCoords[branchIndex++] = coords;
    branchesCoords[branchIndex++] = coords = glm::vec2(x2, y2);
}


void Tree::MakeLeaf(float x1, float y1, float angle){
    float x2 = x1 + cos(angle * M_PI / 180.f);
    float y2 = y1 + sin(angle * M_PI / 180.f);
    
    float xleft = x1 + 0.5 * cos((angle + 45) * M_PI / 180.f);
    float yleft = y1 + 0.5 * sin((angle + 45) * M_PI / 180.f);

    float xright = x1 + 0.5 * cos((angle - 45) * M_PI / 180.f);
    float yright = y1 + 0.5 * sin((angle - 45) * M_PI / 180.f);

    leavesCoords[leafIndex++] = glm::vec2(x1, y1);
    leavesCoords[leafIndex++] = glm::vec2(xleft, yleft);
    leavesCoords[leafIndex++] = glm::vec2(x2, y2);
    leavesCoords[leafIndex++] = glm::vec2(x1, y1);
    leavesCoords[leafIndex++] = glm::vec2(xright, yright);
    leavesCoords[leafIndex++] = glm::vec2(x2, y2);
}


void Tree::DrawTree(std::string& axiom, 
                    glm::vec2 initialCoords, 
                    float angle) {

    float additional(0);
    glm::vec2 newCoords = initialCoords;
    for (int i(0); i < axiom.length(); ++i){
        if (axiom[i] == 'F'){
            Tree::MakeBranch(newCoords, angle + additional); 
        }
        if (axiom[i] == '+') {
            additional += 17;
        }
        if (axiom[i] == '-') {
            additional -= 17;
        }
        if (axiom[i] == '[') {
            std::string newAxiom = "";
            int count(1);
            ++i;
            while (true){
                if  (axiom[i] == '['){
                    count++;
                }
                if (axiom[i] == ']'){
                    count--;
                }
                if (!count) break;
                else
                    newAxiom += axiom[i++];
            }
            DrawTree(newAxiom, newCoords, angle + additional);
        }
        
    }
    Tree::MakeLeaf(newCoords.x, newCoords.y, angle + additional);
}
