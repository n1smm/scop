#include "Model.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>

static std::string findTexturePath(const std::string& dir, const std::string& fileName)
{
    std::filesystem::path base(dir);
    std::filesystem::path exact = base / fileName;
    if (std::filesystem::exists(exact))
        return exact.string();

    std::filesystem::path current = base;
    for (const auto& part : std::filesystem::path(fileName))
    {
        if (!std::filesystem::is_directory(current))
            return exact.string();

        std::string wanted = part.string();
        std::transform(wanted.begin(), wanted.end(), wanted.begin(),
                       [](unsigned char c) { return (char)std::tolower(c); });

        bool found = false;
        for (const auto& entry : std::filesystem::directory_iterator(current))
        {
            std::string candidate = entry.path().filename().string();
            std::transform(candidate.begin(), candidate.end(), candidate.begin(),
                           [](unsigned char c) { return (char)std::tolower(c); });
            if (candidate == wanted)
            {
                current = entry.path();
                found = true;
                break;
            }
        }
        if (!found)
            return exact.string(); // let loadTexture() report the failure
    }
    return current.string();
}


Model::Model(const char *filePath) : _isLoaded(false), _current_surface(SurfaceState::TEXTURE), _space_latch(false)
{
	_loadModel(filePath);
}

/* public methods */
void	Model::renderModel(GLint inColorLocation)
{
	if (!_isLoaded)
		return ;

	for (size_t i = 0; i < _meshList.size(); i++)
	{
		if (i >= _mesh_to_tex.size())
			continue ;

		unsigned int material_idx = _mesh_to_tex[i];
		if (material_idx < _textureList.size() && _textureList[material_idx].isValid())
		{
			_textureList[material_idx].useTexture(0);
			if (inColorLocation != -1)
			{
				mymath::vec3 &tmp = _colorList[material_idx];
				glUniform3f(inColorLocation, tmp.x, tmp.y, tmp.z);
			}
		}
		if (_current_surface == SurfaceState::TEXTURE || _current_surface == SurfaceState::COLOR)
			_meshList[i].renderMesh(Mesh::RenderMode::FILLED);
		if (_current_surface == SurfaceState::WIREFRAME)
			_meshList[i].renderMesh(Mesh::RenderMode::WIREFRAME);
		if (_current_surface == SurfaceState::POINT)
			_meshList[i].renderMesh(Mesh::RenderMode::POINTS);
	}

}

void	Model::clearModel()
{
	_isLoaded = false;
	_meshList.clear();
	_textureList.clear();
	_mesh_to_tex.clear();
}

bool			Model::isValid() const { return _isLoaded; }

SurfaceState	Model::getSurfaceState() const { return _current_surface; }

void			Model::setSurfaceState(bool *keys)
{ 
	bool space_key = keys[GLFW_KEY_SPACE];
	if (keys[GLFW_KEY_SPACE] && !_space_latch)
	{
		GLint size = static_cast<GLint>(SurfaceState::COUNT);
		GLint next_step = (static_cast<GLint>(_current_surface) + 1) % size;
		_current_surface = static_cast<SurfaceState>(next_step);
	}
	_space_latch = space_key;

}


/* private methods */

void	Model::_loadModel(const char *filePath)
{
	std::string 			file_path = filePath;
	mymodel::Importer		importer;
	const mymodel::Scene	*scene = importer.ReadFile(file_path
												 , aiProcess_Triangulate
												 | aiProcess_FlipUVs
												 | aiProcess_GenSmoothNormals
												 | aiProcess_JoinIdenticalVertices);
	if (!scene)
	{
		std::cerr << "model failed to load: "  << importer.GetErrorString() << ", filepath: "
			<< file_path << std::endl;
		_isLoaded = false;
		return ;
	}

	_loadNode(scene->mRootNode, scene);
	_loadMaterials(scene);
	_isLoaded = true;
}

void	Model::_loadNode(mymodel::Node *node, const mymodel::Scene *scene)
{
	if (!node || !scene || !scene->mMeshes)
		return ;
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		unsigned int mesh_idx = node->mMeshes[i];
		if (mesh_idx >= scene->mNumMeshes)
		{
			std::cerr << "node references out-of-range mesh index: "
				<< mesh_idx << std::endl;
			continue ;
		}
		_loadMesh(scene->mMeshes[mesh_idx], scene);
	}

	if (node->mNumChildren > 0 && !node->mChildren)
		return ;
	for (size_t i = 0; i < node->mNumChildren; i++)
		_loadNode(node->mChildren[i], scene);
}

void	Model::_loadMesh(mymodel::Mesh *mesh, const mymodel::Scene *scene)
{
	if (!mesh || !scene)
		return ;

	if (!mesh->mVertices || !mesh->mFaces
			|| mesh->mNumVertices == 0 || mesh->mNumFaces == 0)
	{
		std::cerr << "skipping mesh: missing vertex or face data" << std::endl;
		return ;
	}

	const bool has_normals = (mesh->mNormals != nullptr);

	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;


	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x,
	  									  mesh->mVertices[i].y,
	  									  mesh->mVertices[i].z });

		if (mesh->mTextureCoords[0] && mesh->mTexValid)
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y });
		else	
		{
			size_t corner = i % 3;
			if (corner == 0)
				vertices.insert(vertices.end(), { 0.0f, 0.0f});
			else if (corner == 1)
				vertices.insert(vertices.end(), { 1.0f, 0.0f});
			else
				vertices.insert(vertices.end(), { 0.0f, 1.0f});

		}

		if (has_normals)
			vertices.insert(vertices.end(), { mesh->mNormals[i].x,
	  									  mesh->mNormals[i].y,
	  									  mesh->mNormals[i].z });
		else
			vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		mymodel::Face face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (indices.empty())
	{
		std::cerr << "skipping mesh with empty index buffer" << std::endl;
		return ;
	}

	if (scene->mNumMaterials == 0 || mesh->mMaterialIndex >= scene->mNumMaterials)
		std::cerr << "mesh material index " << mesh->mMaterialIndex
			<< " is out of range (material count: " << scene->mNumMaterials
			<< "), rendering without texture" << std::endl;

	Mesh new_mesh;
	new_mesh.createMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	_meshList.push_back(std::move(new_mesh));
	_mesh_to_tex.push_back(mesh->mMaterialIndex);
}

void	Model::_loadMaterials(const mymodel::Scene *scene)
{
	if (!scene || !scene->mMaterials)
		return ;

	_textureList.resize(scene->mNumMaterials);
	_colorList.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		mymodel::Material *material = scene->mMaterials[i];
		if (!material)
			continue ;

		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			mymodel::String path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				std::string full_path = path.data;
				size_t pos = full_path.find_last_of("\\/");
				std::string filename = (pos == std::string::npos) ? full_path : full_path.substr(pos + 1);

				std::string texPath = findTexturePath("textures", filename);

				_textureList[i] = Texture(texPath.c_str());

				if(!_textureList[i].isValid())
				{
					std::cout << "failed to load texture at: " << texPath << std::endl;
					_textureList[i].clearTexture();
				}
			}
		}
		mymath::vec3 color;
		if (material->GetColor(color) == AI_SUCCESS)
			_colorList[i] = color;
	}
}



Model::~Model()
{
}
