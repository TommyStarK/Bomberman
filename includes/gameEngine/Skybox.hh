//
// Skybox.hh for CPP_BOMBERMAN in /home/Jamais/cpp_bomberman
//
// Made by Jamais
// Login   <Jamais@epitech.net>
//
// Started on  Sat May 23 01:10:50 2015 Jamais
// Last update Sat May 23 01:47:24 2015 Jamais
//

#ifndef			___SKYBOX_HH___
# define		___SKYBOX_HH___

# include		"Geometric.hh"

class			Skybox
{
public:
  std::vector<Geometric *>	_faces;
  glm::vec3			_rotation;

public:
  Skybox();
  ~Skybox();

public:
  void			draw(gdl::AShader& shader, gdl::Clock const& clock);
};

#endif			/* !___SKYBOX_HH___ */