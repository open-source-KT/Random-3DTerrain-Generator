#pragma once
#include "json_parser.h"

#include "renderer.h"
#include <vector>
#include "Texture.h"
#include "Random.h"
#include "shader.h"
#include "model.h"
 

struct Config
{
    int octaves = 7;
    float bias = 1.8;
    int seed = 423;

    int x = 256;
    int y = 256;
    float height = 10.0f;
    
    float posX = 0.0f;
    float posY = 0.0f;
    float offset = 0.2f;

    bool trees = false;
    int gridX = 50;
    int gridY = 50;
    std::string treeModel = "res/models/tree.obj";
    std::string treeShader = "res/shaders/simple_model.shader";
    bool instancing = false; //If you set this to true make sure to use a shader that works with instancing and vice versa
   
    float treeChance = 20; //It is 1 over this number so 1 in 20 chance




    /* This mostly determines how the index buffer will be written to and determines the default primitive in the Draw function if you don't pass any paramters.
    Note that if you set this to GL_POINTS for example and in the Draw function give a paramter of GL_TRIANGLES you will get a black screen because specifying GL_POINTS
    means that it will not generate an index buffer, if you want to have both triangles and points then specify as GL_TRIANGLES and pass GL_POINTS to Draw() when you  want to draw points*/
    GLenum primitive = GL_TRIANGLES;

    bool staticColor = false;
    bool texture = false;
    std::string textureLocation = "";
    int textureSlot = 0;
    bool textureRepeat = false;
    float xTextureRepeatOffset = 1.0f;
    float yTextureRepeatOffset = 1.0f;
    GLenum wrapMode = GL_REPEAT;

    std::vector<float> color1 = {0.317,0.149,0.149};
    std::vector<float> color2 = {0.239,0.576,0};

    //This shader doesn't take in texture coordinates it just takes colors. for textures change to a shader such as basic.shader or your own shader
    std::string shaderLocation = "res/shader/color2.shader"; 
};  

class Terrain
{
    public:

    
        Terrain(std::string config_file = "");
        ~Terrain();
        
        void init(); 
        
        inline int getVao() const {return vao;}
        inline int getIb() const {return ib;}

        void Draw(GLenum primitive = -1);

        float getTerrainHeight(float x, float y);

        float length;
        float width;
        Texture terrainTexture;
        shader terrainShader;
        shader treeShader;

    private:
        void read_config_file(std::string& name);
        float interpolateFloat(float color1, float color2, float fraction);

        unsigned int vao;
        unsigned int vbo;
        unsigned int ib;
        unsigned int modelMatrixBuffer; //If instancing for trees is true this will be the id for that buffer




        float barryCentric(std::vector<float> p1, std::vector<float> p2, std::vector<float> p3, std::vector<float> pos) const;
        

        void determineColAttrib(float *& buffer,int place);
        void determineTexAttrib(float *& buffer,int x, int y, int place);

        void indexBufferTriangles(unsigned int*& buffer);
        void indexBufferLines(unsigned int*& buffer);

        void genTerrainTrees();


        void newColors();

        Json::Value* configuration;
        Config config_struct;
        float* height_map;
        

        Model tree;
        std::vector<glm::mat4> treeModelMatrices;

        
};