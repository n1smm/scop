#ifndef IMPORTER_HPP
#define IMPORTER_HPP

#include <string>
#include <vector>
#include "std_libs.h"

#include "importer_types.h"
#include "utils.h"

namespace mymodel {

class Importer {
public:
    Importer();
    Importer(const Importer& other) = delete;
    Importer& operator=(const Importer& other) = delete;

    const Scene*        ReadFile(const std::string& filePath, unsigned int flags);
    std::string	GetErrorString() const;

    ~Importer();

private:
    /*
     * Line classification only — OBJ lines are independent of each other,
     * so there is no state machine and no valid/invalid ordering to check.
     * This just answers "what kind of line am I looking at".
     */
    enum LineContext {
        NAME,          // o / g        → new node + new mesh
        USE_MATERIAL,  // usemtl name  → new mesh, same node
        VERTEX,        // v x y z
        NORMAL,        // vn x y z
        TEXCOORD,      // vt u v
        FACE,          // f v[/vt][/vn] ...
        MTLLIB,        // mtllib file  → stored for the later MTL pass
        IGNORE,        // s, vp, l, p, #, unknown → skip
        INVALID        // recognized keyword but malformed content → warn + skip
    };

    struct RawCorner { int v, vt, vn;
                       RawCorner() : v(-1), vt(-1), vn(-1) {} };

    struct RawFace   { std::vector<RawCorner> corners; };
	
    struct RawMesh   { std::string name;       // from o/g
                       std::string material;   // from usemtl
                       std::vector<RawFace> faces; };

	struct RawUv	 { float	x;
					   float	y;
						RawUv() : x(-1.0f), y(-1.0f) {} };

    /* parser state */
	LineContext					_curr_context;
    std::string                 _error;
    unsigned int                _flags;
    std::vector<mymath::vec3>   _vertices;
    std::vector<mymath::vec3>   _normals;
    std::vector<RawUv> 			_uvs;
    std::vector<RawMesh>        _raw_meshes;
	std::vector<Material>		_materials;
    std::string                 _mtllib;
    Scene                       _scene;

    /* pipeline */
    void        _parseContent(const std::string& fileContent);
	void		_parseMtl(const	std::string& filePath);
    LineContext _checkLine(std::vector<std::string_view> &tokens) const;
    void        _parseLine(std::vector<std::string_view> &tokens, RawMesh &mesh);
	void		_parseFace(std::vector<std::string_view> &tokens, RawFace &face);
    void        _finishMesh(RawMesh &mesh);
    void        _bake();
	void		_bakeMesh(RawMesh &rawMesh, Mesh &newMesh);
    void        _clearScene();
};

}

#endif
