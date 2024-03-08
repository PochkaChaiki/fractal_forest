#ifndef _TREE_
#define _TREE_


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Tree{
public:
    std::string axiom;
    glm::vec2* branchesCoords;
    glm::vec2* leavesCoords;
    int numOfBranches, numOfLeaves;
    int branchIndex, leafIndex;
    
    Tree(std::string inputAxiom);

    void MakeAxiom(int depth);
    void MakeBranchesCoordsArray();
    void MakeLeavesCoordsArray();
    
    void DrawTree(std::string& axiom, glm::vec2 initialCoords, float angle);

private:
    void MakeBranch(glm::vec2& coords, float angle);
    void MakeLeaf(float x1, float y1, float angle);
    int CountParts(std::string& axiom, char symbol);
};



#endif