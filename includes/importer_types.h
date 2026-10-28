#ifndef IMPORTER_TYPES_H
#define IMPORTER_TYPES_H

#include <string>
#include "mymath/vec3.hpp"
#include "mymath/mat4.hpp"

/*
 * Post-processing flags — same names/values Assimp uses, so Model.cpp's
 * ReadFile(...) call compiles unchanged. Do NOT include assimp/postprocess.h
 * in the same translation unit as this file once the swap is made.
 */
enum {
    aiProcess_JoinIdenticalVertices = 0x2,
    aiProcess_Triangulate           = 0x8,
    aiProcess_GenSmoothNormals      = 0x40,
    aiProcess_FlipUVs               = 0x800000
};

/* Texture types — Model.cpp currently only consumes DIFFUSE (map_Kd). */
enum aiTextureType {
    aiTextureType_NONE       = 0,
    aiTextureType_DIFFUSE    = 1,   // map_Kd
    aiTextureType_SPECULAR   = 2,   // map_Ks
    aiTextureType_AMBIENT    = 3,   // map_Ka
    aiTextureType_REFLECTION = 4,   // map_refl
    aiTextureType_HEIGHT     = 5,   // map_bump
    aiTextureType_NORMALS    = 6
};

enum aiReturn {
    AI_SUCCESS     = 0,
    AI_FAILURE     = -1,
    AI_OUTOFMEMORY = -3
};

namespace mymodel {

/* Assimp's aiString equivalent; Model.cpp only reads path.data. */
struct String {
    std::string data;

    String() = default;
    explicit String(const std::string& s) : data(s) {}
    const char* C_Str() const { return data.c_str(); }
};

/*
 * MTL parsing comes later. These stubs keep Model.cpp compiling:
 * GetTextureCount() reports nothing, so _loadMaterials() falls back to
 * the default texture until the MTL pass is implemented.
 */
struct Material {
    std::string 	mName;
	std::string		mDiffuseTex; //path to tex
	mymath::vec3	mDiffuseColor;

    unsigned int GetTextureCount(aiTextureType type) const 
	{
		return (type == aiTextureType_DIFFUSE && !mDiffuseTex.empty()) ? 1 : 0;
	}
    aiReturn     GetTexture(aiTextureType type, unsigned int index, String* path) const
    {
		if (type != aiTextureType_DIFFUSE || index > 0 || mDiffuseTex.empty() || !path)
			return AI_FAILURE;
		path->data = mDiffuseTex;
		return AI_SUCCESS;
    }

	aiReturn	GetColor(mymath::vec3 &diffuseColor)
	{
		if (mDiffuseColor.x || mDiffuseColor.y || mDiffuseColor.z)
		{
			diffuseColor = mDiffuseColor;
			return AI_SUCCESS;
		}
		return AI_FAILURE;
	}
};

/*
 * Memory ownership: the Importer allocates everything below in _bake()
 * and frees it in _clearScene().
 *
 * Raw pointers + counts are kept (instead of std::vector) because
 * Model.cpp null-checks these members: !scene->mMeshes, !node->mChildren,
 * !mesh->mVertices, if (mesh->mTextureCoords[0]).
 */

struct Face {
    unsigned int  mNumIndices = 0;
    unsigned int* mIndices    = nullptr;   // into the mesh's unified vertex arrays
};

struct Mesh {
    mymath::vec3*  mVertices          = nullptr;              // mNumVertices entries
    unsigned int   mNumVertices       = 0;
    mymath::vec3*  mNormals           = nullptr;              // may be null
    mymath::vec3*  mTextureCoords[1]  = { nullptr };          // [0] = UVs, z unused
    Face*          mFaces             = nullptr;
    unsigned int   mNumFaces          = 0;
    unsigned int   mMaterialIndex     = 0;                    // into Scene::mMaterials
	bool		   mTexValid		  = false;
	bool		   mNormValid		  = false;
};

struct Node {
    std::string    mName;
    mymath::mat4   mTransformation;          // identity for OBJ files
    Node**         mChildren          = nullptr;
    unsigned int   mNumChildren       = 0;
    unsigned int*  mMeshes            = nullptr;   // indices into Scene::mMeshes
    unsigned int   mNumMeshes         = 0;
};

struct Scene {
    Node*          mRootNode        = nullptr;
    Mesh**         mMeshes          = nullptr;
    unsigned int   mNumMeshes       = 0;
    Material**     mMaterials       = nullptr;
    unsigned int   mNumMaterials    = 0;
};

} // namespace mymodel

#endif
