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
	mymath::vec3	getCenter() const;

	void			setSurfaceState(bool *keys);
	void			correctCenter(GLfloat scalar);



    ~Model();

private:
	bool						_isLoaded;
	SurfaceState				_current_surface;
	bool						_space_latch;
	bool						_bound_set;
	std::vector<bool>			_tex_is_default;
	std::vector<Mesh>			_meshList;
	std::vector<Texture>		_textureList;
	std::vector<mymath::vec3>	_colorList;
	std::vector<unsigned int>	_mesh_to_tex;
	mymath::vec3				_center;
	mymath::vec3				_min;
	mymath::vec3				_max;

	void	_loadModel(const char *filePath);
	void	_loadNode(mymodel::Node *node, const mymodel::Scene *scene);
	void	_loadMesh(mymodel::Mesh *mesh, const mymodel::Scene *scene);
	void	_loadMaterials(const mymodel::Scene *scene);
};

#endif
