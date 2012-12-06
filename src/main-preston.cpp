#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <iostream>
#include <chrono>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

#include <ImfRgbaFile.h>
#include <ImfIO.h>
#include <drawImage.h>

using namespace Imf;
using namespace Imath;

//--Data types
//This object will define the attributes of a vertex(position, texture coordinates)
struct Vertex
{
    GLfloat position[3];
    glm::vec2 texCoord;
};

class Texture
{
 public:
    Texture(GLenum TextureTarget, const std::string& FileName)
    {
     m_textureTarget = TextureTarget;
     m_fileName = FileName;
    };

    bool Load()
    {
     // load file
     std::string fileName = "checker.jpg";
     Array2D<Rgba> pixels;
     int width, height;
     readRgba1(fileName.c_str(), pixels, width, height);
     const void* imgData;
     imgData = pixels;
  
     glGenTextures(1, &m_textureObj);
     glBindTexture(m_textureTarget, m_textureObj);
     glTexImage2D(m_textureTarget, 0, GL_RGB, height, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
     glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
     return true;
    };

    void Bind(GLenum TextureUnit)
    {
     glActiveTexture(TextureUnit);
     glBindTexture(m_textureTarget, m_textureObj);
    };
    
    void readRgba1 (const char fileName[], Array2D<Rgba> &pixels, int &width, int &height)
    // from openexr-1.7.0/doc/ReadingAndWritingImageFiles.pdf page 6
    {
      RgbaInputFile file (fileName);
      Box2i dw = file.dataWindow();
      width = dw.max.x - dw.min.x + 1;
      height = dw.max.y - dw.min.y + 1;
      pixels.resizeErase (height, width);
      file.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
      file.readPixels (dw.min.y, dw.max.y);
    }

 private:
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
};

// Global variables

int w = 640, h = 480;// Window size
GLuint program;// The GLSL program handle
GLuint vbo_geometry;// VBO handle for our geometry
Vertex *geometry=NULL;// Pointer to geometry
int vertexCount=0;// Vertex count of geometry

//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader

//attribute locations
GLint loc_position;
GLint loc_tex;

//transform matrices
glm::mat4 model;//obj->world each object should have its own model matrix
glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 mvp;//premultiplied modelviewprojection

//--GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);

//--Load Obj
bool loadObj(const char *filename, Vertex* &obj, int &vertexCount);

//--Resource management
bool initialize();
void cleanUp();

//--Time Difference
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

// Texture
//tex = new Texture(GL_TEXTURE_2D, "checker.jpg");

//--Main
int main(int argc, char **argv)
{
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);

    // Name and create the Window
    glutCreateWindow("ASSIMP Textured Buddha");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);// Called when its time to display
    glutReshapeFunc(reshape);// Called if the window is resized
    glutIdleFunc(update);// Called if there is nothing else to do
    glutKeyboardFunc(keyboard);// Called if there is keyboard input

    // Initialize all of our resources(shaders, geometry)
    bool init = initialize();
    if(init)
    {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    
    return 0;
}

//--Implementations
void render()
{
    //--Render the scene

    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //premultiply the matrix for this example
    mvp = projection * view * model;

    //enable the shader program
    glUseProgram(program);

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_tex);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //set pointers into the vbo for each of the attributes
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           (void*)offsetof(Vertex,position));//offset

    glVertexAttribPointer( loc_tex,
                           2,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,texCoord));

    // bind texture
    //tex->Bind(GL_TEXTURE0);
    
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_tex);
                           
    //swap the buffers
    glutSwapBuffers();
}

void update()
{
    //total time
    static float angle = 0.0;
    float dt = getDT();// if you have anything moving, use dt.

    // do stuff

    // Update the state of the scene
    glutPostRedisplay();//call the display callback
}


void reshape(int n_w, int n_h)
{
    w = n_w;
    h = n_h;
    //Change the viewport to be correct
    glViewport( 0, 0, w, h);
    //Update the projection matrix as well
    //See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);

}

void keyboard(unsigned char key, int x_pos, int y_pos)
{
    // Handle keyboard input
    if(key == 27)//ESC
    {
        exit(0);
    }
}

bool loadObj(const char *filename, Vertex* &obj, int &vertexCount)
{
 Assimp::Importer importer;

 if(obj)
 {
        std::cerr << "[F] loadObj function used incorrectly." << std::endl;
        return false;
 }

 //load the file and make sure all polygons are triangles
 const aiScene *scene = importer.ReadFile(filename,aiProcess_Triangulate );

 if(!scene)
  return false;

 //get vertices from assimp
 aiVector3D *vertices = (*scene->mMeshes)->mVertices;

 //get vertex count from assimp
 vertexCount = (*scene->mMeshes)->mNumVertices;
 
 // get texture coordinates from assimp
 aiVector3D *texCoords = (*scene->mMeshes)->mTextureCoords[vertexCount];

 //allocate memory for obj
 obj = new Vertex[vertexCount];

 //add vertex, normals, and UVs to mesh object
 for(int i=0;i<vertexCount;++i)
 {
  obj[i].position[0] = vertices[i].x;
  obj[i].position[1] = vertices[i].y;
  obj[i].position[2] = vertices[i].z;

  if(scene->HasTextures()) {
   obj[i].texCoord[0] = texCoords[i].x;
   obj[i].texCoord[1] = texCoords[i].y;
  }
 }

 return true;
}

bool initialize()
{
    // Initialize basic geometry and shaders for this example

    //this defines a cube, this is why a model loader is nice
    //you can also do this with a draw elements and indices, try to get that working
    std::cout << "Obj file is loading this might take a moment. Please wait." << std::endl;
    if(!loadObj("buddha.obj", geometry, vertexCount))
     {
        std::cerr << "[F] The obj file did not load correctly." << std::endl;
        return false;
     }

    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, vertexCount*sizeof(Vertex), geometry, GL_STATIC_DRAW);

    //--Geometry done

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    //Shader Sources
    // Put these into files and write a loader in the future
    // Note the added uniform!
    const char *vs =
        "attribute vec3 v_position;"
        "attribute vec2 TexCoord;"
        "varying vec2 TexCoord0;"
        "uniform mat4 mvpMatrix;"
        "void main(void){"
        " gl_Position = mvpMatrix * vec4(v_position, 1.0);"
        " TexCoord0 = TexCoord;"
        "}";

    const char *fs =
        "varying vec2 TexCoord0;"
        "uniform sampler2D gSampler;"
        "void main(void){"
        " gl_FragColor = texture2D(gSampler, TexCoord0.xy);"
        "}";

    //compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(vertex_shader, 1, &vs, NULL);
    glCompileShader(vertex_shader);
    //check the compile status
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
    
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
        return false;
    }

    // Now the Fragment shader
    glShaderSource(fragment_shader, 1, &fs, NULL);
    glCompileShader(fragment_shader);
    //check the compile status
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
        return false;
    }

    //Now we link the 2 shader objects into a program
    //This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    //check if everything linked ok
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        return false;
    }

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_tex = glGetAttribLocation(program,
                    const_cast<const char*>("TexCoord"));
    if(loc_tex == -1)
    {
        std::cerr << "[F] TEXCOORD NOT FOUND" << std::endl;
        return false;
    }

    loc_mvpmat = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat == -1)
    {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }
    
    //--Init the view and projection matrices
    // if you will be having a moving camera the view matrix will need to more dynamic
    // ...Like you should update it before you render more dynamic
    // for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   100.0f); //Distance to the far plane,

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //and its done
    return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_geometry);
}

//returns the time delta
float getDT()
{
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}
