#include "include.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void FrameBufferSizeCallback( GLFWwindow* _window UNUSED, int _w, int _h )
{
    glViewport( 0, 0, _w, _h );
} 

GLFWwindow* CreateWindow( int _w, int _h, const char* _title )
{
    GLFWwindow* window = glfwCreateWindow( _w, _h, _title, NULL, NULL );
    if ( !window )
    {
        printf( "GLFW Window Failed to Create!\n" );
        glfwTerminate( );
        exit( 1 );
    }

    glfwSetFramebufferSizeCallback( window, FrameBufferSizeCallback );

    if ( !window )
    {
        printf( "ERROR Window Not Yet Created!\n" );
        glfwTerminate( );
        exit( 1 );
    }

    glfwMakeContextCurrent( window );

    return window;
}

void GLADinit( )
{
    if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        printf( "GLAD Failed To INIT!\n" );
        glfwTerminate( );
        exit( 1 );
    }
}

void GLFWinit( )
{
    // init GLFW
    if ( !glfwInit() )
    {
        printf("GLFW Failed To INIT!\n");
        exit( 1 );
    }

    // set GLFW window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int main( int argc UNUSED, char** argv UNUSED )
{
    GLFWinit( );
    GLFWwindow* window = CreateWindow( 640, 480, "hello world" );
    GLADinit( );
    // Shader.
    unsigned int shaderProgram = createShader( "res/shaders/texture" );

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float vertices[] = {
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f, //0
         1.0f, -1.0f,  1.0f, 1.0f, 0.0f, //1
        -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, //2
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, //3
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f, //4
         1.0f, -1.0f, -1.0f, 1.0f, 0.0f, //5
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, //6
        -1.0f,  1.0f, -1.0f, 1.0f, 1.0f  //7
    };


    unsigned int indices[] = {
        0, 1, 3, 1, 2, 3,
        4, 5, 0, 5, 1, 0,
        7, 6, 4, 6, 5, 4,
        3, 2, 7, 2, 6, 7,
        4, 0, 7, 0, 3, 7,
        5, 1, 6, 1, 2, 6
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO); 

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // pos atr
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    // tex atr
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //init texture
    unsigned int tex;
    glGenTextures( 1, &tex );
    glBindTexture( GL_TEXTURE_2D, tex );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    int width, height, nrChannels;
    unsigned char* data = stbi_load( "res/img/wall.jpg", &width, &height,
            &nrChannels, 0 );
    
    if ( data )
    {
        //printf( "%s\n", data );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );
    }
    else
    {
        printf( "Failed to load texture\n" );
    }
    stbi_image_free( data );



    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so afterwards
    // we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound
    // element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally
    // modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't
    // unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    glEnable( GL_DEPTH_TEST );
    glUseProgram(shaderProgram);
    glUniform1i( glGetUniformLocation( shaderProgram, "uTex" ), 0 );


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    mat4x4 projection, model, view;

    mat4x4_perspective( projection, 0.785398f, 480.0f/640.0f, 0.1f, 100.0f  );
    mat4x4_identity( model );
    //mat4x4_scale( model, model, 0.5f );
    mat4x4_rotate( model, model, 1.0f, 0.0f, 0.0f, -0.959931 );
    mat4x4_identity( view );
    mat4x4_translate( view, 0.0f, 0.0f, -6.0f );
    
    //mat4x4_translate( transform, 0.5f, -0.5f, 0.0f );

    ////////////
    while ( !glfwWindowShouldClose( window ) )
    {
        glClearColor( 0.8f, 0.2f, 0.4f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // draw our first triangle
        glActiveTexture(GL_TEXTURE0);
        glBindTexture( GL_TEXTURE_2D, tex );

        mat4x4_rotate( model, model, 0.5f, 1.0f, 0.0f, 0.01f );

        glUseProgram(shaderProgram);
        int width, height;
        glfwGetFramebufferSize( window, &width, &height );
        glUniform2f( glGetUniformLocation( shaderProgram, "uResolution" ),
                (float)width, (float)height );

        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "uProjection" ),
                1, GL_FALSE, &projection[0][0]  );
        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "uModel" ),
                1, GL_FALSE, &model[0][0]  );
        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "uView" ),
                1, GL_FALSE, &view[0][0]  );
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's
                                // no need to bind it every time, but we'll do
                                // so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released,
        // mouse moved etc.)
        // --------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow( window );
    glfwTerminate( );
    return 0;
}
