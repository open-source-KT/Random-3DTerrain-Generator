#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "include/renderer.h"
#include "include/shader.h"
#include "include/Texture.h"
#include "include/camera.h"

#include "include/random_terrain.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



void processInput(GLFWwindow *window);
void resetColors(std::vector<glm::vec3>& oldColors);



Camera camera(glm::vec3(0.0f, 0.0f, 3.0f),glm::vec3(0.0f,1.0f,0.0f),-90.0f,0.0f,10.0f);
float lastX = 960 / 2.0f;
float lastY = 540 / 2.0f;
bool firstMouse = true;


//timing
float deltaTime = 0.0f; 
float lastFrame = 0.0f; 


bool spacePressed = false;



void mouse_callback(GLFWwindow* window, double mouseX, double mouseY)
{
	if (firstMouse)
    {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    float xoffset = mouseX - lastX;
    float yoffset = lastY - mouseY; //reversed since y-coordinates go from bottom to top

    lastX = mouseX;
    lastY = mouseY;

    camera.processMouse(xoffset, yoffset);
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
     camera.processScroll(yoffset);
}



int main(void)
{

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);  
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);   
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
    //Antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);
	
    window = glfwCreateWindow(960, 540, "Random terrain", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }



    glfwMakeContextCurrent(window);
    
    //This should fit our monitors refresh rate or whatever
    glfwSwapInterval(1);
 
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,scroll_callback);


    if(glewInit() != GLEW_OK)
    {
	    std::cout << "Error!" << std::endl;
    }	    
	

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));

    glEnable(GL_MULTISAMPLE);


    
    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
    

	GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND)); 

    {
   
    std::vector<glm::vec3> colors;
    resetColors(colors);


    Terrain terrain = Terrain("test.json");
    terrain.init();


 
	glm::mat4 view = glm::lookAt(camera.Position,camera.Position + camera.Front ,camera.Up);	

	glm::mat4 proj = glm::perspective(glm::radians(camera.fov),960/(float)540,0.1f,200.0f);
	glm::mat4 model = glm::mat4(1.0f);	
    	

    //Telling the camera to try and detect collisions. This is very rough and only detects if there is a collision when the forward button is pressed (not any other button) but it is getting there
    
    
    camera.setTerrain(&terrain);

    float planeVertices[18] = {
        0.0f,-20.0f,0.0f,
        1.0f,-20.0f,0.0f,
        1.0f,-20.0f,-1.0f,
  
        0.0f,-20.0f,0.0f,
        0.0f,-20.0f,-1.0f,
        1.0f,-20.0f,-1.0f

    };
    unsigned int planevao;
    glGenVertexArrays(1,&planevao);
    glBindVertexArray(planevao);

    unsigned int planevbo;
    glGenBuffers(1,&planevbo);
    glBindBuffer(GL_ARRAY_BUFFER,planevbo);
    glBufferData(GL_ARRAY_BUFFER,6*3*sizeof(float),planeVertices,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glm::mat4 planeModel = glm::scale(glm::mat4(1.0f),glm::vec3(400.0f,1.0f,400.0f));
    shader planeShader = shader("res/shaders/2d.shader");




    while(!glfwWindowShouldClose(window)){


	    GLCall(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),(float)glfwGetTime(),glm::vec3(1.0f,0.5f,.0f));

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(camera.Position,camera.Position + camera.Front ,camera.Up);
		glm::mat4 proj = glm::perspective(glm::radians(camera.fov),960/(float)540,0.1f,200.0f);
		
        
        terrain.terrainShader.Bind();
        terrain.terrainShader.setUniformMat4f("model",model);
        terrain.terrainShader.setUniformMat4f("proj",proj);
        terrain.terrainShader.setUniformMat4f("view",view);
        //Lighting
        terrain.terrainShader.setUniformVec3f("u_viewPos",camera.Position);
        terrain.terrainShader.setUniformVec3f("light.direction",glm::vec3(0.6f,-0.6f,-0.3f));
        terrain.terrainShader.setUniformVec3f("light.ambient",glm::vec3(0.2f,0.2f,0.2f));
        terrain.terrainShader.setUniformVec3f("light.diffuse",glm::vec3(0.5f,0.5f,0.5f));
        terrain.terrainShader.setUniformVec3f("light.specular",glm::vec3(0.4f,0.4f,0.4f));




        if(terrain.treesPresent())
        {
            terrain.treeShader.Bind();
            terrain.treeShader.setUniformMat4f("view",view);
            terrain.treeShader.setUniformMat4f("proj",proj);
        }
        
        planeShader.Bind();
        planeShader.setUniformMat4f("model",planeModel);
        planeShader.setUniformMat4f("view",view);
        planeShader.setUniformMat4f("proj",proj);
        glBindVertexArray(planevao);
        glDrawArrays(GL_TRIANGLES,0,12);

        glBindVertexArray(0);
        planeShader.UnBind();

        
        

        terrain.Draw();

        if(spacePressed)
        {
            resetColors(colors);
            terrain.newColors(colors);
            spacePressed = false;
        }

        glfwSwapBuffers(window);

        glfwPollEvents();
	    processInput(window);


     }

	}


    

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACK, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {

        spacePressed = true;
       
    }

   
    //Just for debugging
    if(glfwGetKey(window,GLFW_KEY_0)==GLFW_PRESS)
    {
        std::cout << camera.Position.y << "\n";
    }
    

};


 void resetColors(std::vector<glm::vec3>& oldColors)
    {
        for(int i=0; i < 15; i++)
        {
            oldColors.push_back(glm::vec3(randdouble(0,1),randdouble(0,1),randdouble(0,1)));
        }
    }
