#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

struct Texture
{
    GLuint id;
    string type;
    aiString path;
};

struct Material {
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
	float shininess;
};


class Mesh
{
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
    
    /*  Functions  */
    // Constructor
    Mesh( vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures,
			glm::vec3 materialAmbiente, glm::vec3 materialDifuso, glm::vec3 materialSpecular, GLfloat materialShininess)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
		this->ambient = materialAmbiente;
		this->diffuse = materialDifuso;
		this->specular = materialSpecular;
		this->shininess = materialShininess;
        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        this->setupMesh( );
    }

	
    // Render the mesh
    void Draw( Shader shader )
    {
        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
		GLuint normalNr = 1;
		GLuint heightNr = 1;
		
		if (textures.size() > 0) {
			for (GLuint i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
												  // Retrieve texture number (the N in diffuse_textureN)
				stringstream ss;
				string number;
				string name = this->textures[i].type;

				
				if (name == "texture_diffuse")
				{
					ss << diffuseNr++; // Transfer GLuint to stream
				}
				else if (name == "texture_specular")
				{
					ss << specularNr++; // Transfer GLuint to stream
				}
				else if (name == "texture_normal")
				{					
					//number = std::to_string(normalNr++); // transfer unsigned int to stream
					ss << normalNr++;
				}
				else if (name == "texture_height")
				{
					//number = std::to_string(heightNr++); // transfer unsigned int to stream
					ss << heightNr++;
				}
				//std::cout << "Diffuse = " << diffuseNr << ", " << "Normal = " << normalNr << ", " << "Specular = " << specularNr << ". " << std::endl;
				number = ss.str();
				// Now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
				// And finally bind the texture
				glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
			}
		}
		else
		{
			glm::vec3 lightColor;
			lightColor.r = diffuse.r;
			lightColor.g = diffuse.g;
			lightColor.b = diffuse.b;

			glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(1.0f);
			glUniform3f(glGetUniformLocation(shader.ID, "light.ambient"), ambientColor.r, ambientColor.g, ambientColor.b);
			glUniform3f(glGetUniformLocation(shader.ID, "light.diffuse"), diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(glGetUniformLocation(shader.ID, "light.specular"), 1.0f, 1.0f, 1.0f);

			//std::cout << "Llego al sitio " << std::endl;
			//std::cout << "Ambient   = " << ambient.x << ", " << ambient.y << ", " << ambient.z << ", " << std::endl;
			//std::cout << "Diffuse   = " << diffuse.x << ", " << diffuse.y << ", " << diffuse.z << ", " << std::endl;
			//std::cout << "Specular  = " << specular.x << ", " <<specular.y << ", " <<specular.z << ", " << std::endl;
			//std::cout << "shininess = " << shininess << std::endl;

			glUniform3f(glGetUniformLocation(shader.ID, "material.ambient"),  ambient.x,  ambient.y,  ambient.z);
			glUniform3f(glGetUniformLocation(shader.ID, "material.diffuse"),  diffuse.x,  diffuse.y,  diffuse.z);
			glUniform3f(glGetUniformLocation(shader.ID, "material.specular"), specular.x, specular.y, specular.z);
		}
        
        
        // Also set each mesh's shininess property to a default value 
		//(if you want you could extend this to another mesh property and possibly change this value)
        glUniform1f( glGetUniformLocation( shader.ID, "material.shininess" ), 32.0f );
        
        // Draw mesh
        glBindVertexArray( this->VAO );
        glDrawElements( GL_TRIANGLES, this->indices.size( ), GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );
        
        // Always good practice to set everything back to defaults once configured.
        for ( GLuint i = 0; i < this->textures.size( ); i++ )
        {
            glActiveTexture( GL_TEXTURE0 + i );
            glBindTexture( GL_TEXTURE_2D, 0 );
        }
    }
    
private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;
    
    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh( )
    {
        // Create buffers/arrays
        glGenVertexArrays( 1, &this->VAO );
        glGenBuffers( 1, &this->VBO );
        glGenBuffers( 1, &this->EBO );
        
        glBindVertexArray( this->VAO );
        // Load data into vertex buffers
        glBindBuffer( GL_ARRAY_BUFFER, this->VBO );
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData( GL_ARRAY_BUFFER, this->vertices.size( ) * sizeof( Vertex ), &this->vertices[0], GL_STATIC_DRAW );
        
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->EBO );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, this->indices.size( ) * sizeof( GLuint ), &this->indices[0], GL_STATIC_DRAW );
        
        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid * )0 );
        // Vertex Normals
        glEnableVertexAttribArray( 1 );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid * )offsetof( Vertex, Normal ) );
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid * )offsetof( Vertex, TexCoords ) );
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, Bitangent));

        
        glBindVertexArray( 0 );
    }
};


