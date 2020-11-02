#include<sstream>
#include<fstream>
namespace RDAGLTFParser{
    class Gltf{
private:
        struct Attributes{
            float *positionVectors,*normals,*UV_0_Coords;
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
            std::string name;
            Primitvies* primitvies;
            ~Mesh(){
                delete primitvies;
            }
        };
        struct Node{
            std::string name;
            Mesh* mesh;
            ~Node(){
                delete mesh;
            }
        };
        struct Scene{
            std::string name;
            Node* nodes;
            int nodesCount;
            ~Scene(){
                delete[] nodes;
            }
        };
        Scene* scene;
public:
        Gltf(const char* path);
        ~Gltf();
        float* getPositionVectors(unsigned int& count,int nodeIndex);
        float* getPositionVectors(unsigned int& count,const char* nodeName); 
        float* getNormals(unsigned int& count,int nodeIndex);
        float* getNormals(unsigned int & count,const char* nodeName);
        float* getUV0Coords(unsigned int& count,int nodeIndex);
        float* getUV0Coords(unsigned int& count,const char* nodeName);
        unsigned short* getFacesIndices(unsigned int& count,int nodeIndex);
        unsigned short* getFacesIndices(unsigned int& count,const char* nodeName);
    };
}