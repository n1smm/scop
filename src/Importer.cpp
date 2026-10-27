#include "Importer.hpp"

namespace mymodel {

Importer::Importer() : _curr_context(LineContext::IGNORE)
					, _flags(0)
{

}

/* public methods */
const	Scene		*Importer::ReadFile(const std::string &filePath, unsigned int flags)
{
	// reset parser state
	_clearScene();
	_error.clear();
	_curr_context = LineContext::IGNORE;
	_vertices.clear();
	_normals.clear();
	_uvs.clear();
	_raw_meshes.clear();
	_materials.clear();
	_mtllib.clear();

	const std::string file_content = readFromfile(filePath.c_str());	
	if (file_content == "FAIL")
	{
		_error = "failed to read file: " + filePath;
		return &_scene;
	}

	_flags = flags;
	_parseContent(file_content);
	if (!_mtllib.empty())
	{
		std::string mtl_path = filePath.substr(0, filePath.find_last_of('/') + 1) + _mtllib;
		_parseMtl(mtl_path);
	}
	_bake();

	return &_scene;

}
std::string	Importer::GetErrorString() const
{
	return _error;
}


/* private methods - parsing pipeline */
void        Importer::_parseContent(const std::string& fileContent)
{
	std::istringstream file_stream(fileContent);

	std::string line;
	RawMesh	mesh;
	while (std::getline(file_stream, line))
	{
		std::vector<std::string_view> tokens = split_line(line);

		_curr_context = _checkLine(tokens);
		
		if (_curr_context == LineContext::INVALID || _curr_context == LineContext::IGNORE)
			continue;
		else if (_curr_context == LineContext::NAME)
		{
			_finishMesh(mesh);
		}
		else if (_curr_context == LineContext::USE_MATERIAL)
			_finishMesh(mesh);

		_parseLine(tokens, mesh);
	}
	_finishMesh(mesh);
	

}

void	Importer::_parseMtl(const std::string &filePath)
{
	const std::string file_content = readFromfile(filePath.c_str());	
	if (file_content == "FAIL")
	{
		_error = "failed to read material file: " + filePath;
		return;
	}

	std::istringstream file_stream(file_content);

	std::string line;
	Material curr_material = Material();
	while (std::getline(file_stream, line))
	{
		std::vector<std::string_view> tokens = split_line(line);
		if (tokens.empty())
			continue;

		if (tokens[0] == "newmtl" && tokens.size() > 1)
		{
			if (!curr_material.mName.empty())
			{
				_materials.push_back(std::move(curr_material));
				curr_material = Material();
			}
			curr_material.mName = std::string(tokens[1]);
		}
		else if (tokens[0] == "Kd" && tokens.size() > 3)
		{
			float x = 0.0f, y = 0.0f, z = 0.0f;
			bool xx = toNumber(tokens[1], x);
			bool yy = toNumber(tokens[2], y);
			bool zz = toNumber(tokens[3], z);
			if (xx && yy && zz)
				curr_material.mDiffuseColor = mymath::vec3(x, y, z);
		}
		else if (tokens[0] == "map_Kd" && tokens.size() > 1)
		{
			size_t pos = static_cast<size_t>(tokens[1].data() - line.data());
			size_t hash = line.find('#', pos);

			size_t len = (hash == std::string::npos ? line.size() : hash) - pos;
			std::string_view rest(line.data() + pos, len);

			while (!rest.empty() && (rest.back() == ' ' || rest.back() == '\t' || rest.back() == '\r'))
				rest.remove_suffix(1);

			if (!rest.empty())
				curr_material.mDiffuseTex = std::string(rest);
		}
	}

	if (!curr_material.mName.empty())
		_materials.push_back(std::move(curr_material));
}

Importer::LineContext Importer::_checkLine(std::vector<std::string_view> &tokens) const
{
	if (tokens.empty())
		return LineContext::IGNORE;

	const std::string_view identity = tokens[0];

	if (identity == "o" || identity == "g")
		return tokens.size() > 1 ? LineContext::NAME : LineContext::INVALID;

	if (identity == "usemtl")
		return tokens.size() > 1 ? LineContext::USE_MATERIAL : LineContext::INVALID;

	if (identity == "v")
		return tokens.size() > 3 ? LineContext::VERTEX : LineContext::INVALID;

	if (identity == "vn")
		return tokens.size() > 3 ? LineContext::NORMAL : LineContext::INVALID;

	if (identity == "vt")
		return tokens.size() > 2 ? LineContext::TEXCOORD : LineContext::INVALID;

	if (identity == "f")
		return tokens.size() > 3 ? LineContext::FACE : LineContext::INVALID;

	if (identity == "mtllib")
		return tokens.size() > 1 ? LineContext::MTLLIB : LineContext::INVALID;

	if (identity == "s" || identity == "vp" || identity == "l" || identity == "p"
		|| identity[0] == '#')
		return LineContext::IGNORE;

	return LineContext::INVALID;
}

void        Importer::_parseLine(std::vector<std::string_view> &tokens, RawMesh &mesh)
{
	if (_curr_context == LineContext::NAME && tokens.size() >= 2 && !tokens[1].empty())
	{
		mesh.name = tokens[1];	
	}
	else if (_curr_context == LineContext::USE_MATERIAL && tokens.size() >= 2 && !tokens[1].empty())
	{
		mesh.material = tokens[1];
	}
	else if (_curr_context == LineContext::VERTEX)
	{
		float x, y, z;
		bool xx = toNumber(tokens[1], x);
		bool yy = toNumber(tokens[2], y);
		bool zz = toNumber(tokens[3], z);
		if (!xx || !yy || !zz)
			return;
		_vertices.push_back(mymath::vec3(x, y, z));
	}
	else if (_curr_context == LineContext::NORMAL)
	{
		float x, y, z;
		bool xx = toNumber(tokens[1], x);
		bool yy = toNumber(tokens[2], y);
		bool zz = toNumber(tokens[3], z);
		if (!xx || !yy || !zz)
			return;
		_normals.push_back(mymath::vec3(x, y, z));
	}
	else if (_curr_context == LineContext::TEXCOORD)
	{
		RawUv	uv;
		bool xx = toNumber(tokens[1], uv.x);
		bool yy = toNumber(tokens[2], uv.y);
		if (!xx || !yy)
			return;
		_uvs.push_back(uv);
	}
	else if (_curr_context == LineContext::FACE)
	{
		if (tokens.size() >= 4)
			_parseFace(tokens, mesh.faces.emplace_back());
	}
	else if (_curr_context == LineContext::MTLLIB && tokens.size() >= 2 && !tokens[1].empty())
	{
		_mtllib = tokens[1];
	}
}

void		Importer::_parseFace(std::vector<std::string_view> &tokens, RawFace &face)
{
	for (size_t i = 1; i < tokens.size(); i++)
	{
		size_t 				start = 0;
		size_t 				which_value = 0;
		std::string_view 	token = tokens[i];
		RawCorner			corner;
		for(size_t j = 0; j < token.size(); j++)
		{
			if (token[j] == '/' || j == token.size() - 1)	
			{
				size_t				end = (token[j] == '/') ? j : j +1;	
				std::string_view	value = token.substr(start, end - start);
	  			bool success = false;
				switch (which_value) {
					case 0:
						success = toNumber(value, corner.v);
						if (!success || corner.v == 0)
							corner.v = -1;
						break;
					case 1:
						success = toNumber(value, corner.vt);
						if (!success)
							corner.vt = -1;
						break;
					case 2:
						success = toNumber(value, corner.vn);
						if (!success)
							corner.vn = -1;
						break;
				}
				which_value++;
				if (which_value >= 3)
					break;
				start = j + 1;
			}
		}
		face.corners.push_back(corner);
	}
}

void        Importer::_finishMesh(RawMesh &mesh)
{
	RawMesh next;

	/* name/material apply to the FOLLOWING faces, so carry them over
	 * even when the current mesh has no geometry yet (g Joint -> usemtl
	 * arrives before the first f line in x-wing.obj) */
	next.name = mesh.name;
	next.material = mesh.material;
	if (!mesh.faces.empty())
		_raw_meshes.push_back(std::move(mesh));
	mesh = std::move(next);
}

void        Importer::_bake()
{
	_clearScene();
	//check that meshes exist
	_raw_meshes.erase(std::remove_if(_raw_meshes.begin()
				   					, _raw_meshes.end()
								  	, [](const RawMesh &m) { return m.faces.empty(); })
								  	, _raw_meshes.end());

	// set materials from raw mesh to mMaterials array
	std::set<std::string>	mat_name;
	for (size_t i = 0; i < _raw_meshes.size(); i++)
	{
		mat_name.insert(_raw_meshes[i].material.empty() ? "default" :
				  _raw_meshes[i].material);
	}
	_scene.mNumMaterials = static_cast<unsigned int>(mat_name.size());

	_scene.mMaterials = new Material*[mat_name.size()];
	std::map<std::string, unsigned int> mat_lookup;
	size_t i = 0;
	for (auto it = mat_name.begin(); it != mat_name.end(); ++it, i++)
	{
		_scene.mMaterials[i] = new Material();
		_scene.mMaterials[i]->mName = *it;	
		for (size_t m = 0; m < _materials.size(); m++)
		{
			if (_materials[m].mName == *it)
			{
				_scene.mMaterials[i]->mDiffuseTex = _materials[m].mDiffuseTex;
				_scene.mMaterials[i]->mDiffuseColor = _materials[m].mDiffuseColor;
				break;
			}
		}
		mat_lookup[*it] = static_cast<unsigned int>(i);
	}
	

	// allocate and populate mMeshes from _raw_meshes
	_scene.mNumMeshes = static_cast<unsigned int>(_raw_meshes.size());
	_scene.mMeshes = new Mesh*[_raw_meshes.size()];
	for (size_t i = 0; i < _raw_meshes.size(); i++)
	{
		_scene.mMeshes[i] = new Mesh();
		std::string mat = _raw_meshes[i].material;
		if (mat.empty())
			mat = "default";
		_scene.mMeshes[i]->mMaterialIndex = mat_lookup[mat];
		_bakeMesh(_raw_meshes[i], *_scene.mMeshes[i]);
	}

	// build node tree
	if (_raw_meshes.empty())
	{
		_scene.mRootNode =nullptr;
		return;
	}

	_scene.mRootNode = new Node();
	_scene.mRootNode->mName = "root";
	size_t nodeCount = 0;
	//count nodes
	for (size_t i = 0; i < _raw_meshes.size();)
	{
		const std::string &name = _raw_meshes[i].name;
		size_t j = i + 1;
		while (j < _raw_meshes.size() && name == _raw_meshes[j].name)
			j++;
		++nodeCount;
		i = j;
	}

	_scene.mRootNode->mNumChildren = nodeCount;
	_scene.mRootNode->mChildren = new Node*[nodeCount];
	
	nodeCount = 0;
	// assign meshes to nodes and alloc memory
	for (size_t i = 0; i < _raw_meshes.size();)
	{
		const std::string &name = _raw_meshes[i].name;
		size_t j = i + 1;
		while (j < _raw_meshes.size() && name == _raw_meshes[j].name)
			j++;

		Node *node = new Node();
		node->mName = name;
		node->mTransformation = mymath::mat4();
		node->mNumMeshes = static_cast<unsigned int>(j - i);
		node->mMeshes = new unsigned int[j - i];
		node->mNumChildren = 0;
		node->mChildren = nullptr;

		for (size_t k = i; k < j; k++)
			node->mMeshes[k - i] = static_cast<unsigned int>(k);

		_scene.mRootNode->mChildren[nodeCount++] = node;
		i = j;
	}
}

void	Importer::_bakeMesh(RawMesh &rawMesh, Mesh &newMesh)
{
	size_t vCount = 0;
	size_t triCount = 0;
	//check vertex and index count of rawMesh
	for (size_t i = 0; i < rawMesh.faces.size(); i++)
	{
		const RawFace &face = rawMesh.faces[i];
		if (face.corners.size() < 3) continue;

		bool	valid = true;
		for (size_t j = 0; j < face.corners.size(); j++)
		{
	   		const RawCorner &corner = face.corners[j];
			if ( corner.v < 1 || static_cast<size_t>(corner.v) > _vertices.size())
			{
				valid = false;
				break;
			}
		}
		if (!valid) continue;
		vCount += face.corners.size();
		triCount += face.corners.size() - 2;
	}

	newMesh.mNumVertices 		= static_cast<unsigned int>(vCount);
	newMesh.mNumFaces 			= static_cast<unsigned int>(triCount);
	newMesh.mVertices 			= new mymath::vec3[vCount];
	newMesh.mNormals 			= new mymath::vec3[vCount];
	newMesh.mTextureCoords[0] 	= new mymath::vec3[vCount];
	newMesh.mFaces 				= new Face[triCount];

	vCount = 0;
	triCount = 0;
	// bake the vertices, uvs and normals into the new mesh
	for (size_t i = 0; i < rawMesh.faces.size(); i++)
	{
		const RawFace 	&face = rawMesh.faces[i];
		bool			valid = true;
		if (face.corners.size() < 3) continue;

		for (size_t j = 0; j < face.corners.size(); j++)
		{
	   		const RawCorner &corner = face.corners[j];
			if ( corner.v < 1 || static_cast<size_t>(corner.v) > _vertices.size())
			{
				valid = false;
				break;
			}
		}
		if (!valid) continue;
		size_t base = vCount;

		for (size_t j = 0; j < face.corners.size(); j++)
		{
			const RawCorner &corner = face.corners[j];

			newMesh.mVertices[vCount] = _vertices[corner.v -1];
			
			if (corner.vn >= 1 && static_cast<size_t>(corner.vn) <= _normals.size())
				newMesh.mNormals[vCount] = _normals[corner.vn -1];

			if (corner.vt >= 1 && static_cast<size_t>(corner.vt) <= _uvs.size())
			{
				newMesh.mTextureCoords[0][vCount] = mymath::vec3(_uvs[corner.vt -1].x
											 			, _uvs[corner.vt -1].y
											 			, 0.0f);
			}
			vCount++;
		}

		// add face indices that reference vertex data
		for (size_t k = 1; k +1 < face.corners.size(); k++)
		{
			Face &new_face = newMesh.mFaces[triCount++];
			new_face.mNumIndices = 3;
			new_face.mIndices = new unsigned int[3];
			new_face.mIndices[0] = static_cast<unsigned int>(base);
			new_face.mIndices[1] = static_cast<unsigned int>(base + k);
			new_face.mIndices[2] = static_cast<unsigned int>(base + k + 1);
		}
	}
}

void        Importer::_clearScene()
{
	if (_scene.mMeshes)
	{
		for (size_t i = 0; i < _scene.mNumMeshes; i++)
		{
			Mesh *curr_mesh =_scene.mMeshes[i];
			if (!curr_mesh) continue;
			for (size_t j = 0; j < curr_mesh->mNumFaces; j++)
				delete[] curr_mesh->mFaces[j].mIndices;
			delete[]	curr_mesh->mVertices;
			delete[]	curr_mesh->mNormals;
			delete[]	curr_mesh->mTextureCoords[0];
			delete[]	curr_mesh->mFaces;
			delete		curr_mesh;	
		}
	}
	delete[] _scene.mMeshes;

	for (size_t i = 0; i < _scene.mNumMaterials; i++)
		delete _scene.mMaterials[i];
	delete[] _scene.mMaterials;

	if (_scene.mRootNode)
	{
		for (size_t i = 0; i < _scene.mRootNode->mNumChildren; i++)
		{
			delete[]	_scene.mRootNode->mChildren[i]->mMeshes;
			delete		_scene.mRootNode->mChildren[i];
		}
		delete[] _scene.mRootNode->mChildren;
		delete	 _scene.mRootNode;
	}
	
	_scene = Scene();
}

Importer::~Importer()
{
	_clearScene();
}

}
