#include<sstream>
#include<fstream>
#include<iostream>
namespace RDAGLTFParser{
    class Gltf{
private:
        struct Attributes{
            float* positionVectors,*normals,*UV_0_Coords;
            int positionVectorsCount,normalsCount,UV_0_CoordsCount;
            ~Attributes(){
                delete[] positionVectors;
                delete[] normals;
                delete[] UV_0_Coords;
            }
        };
        struct Primitvies
        {
            Attributes* attributes;
            unsigned short* indices;
            int indicesCount;
            ~Primitvies(){
                delete attributes;
                delete[] indices;
            }
        };
        struct Mesh
        {
            const char* name;
            Primitvies* primitvies;
            ~Mesh(){
                delete primitvies;
            }
        };
        struct Node{
            const char * name;
            Mesh* mesh;
            ~Node(){
                delete mesh;
            }
        };
        struct Scene{
            const char* name;
            Node* nodes;
            int nodesCount;
            ~Scene(){
                delete[] nodes;
            }
        };
        Scene* scene;
public:
        Gltf(const char* path);
    };
}