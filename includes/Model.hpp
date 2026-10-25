#ifndef MODEL_HPP
#define MODEL_HPP


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "std_libs.h"
#include "Mesh.hpp"
#include "Texture.hpp"

namespace mymodel {
	using Importer	= Assimp::Importer;
	using Node		= aiNode;
	using Scene		= aiScene;
	using Mesh		= aiMesh;
	using Face		= aiFace;
	using Material	= aiMaterial;
	using String	= aiString;

    inline unsigned            childCount(const Node& n)              { return n.mNumChildren; }
    inline const Node*         child(const Node& n, unsigned i)       { return n.mChildren[i]; }
    inline unsigned            meshCount(const Node& n)               { return n.mNumMeshes; }
    inline unsigned            meshIndex(const Node& n, unsigned i)   { return n.mMeshes[i]; }
    inline const Node&         root(const Scene& s)                   { return *s.mRootNode; }
};

class Model {
public:
    Model(const char *filePath);
    Model(const Model& other)				= delete;
    Model& operator=(const Model& other)	= delete;
    Model(Model&& other)					noexcept = default;
    Model& operator=(Model&& other)			noexcept = default;

	void	renderModel();
	void	clearModel();
	bool	isValid() const;


    ~Model();

private:
	bool						_isLoaded;
	std::vector<Mesh>			_meshList;
	std::vector<Texture>		_textureList;
	std::vector<unsigned int>	_mesh_to_tex;

	void	_loadModel(const char *filePath);
	void	_loadNode(mymodel::Node *node, const mymodel::Scene *scene);
	void	_loadMesh(mymodel::Mesh *mesh, const mymodel::Scene *scene);
	void	_loadMaterials(const mymodel::Scene *scene);
};

#endif
