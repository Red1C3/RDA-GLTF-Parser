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
    
}