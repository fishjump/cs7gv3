#ifndef GL_MODEL_HPP
#define GL_MODEL_HPP

#include <unordered_map>
#include <vector>

#include <assimp/scene.h>
#include <gl/mesh.hpp>

namespace gl {

class model_t final {
public:
  using init_func_t = std::function<void(model_t &self)>;
  using loop_func_t = std::function<void(model_t &self)>;

  model_t(const std::string &path, const init_func_t init = nullptr,
          const loop_func_t loop = nullptr, bool gamma_correction = false);
  void draw(const shader_t &shader);

  const glm::mat4 &position() const;
  const void init();
  const void loop();

  model_t &translate(const glm::vec3 &v);
  model_t &scale(const glm::vec3 &v);
  model_t &rotate(float degree, const glm::vec3 &axis);

private:
  bool _gamma_correction;
  glm::mat4 _position;
  init_func_t _init;
  loop_func_t _loop;

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