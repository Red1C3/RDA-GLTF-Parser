/*MIT License

Copyright (c) 2020 Mohammad Issawi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

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