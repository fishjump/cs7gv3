#ifndef GL_MODEL_HPP
#define GL_MODEL_HPP

#include <unordered_map>
#include <vector>

#include <assimp/scene.h>
#include <gl/mesh.hpp>

namespace gl {

class model_t final {
public:
  model_t(const std::string &path, bool gamma_correction = false);
  void draw(const shader_t &shader);

private:
  bool _gamma_correction;
  std::string _dir;
  std::vector<mesh_t> _meshes;
  std::unordered_map<std::string, texture_t> _textures_cache;

  common::result_t<> load(const std::string &path);
  void process_node(aiNode *node, const aiScene *scene);
  mesh_t process_mesh(aiMesh *mesh, const aiScene *scene);
  std::vector<gl::texture_t>
  load_material_textures(aiMaterial *mat, aiTextureType type,
                         const std::string &typeName);
};

} // namespace gl

#endif // GL_MODEL_HPP