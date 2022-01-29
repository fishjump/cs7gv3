#define STB_IMAGE_IMPLEMENTATION

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <boost/filesystem.hpp>
#include <stb_image.h>

#include <common.hpp>
#include <gl/model.hpp>

namespace {

common::result_t<GLuint> texture_from_file(const std::string &file,
                                           const std::string &dir, bool gamma) {
  const std::string filename = common::make_str(dir, '/', file);

  GLuint texture_id;
  glGenTextures(1, &texture_id);

  int width = 0, height = 0, nr_components = 0;
  uint8_t *data =
      stbi_load(filename.c_str(), &width, &height, &nr_components, 0);
  defer(stbi_image_free(data));
  if (!data) {
    std::string err =
        common::make_str("Texture failed to load at path: ", filename);
    LOG_ERR(err);
    return {0, err};
  }

  GLenum format;
  switch (nr_components) {
  case 1:
    format = GL_RED;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  }

  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return {texture_id, std::nullopt};
}

} // namespace

gl::model_t::model_t(const std::string &path, bool gamma_correction)
    : _gamma_correction(gamma_correction) {
  auto res = load(path);
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return;
  }
}

void gl::model_t::draw(const shader_t &shader) {
  for (const auto &mesh : _meshes) {
    mesh.draw(shader);
  }
}

common::result_t<> gl::model_t::load(const std::string &path) {
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) ||
      !scene->mRootNode) {
    std::string err = importer.GetErrorString();
    LOG_ERR(err);
    return {common::none_v, err};
  }

  _dir = path.substr(0, path.find_last_of('/'));

  process_node(scene->mRootNode, scene);

  return {common::none_v, std::nullopt};
}

void gl::model_t::process_node(aiNode *node, const aiScene *scene) {
  for (size_t i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    _meshes.push_back(process_mesh(mesh, scene));
  }

  for (size_t i = 0; i < node->mNumChildren; i++) {
    process_node(node->mChildren[i], scene);
  }
}

gl::mesh_t gl::model_t::process_mesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<gl::vertex_t> vertices;
  std::vector<uint32_t> indices;
  std::vector<gl::texture_t> textures;

  for (size_t i = 0; i < mesh->mNumVertices; i++) {
    gl::vertex_t vertex;

    vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y,
                       mesh->mVertices[i].z};

    if (mesh->HasNormals()) {
      vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y,
                       mesh->mNormals[i].z};
    }

    if (mesh->mTextureCoords[0]) {
      vertex.texture_coordinate = {mesh->mTextureCoords[0][i].x,
                                   mesh->mTextureCoords[0][i].y};
      vertex.tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y,
                        mesh->mTangents[i].z};
      vertex.bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y,
                          mesh->mBitangents[i].z};
    } else {
      vertex.texture_coordinate = {0.0f, 0.0f};
    }

    vertices.push_back(vertex);
  }

  for (size_t i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (size_t j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  std::vector<gl::texture_t> diffuse_maps = load_material_textures(
      material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

  std::vector<gl::texture_t> specular_maps = load_material_textures(
      material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

  std::vector<gl::texture_t> normal_maps =
      load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
  textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

  std::vector<gl::texture_t> height_maps =
      load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
  textures.insert(textures.end(), height_maps.begin(), height_maps.end());

  return mesh_t(vertices, indices, textures);
}

std::vector<gl::texture_t>
gl::model_t::load_material_textures(aiMaterial *mat, aiTextureType type,
                                    const std::string &typeName) {
  std::vector<gl::texture_t> textures;
  for (size_t i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);

    auto res = _textures_cache.find(str.C_Str());
    if (res != _textures_cache.end()) {
      textures.push_back(res->second);
      continue;
    }

    auto res_1 = texture_from_file(str.C_Str(), _dir, _gamma_correction);
    if (res_1.err != std::nullopt) {
      LOG_ERR(res_1.err.value());
      continue;
    }

    gl::texture_t texture = {
        .id = res_1.result,
        .type = typeName,
        .path = str.C_Str(),
    };
    textures.push_back(texture);
    _textures_cache.insert({texture.path, texture});
  }
  return textures;
}