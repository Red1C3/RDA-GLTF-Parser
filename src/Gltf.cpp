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

#include<RDAGLTFParser/Gltf.h>
using namespace RDAGLTFParser;
using namespace std;
static inline int getElementOffsetOutsideArray(const char* element,const string& gltfFile){
    int elementLength=strlen(element);
    int elementOffset=0;
    for(int i=0;i<gltfFile.length();i++){
        if(gltfFile[i]=='['){
            while(gltfFile[i]!=']') i++;
        }
        if(gltfFile[i]==element[elementOffset]){
            ++elementOffset;
            if(elementOffset==elementLength){
                return i+2;
            }
        }else if(elementOffset!=0){
            elementOffset=0;
        }
    }
    return INT_MAX;
}
static inline int getElementOffsetInsideArray(const char* element,const string& gltfFile,int rootOffset,int index){
    int currentIndex=0,elementLength=strlen(element),elementOffset=0;
    for(int i=rootOffset;i<gltfFile.length();i++){
        if(index!=currentIndex){
            while(gltfFile[i]!=','){
                if(gltfFile[i]=='{'){
                    while(gltfFile[i]!='}') i++;
                }
                i++;
            }
            currentIndex++;
            continue;
        }
        if(gltfFile[i]==element[elementOffset]){
            ++elementOffset;
            if(elementOffset==elementLength){
                return i+2;
            }
        }else if(elementOffset!=0){
            elementOffset=0;
        }
    }
    return INT_MAX;
}
static inline int getArrayIndicesCount(const string& gltfFile,int rootOffset){
    int indices=1;
    for(int i=rootOffset;i<gltfFile.length();i++){
        if(gltfFile[i]==']') return indices;
        if(gltfFile[i]=='{'){
            while(gltfFile[i]!='}') i++;
        }
        if(gltfFile[i]==',') indices++;
    }
    return INT_MAX;
}
static inline int toInt(string& element){
    for(string::iterator i = element.begin();i<element.end();i++){
        if(*i==' ' || *i=='\n'){
            element.erase(i,element.end());
            break;
        }
    }
    int length=element.length();
    int goal=0;
    bool isNegative=false;
    int i=0;
    if(element[0]=='-'){
        isNegative=true;
        i=1;
    }
    for(;i<length;i++){
        goal+=((int)element[i]-48)*(pow(10,length-i-1)); 
    }
    if(isNegative) goal*=-1;
    return goal;
}
static inline string getElementData(const string& gltfFile,int offset){
    string data="";
    int i=offset;
    for(;gltfFile[i]==' '||gltfFile[i]==':'||gltfFile[i]==','||gltfFile[i]=='{'||gltfFile[i]=='[';i++);
    for(;gltfFile[i]!=','&&gltfFile[i]!=']'&&gltfFile[i]!='}';i++){
        data.insert(data.end(),gltfFile[i]);
    }
    if(data[0]=='\"') data=string(data.begin()+1,data.end());
    for(string::iterator i = data.begin();i<data.end();i++){
        if(*i=='\"') {data.erase(i,data.end()); break;}
    }
    return data;
}
static inline char* readDataFromAccessor(int accessor,int& count,const string& gltfFile,const string& path){
    int accessorsOffset=getElementOffsetOutsideArray("accessors",gltfFile);
    int bufferViewsOffset=getElementOffsetOutsideArray("bufferViews",gltfFile);
    int buffersOffset=getElementOffsetOutsideArray("buffers",gltfFile);
    count=toInt(getElementData(gltfFile,getElementOffsetInsideArray("count",gltfFile,accessorsOffset,accessor)));
    int bufferViewIndex=toInt(getElementData(gltfFile,getElementOffsetInsideArray("bufferView",gltfFile,accessorsOffset,accessor)));
    int bufferIndex=toInt(getElementData(gltfFile,getElementOffsetInsideArray("buffer",gltfFile,bufferViewsOffset,bufferViewIndex)));
    int byteLength=toInt(getElementData(gltfFile,getElementOffsetInsideArray("byteLength",gltfFile,bufferViewsOffset,bufferViewIndex)));
    int byteOffset=toInt(getElementData(gltfFile,getElementOffsetInsideArray("byteOffset",gltfFile,bufferViewsOffset,bufferViewIndex)));
    int bufferLength=toInt(getElementData(gltfFile,getElementOffsetInsideArray("byteLength",gltfFile,buffersOffset,bufferIndex)));
    string fileName=getElementData(gltfFile,getElementOffsetInsideArray("uri",gltfFile,buffersOffset,bufferIndex));
    char* data= new char[byteLength];
    fstream fStream(path+fileName,ios::in|ios::binary);
    if(fStream.is_open()){
        fStream.seekg(byteOffset,ios::beg);
        fStream.read(data,byteLength);
        fStream.close();
    }else{
        throw runtime_error("Couldn't open a bin file");
    }
    return data;
    }
Gltf::Gltf(const char* path){
    string gltfFilePath=string(path);
    for(string::iterator i=gltfFilePath.end();i!=gltfFilePath.begin();i--){
        if(*i=='/'){
            gltfFilePath.erase(i+1,gltfFilePath.end());
        }
    }
    fstream fileStream(path,ios::in);
    stringstream stringStream;
    string gltfFile;
    if(fileStream.is_open()){
        stringStream<<fileStream.rdbuf();
        gltfFile=stringStream.str();
        fileStream.close();
    }else throw runtime_error("Couldn't open a GLTF file");
    scene = new Scene();
    scene->name=getElementData(gltfFile,getElementOffsetInsideArray("name",gltfFile,getElementOffsetOutsideArray("scenes",gltfFile),0));
    scene->nodesCount=getArrayIndicesCount(gltfFile,getElementOffsetOutsideArray("nodes",gltfFile));
    scene->nodes=new Node[scene->nodesCount];
    for(int i=0;i<scene->nodesCount;i++){
        scene->nodes[i].name=getElementData(gltfFile,getElementOffsetInsideArray("name",gltfFile,getElementOffsetOutsideArray("nodes",gltfFile),i));
        scene->nodes[i].mesh=new Mesh();
        int meshIndex=toInt(getElementData(gltfFile,getElementOffsetInsideArray("mesh",gltfFile,getElementOffsetOutsideArray("nodes",gltfFile),i)));
        scene->nodes[i].mesh->name=getElementData(gltfFile,getElementOffsetInsideArray("name",gltfFile,getElementOffsetOutsideArray("meshes",gltfFile),meshIndex));
        scene->nodes[i].mesh->primitvies=new Primitvies();
        scene->nodes[i].mesh->primitvies->attributes=new Attributes();
        scene->nodes[i].mesh->primitvies->indices=(unsigned short*) readDataFromAccessor(
            toInt(getElementData(gltfFile,getElementOffsetInsideArray("indices",gltfFile,getElementOffsetInsideArray("primitives",gltfFile,getElementOffsetOutsideArray("meshes",gltfFile),meshIndex),0))),
            scene->nodes[i].mesh->primitvies->indicesCount,gltfFile,gltfFilePath
        );
        scene->nodes[i].mesh->primitvies->attributes->positionVectors=(float*) readDataFromAccessor(
            toInt(getElementData(gltfFile,getElementOffsetInsideArray("POSITION",gltfFile,getElementOffsetInsideArray("primitives",gltfFile,getElementOffsetOutsideArray("meshes",gltfFile),meshIndex),0))),
            scene->nodes[i].mesh->primitvies->attributes->positionVectorsCount,gltfFile,gltfFilePath
        );
        scene->nodes[i].mesh->primitvies->attributes->normals=(float*)readDataFromAccessor(
            toInt(getElementData(gltfFile,getElementOffsetInsideArray("NORMAL",gltfFile,getElementOffsetInsideArray("primitives",gltfFile,getElementOffsetOutsideArray("meshes",gltfFile),meshIndex),0))),
            scene->nodes[i].mesh->primitvies->attributes->normalsCount,gltfFile,gltfFilePath
        );
        scene->nodes[i].mesh->primitvies->attributes->UV_0_Coords=(float*) readDataFromAccessor(
            toInt(getElementData(gltfFile,getElementOffsetInsideArray("TEXCOORD_0",gltfFile,getElementOffsetInsideArray("primitives",gltfFile,getElementOffsetOutsideArray("meshes",gltfFile),meshIndex),0))),
            scene->nodes[i].mesh->primitvies->attributes->UV_0_CoordsCount,gltfFile,gltfFilePath
        );
    }
}
float* Gltf::getPositionVectors(unsigned int & count,int nodeIndex){
    if(nodeIndex>=scene->nodesCount) throw runtime_error("Invalid node index was inserted while getting position vectors");
    count=scene->nodes[nodeIndex].mesh->primitvies->attributes->positionVectorsCount;
    return scene->nodes[nodeIndex].mesh->primitvies->attributes->positionVectors;
}
float* Gltf::getPositionVectors(unsigned int& count,const char* nodeName){
    for(int i=0;i<scene->nodesCount;i++){
        if(strcmp(nodeName,scene->nodes[i].name.c_str())==0){
            return getPositionVectors(count,i);
        }
    }
    throw runtime_error("Invalid node name was inserted while getting position vectors");
}
float* Gltf::getNormals(unsigned int& count,int nodeIndex){
    if(nodeIndex>=scene->nodesCount) throw runtime_error("Invalid node index was inserted while getting normal vectors");
    count=scene->nodes[nodeIndex].mesh->primitvies->attributes->normalsCount;
    return scene->nodes[nodeIndex].mesh->primitvies->attributes->normals;
}
float* Gltf::getNormals(unsigned int& count,const char* nodeName){
    for(int i=0;i<scene->nodesCount;i++){
        if(strcmp(nodeName,scene->nodes[i].name.c_str())==0){
            return getNormals(count,i);
        }
    }
    throw runtime_error("Invalid node name was inserted while getting normal vectors");
}
float* Gltf::getUV0Coords(unsigned int& count,int nodeIndex){
    if(nodeIndex>=scene->nodesCount) throw runtime_error("Invalid node index was inserted while getting UVs_0");
    count=scene->nodes[nodeIndex].mesh->primitvies->attributes->UV_0_CoordsCount;
    return scene->nodes[nodeIndex].mesh->primitvies->attributes->UV_0_Coords;
}
float* Gltf::getUV0Coords(unsigned int& count,const char* nodeName){
    for(int i=0;i<scene->nodesCount;i++){
        if(strcmp(nodeName,scene->nodes[i].name.c_str())==0){
            return getUV0Coords(count,i);
        }
    }
    throw runtime_error("Invalid node name was inserted while getting UVs");
}
unsigned short* Gltf::getFacesIndices(unsigned int& count,int nodeIndex){
    if(nodeIndex>=scene->nodesCount) throw runtime_error("Invalid node index was inserted while getting Faces");
    count=scene->nodes[nodeIndex].mesh->primitvies->indicesCount;
    return scene->nodes[nodeIndex].mesh->primitvies->indices;
}
unsigned short* Gltf::getFacesIndices(unsigned int& count,const char* nodeName){
    for(int i=0;i<scene->nodesCount;i++){
        if(strcmp(nodeName,scene->nodes[i].name.c_str())==0){
            return getFacesIndices(count,i);
        }
    }
    throw runtime_error("Invalid node name was inserted while getting UVs");
}
Gltf::~Gltf(){
    delete scene;
}