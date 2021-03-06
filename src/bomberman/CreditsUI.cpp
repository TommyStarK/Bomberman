//
// CreditsUI.cpp for BombAERman in /home/tommy/Dropbox/Epitech/2ème Année/cpp_bomberman/src/bomberman/
//
// Made by TommyStarK
// Login   <milox_t@epitech.eu>
//
// Started on  Sat Jun 13 00:56:32 2015 TommyStarK
// Last update Sun Jun 14 19:38:00 2015 TommyStarK
//

#include "UIManager/CreditsUI.hh"

CreditsUI::CreditsUI()
  : _first(true), _spreading(10)
{
  _itemsName.push_back("EndBackground");
  _itemsName.push_back("GameOver");
  _itemsName.push_back("credits");
  this->setName("CreditsUI");
}

CreditsUI::~CreditsUI()
{

}

void                        CreditsUI::setupDisplay()
{
  if (!_camera.setupCamera(*_window))
    throw std::runtime_error("(CreditsUI::)setupDisplay - setup camera failed.");
  if (!_shader.load(FRAGMENT_SHADER, GL_FRAGMENT_SHADER)
      || !_shader.load(VERTEX_SHADER, GL_VERTEX_SHADER)
      || !_shader.build())
    throw std::runtime_error("(CreditsUI::)setupDisplay - load/build shader failed.");
  _camera.setPosition(glm::vec3(0, 0, -20));
  _camera.setZoom(glm::vec3(12, 12, 12));
  _camera.refreshPosition();
  _shader.bind();
  _camera.lockShader(_shader, true);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  this->setupItemsCredits();
  _window->flush();
}

void                        CreditsUI::setupItemsCredits()
{
  auto asset = AssetManager::instance();
  _factory = GeometryFactory::instanciate();
  _front[0] = new Geometric(glm::vec3(0, 0, 15.1), glm::vec3(0, 0, 0), glm::vec3(42, 26, 4));
  _front[1] = new Geometric(glm::vec3(0, 5, 12), glm::vec3(0, 0, 0), glm::vec3(8, 4, 1));
  _front[2] = new Geometric(glm::vec3(0, -5.5, 12), glm::vec3(0, 0, 0), glm::vec3(10, 4, 1));
  for (size_t i = 0; i < _itemsName.size(); i++) {
    _front[i]->setGeometry(_factory->getGeometry(GeometryFactory::VERTICAL_PLANE));
    _front[i]->setTexture((*(*THEME((*THEME_HANDLER(asset["themes"]))["default"]))[_itemsName[i]]));
    _front[i]->initialize();
    _front[i]->draw(_shader, _clock);
  }
}

void                        CreditsUI::updateContext()
{
  _window->updateContext(_clock, _input);
  for (size_t i = 0; i < _itemsName.size(); i++)
    _front[i]->update(_clock, _input._default);
  _camera.update(_clock, _input);
}

void                        CreditsUI::launch()
{
  _isRunning = true;
  _window = VideoContext::instanciate();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  this->setupDisplay();
}


stateUI                     CreditsUI::handlerEvent()
{
  sleep(3);
  this->updateContext();
  return (std::tuple<bool, std::string>(true, this->getName()));
}
