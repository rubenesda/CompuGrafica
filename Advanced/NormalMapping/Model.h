#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SOIL2/SOIL2.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

using namespace std;

GLint TextureFromFile( const char *path, string directory );

class Model
{
public:

	

    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model( const GLchar *path )
    {
        this->loadModel( path );
    }
    
    // Draws the model, and thus all its meshes
    void Draw( Shader shader )
    {
        for ( GLuint i = 0; i < this->meshes.size( ); i++ )
        {
            this->meshes[i].Draw( shader );
        }
    }
    
private:
    /*  Model Data  */
    vector<Mesh> meshes;

	struct Material {
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient;
		float shininess;
	};

	aiMaterial mat;

	Material materiales; 
    string directory;
    vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    
    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel( string path )
    {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenUVCoords |aiProcess_CalcTangentSpace );
        
        // Check for errors
        if( !scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString( ) << endl;
            return;
        }
        // Retrieve the directory path of the filepath
        this->directory = path.substr( 0, path.find_last_of( '/' ) );
        
        // Process ASSIMP's root node recursively
        this->processNode( scene->mRootNode, scene );
    }
    
    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode( aiNode* node, const aiScene* scene )
    {
        // Process each mesh located at the current node
        for ( GLuint i = 0; i < node->mNumMeshes; i++ )
        {
            // The node object only contains indices to index the actual objects in the scene.
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            
            this->meshes.push_back( this->processMesh( mesh, scene ) );
			//loadMaterial(mat);
        }
        
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for ( GLuint i = 0; i < node->mNumChildren; i++ )
        {
            this->processNode( node->mChildren[i], scene );
        }
    }
    
    Mesh processMesh( aiMesh *mesh, const aiScene *scene )
    {
        // Data to fill
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        
		vector<glm::vec3> vert;
		vector<glm::vec2> uvs;
		vector<glm::vec3> norms;
		//outputs
		vector<glm::vec3> tangents;
		vector<glm::vec3> bitangents;
 
		Material materiales;
		glm::vec3 ambienteV;
		glm::vec3 difusoV;
		glm::vec3 specularV;
		GLfloat shininessV;

        // Walk through each of the mesh's vertices
        for ( GLuint i = 0; i < mesh->mNumVertices; i++ )
        {
            Vertex vertex;
            glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            
            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            
            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

			//std::cout << "Normal = " << mesh->mNormals[i].x << ", " << mesh->mNormals[i].y << ", " << mesh->mNormals[i].z << ", " << std::endl;
            
            // Texture Coordinates
            //if( mesh->mTextureCoords[0] ) 
			if ((((scene)->mMeshes)[0])->mTextureCoords[0]) // Does the mesh contain texture coordinates?
            {
				//std::cout << "Por aqui"  << std::endl;
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;				
                vec.y = mesh->mTextureCoords[0][i].y;				
                vertex.TexCoords = vec;

            }
            else
            {
				//std::cout << "Por aca" << std::endl;
                vertex.TexCoords = glm::vec2( 0.0f, 0.0f );
            }
            /*
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			*/			

            vertices.push_back( vertex );
			vert.push_back(vertex.Position);
			uvs.push_back(vertex.TexCoords);
			norms.push_back(vertex.Normal);
        }

		vertices = computeTangentBasis(vert,uvs, norms,tangents,bitangents,vertices);

		/*
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			std::cout << vertices[i].Position.x << ", " << vertices[i].Position.y << ", " << vertices[i].Position.z << ", " << vertices[i].Normal.x << ", " << vertices[i].Normal.y << ", " << vertices[i].Normal.z << ", " << vertices[i].TexCoords.x << ", " << vertices[i].TexCoords.y << ", " << vertices[i].Tangent.x << ", " << vertices[i].Tangent.y << ", " << vertices[i].Tangent.z << ", " << vertices[i].Bitangent.x << ", " << vertices[i].Bitangent.y << ", " << vertices[i].Bitangent.z << ", " << std::endl;
		};
		*/
				        
        // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for ( GLuint i = 0; i < mesh->mNumFaces; i++ )
        {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for ( GLuint j = 0; j < face.mNumIndices; j++ )
            {
                indices.push_back( face.mIndices[j] );
            }
        }
        
        // Process materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
			// Same applies to other texture as the following list summarizes:
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. Diffuse maps
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// 2. Specular maps
			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			// 3. Normal maps
			vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			
			// 4. height maps
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			// Extra Material DIffuse, Specular y Ambient
			Material materiales = this->loadMaterial(material);
			
			std::cout << "Ambient   = " << materiales.ambient.x << ", " << materiales.ambient.y << ", " << materiales.ambient.z << ", " << std::endl;
			std::cout << "Diffuse   = " << materiales.diffuse.x << ", " << materiales.diffuse.y << ", " << materiales.diffuse.z << ", " << std::endl;
			std::cout << "Specular  = " << materiales.specular.x << ", " << materiales.specular.y << ", " << materiales.specular.z << ", " << std::endl;
			std::cout << "shininess = " << materiales.shininess << std::endl;
			
			ambienteV = materiales.ambient;
			difusoV = materiales.diffuse;
			specularV = materiales.specular;
			shininessV = 32.0f;

        }

		
        // Return a mesh object created from the extracted mesh data
        return Mesh( vertices, indices, textures, ambienteV, difusoV, specularV, shininessV);
    }
    
    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures( aiMaterial *mat, aiTextureType type, string typeName )
    {
        vector<Texture> textures;
        
        for ( GLuint i = 0; i < mat->GetTextureCount( type ); i++ )
        {
            aiString str;
            mat->GetTexture( type, i, &str );
            
			std::cout << "Nombre de textura = " << str.data << std::endl;
			std::cout << "Cantidad de texturas = " << textures_loaded.size() << std::endl;

            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;
            
            for ( GLuint j = 0; j < textures_loaded.size( ); j++ )
            {
                if( textures_loaded[j].path == str )
                {
                    textures.push_back( textures_loaded[j] );
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    
                    break;
                }
            }
            
            if( !skip )
            {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile( str.C_Str( ), this->directory );
                texture.type = typeName;
                texture.path = str;
                textures.push_back( texture );
                
                this->textures_loaded.push_back( texture );  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        
        return textures;
    }

	Material loadMaterial(aiMaterial* mat) {
		Material material;
		aiColor3D color(1.0f, 0.7f, 0.0f);
		float shininess;

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material.diffuse = glm::vec3(color.r, color.g, color.b);

		mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material.ambient = glm::vec3(color.r, color.g, color.b);

		mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material.specular = glm::vec3(color.r, color.g, color.b);

		mat->Get(AI_MATKEY_SHININESS, shininess);
		material.shininess = shininess;

		return material;
	}


	vector<Vertex> computeTangentBasis(
		// inputs
		std::vector<glm::vec3> & vertices,
		std::vector<glm::vec2> & uvs,
		std::vector<glm::vec3> & normals,
		// outputs
		std::vector<glm::vec3> & tangents,
		std::vector<glm::vec3> & bitangents,
		vector<Vertex> vert)
	{


		for (int i = 0; i < vertices.size()-2; i += 3) {

			// Shortcuts for vertices
			glm::vec3 & v0 = vertices[i + 0];
			glm::vec3 & v1 = vertices[i + 1];
			glm::vec3 & v2 = vertices[i + 2];

			// Shortcuts for UVs
			glm::vec2 & uv0 = uvs[i + 0];
			glm::vec2 & uv1 = uvs[i + 1];
			glm::vec2 & uv2 = uvs[i + 2];

			// Edges of the triangle : postion delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

			if ((deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x) == 0)
			{
				r = 0.0f;
			}

			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			//tangent = glm::normalize(tangent);
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;
			//bitangent = glm::normalize(bitangent);
			// Set the same tangent for all three vertices of the triangle.
			// They will be merged later, in vboindexer.cpp
			tangents.push_back(tangent);
			tangents.push_back(tangent);
			tangents.push_back(tangent);

			// Same thing for binormals
			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);

			/*
			std::cout << "Tangents[" << i + 0 << "] =" << tangents[i + 0].x << ", " << tangents[i + 0].y << ", " << tangents[i + 0].z << ", " << std::endl;
			std::cout << "Tangents[" << i + 1 << "] =" << tangents[i + 1].x << ", " << tangents[i + 1].y << ", " << tangents[i + 1].z << ", " << std::endl;
			std::cout << "Tangents[" << i + 2 << "] =" << tangents[i + 2].x << ", " << tangents[i + 2].y << ", " << tangents[i + 2].z << ", " << std::endl;
			
			std::cout << "biTangents[" << i + 0 << "] =" << bitangents[i + 0].x << ", " << bitangents[i + 0].y << ", " << bitangents[i + 0].z << ", " << std::endl;
			std::cout << "biTangents[" << i + 1 << "] =" << bitangents[i + 1].x << ", " << bitangents[i + 1].y << ", " << bitangents[i + 1].z << ", " << std::endl;
			std::cout << "biTangents[" << i + 2 << "] =" << bitangents[i + 2].x << ", " << bitangents[i + 2].y << ", " << bitangents[i + 2].z << ", " << std::endl;
			*/

			vert[i + 0].Tangent = tangents[i + 0];
			vert[i + 1].Tangent = tangents[i + 1];
			vert[i + 2].Tangent = tangents[i + 2];

			vert[i + 0].Bitangent = bitangents[i + 0];
			vert[i + 1].Bitangent = bitangents[i + 1];
			vert[i + 2].Bitangent = bitangents[i + 2];

			/*
			// Estos sirven de ayuda para revisar el estado de los Tangents y Bitangents antes de ser enviador a vert.
			std::cout << "Tangents[" << i + 0 << "] =" << vert[i + 0].Tangent.x << ", " << vert[i + 0].Tangent.y << ", " << vert[i + 0].Tangent.z << ", " << std::endl;
			std::cout << "Tangents[" << i + 1 << "] =" << vert[i + 0].Tangent.x << ", " << vert[i + 0].Tangent.y << ", " << vert[i + 0].Tangent.z << ", " << std::endl;
			std::cout << "Tangents[" << i + 2 << "] =" << vert[i + 0].Tangent.x << ", " << vert[i + 0].Tangent.y << ", " << vert[i + 0].Tangent.z << ", " << std::endl;

			std::cout << "BiTangents[" << i + 0 << "] =" << vert[i + 0].Bitangent.x << ", " << vert[i + 0].Bitangent.y << ", " << vert[i + 0].Bitangent.z << ", " << std::endl;
			std::cout << "BiTangents[" << i + 1 << "] =" << vert[i + 0].Bitangent.x << ", " << vert[i + 0].Bitangent.y << ", " << vert[i + 0].Bitangent.z << ", " << std::endl;
			std::cout << "BiTangents[" << i + 2 << "] =" << vert[i + 0].Bitangent.x << ", " << vert[i + 0].Bitangent.y << ", " << vert[i + 0].Bitangent.z << ", " << std::endl;
			*/			
		}
		return vert;
	}
};

GLint TextureFromFile( const char *path, string directory )
{
    //Generate texture ID and load texture data
    string filename = string( path );
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures( 1, &textureID );
    
    int width, height;
    
    unsigned char *image = SOIL_load_image( filename.c_str( ), &width, &height, 0, SOIL_LOAD_RGB );
    
    // Assign texture to ID
    glBindTexture( GL_TEXTURE_2D, textureID );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture( GL_TEXTURE_2D, 0 );
    SOIL_free_image_data( image );
    
    return textureID;
}

