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
            while(gltfFile[i]!=',') i++;
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
Gltf::Gltf(const char* path){
    fstream fileStream(path,ios::in);
    stringstream stringStream;
    string gltfFile;
    if(fileStream.is_open()){
        stringStream<<fileStream.rdbuf();
        gltfFile=stringStream.str();
        fileStream.close();
    }else throw runtime_error("Couldn't open an GLTF file");
    cout<<getArrayIndicesCount(gltfFile,1328);
    
}