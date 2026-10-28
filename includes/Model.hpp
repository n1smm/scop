#ifndef MODEL_HPP
#define MODEL_HPP


#include "std_libs.h"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Importer.hpp"
#include "common_values.h"

#include <GLFW/glfw3.h>

class Model {
public:
    Model(const char *filePath);
    Model(const Model& other)				= delete;
    Model& operator=(const Model& other)	= delete;
    Model(Model&& other)					noexcept = default;
    Model& operator=(Model&& other)			noexcept = default;

	void	renderModel(GLint inColorLocation);
	void	clearModel();

	bool			isValid() const;
	SurfaceState	getSurfaceState() const;
	void			setSurfaceState(bool *keys);



    ~Model();

private:
	bool						_isLoaded;
	SurfaceState				_current_surface;
	bool						_space_latch;
	std::vector<Mesh>			_meshList;
	std::vector<Texture>		_textureList;
	std::vector<mymath::vec3>	_colorList;
	std::vector<unsigned int>	_mesh_to_tex;

	void	_loadModel(const char *filePath);
	void	_loadNode(mymodel::Node *node, const mymodel::Scene *scene);
	void	_loadMesh(mymodel::Mesh *mesh, const mymodel::Scene *scene);
	void	_loadMaterials(const mymodel::Scene *scene);
};

#endif
