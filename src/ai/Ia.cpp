//
// Ia.cpp for Bomberman in /home/patoche/rendu/CPP/cpp_bomberman
//
// Made by Hugo Prenat
// Login   <prenat_h@epitech.eu>
//
// Started on  Mon May 18 15:23:47 2015 Hugo Prenat
// Last update Sat Jun 13 06:15:03 2015 Jean-Baptiste Grégoire
//

#include "Ia.hh"

Ia::Ia(std::string const &fileName, Board &Board) : Player(Board),
						    _fileName(fileName)
{
  int north = 0;
  int west = 1;
  int south = 2;
  int east = 3;

  std::cout << "je créé l'IA" << std::endl;
  _key = -1;
  _enemyX = -1;
  _enemyY = -1;
  _chai.add(chaiscript::fun(&Player::is_Alive, this), "iaAlive");
  _chai.add(chaiscript::fun(&Player::triggerOneBomb, this), "iaDropBomb");
  _chai.add(chaiscript::fun(&Ia::chooseDir, this), "iaChooseDir");
  _chai.add(chaiscript::fun(&Ia::getCloserEnemy, this), "iaCloserEnemy");
  _chai.add(chaiscript::fun(&Ia::setPosById, this), "iaPosById");
  _chai.add(chaiscript::fun(&Player::goAhead, this), "iaMove");
  _chai.add_global_const(chaiscript::const_var(north), "North");
  _chai.add_global_const(chaiscript::const_var(west), "West");
  _chai.add_global_const(chaiscript::const_var(south), "South");
  _chai.add_global_const(chaiscript::const_var(east), "East");
}

Ia::~Ia()
{

}

void	Ia::run_user()
{
  _chai.eval_file(_fileName);
  std::cout << "toto" << std::endl;
}

bool	Ia::chooseDir(const int dir)
{
  std::cout << "Before x = " << _x << " y = " << _y << std::endl;
  selectDirection(static_cast<Board::Direction>(dir));
  std::cout << "Afteer x = " << _x << " y = " << _y << std::endl;
  return (true);
}

Board&	Ia::getBoard() const
{
  return (_board);
}

std::map<float, AObj *>& Ia::getMapEnemy() const
{
  std::vector<std::vector<AObj *>>  v = _board.getFullBoard();
  std::map<float, AObj *>           *enemy = new std::map<float, AObj *>;

  for (std::vector<std::vector<AObj *>>::iterator it = v.begin();
       it != v.end(); ++it)
    {
      std::vector<AObj *> tmp = *it;
      for (std::vector<AObj *>::iterator i = tmp.begin(); i != tmp.end(); ++i)
        {
          AObj  *obj = *i;
          if (obj->getType() == PLAYER) {
            std::pair<float, float> pos = obj->getPos();
            float dist = sqrt(pow(_x - pos.first, 2) + pow(_y - pos.second, 2));

            if (dist != 0)
              enemy->insert(std::make_pair(dist, obj));
          }
        }
    }
  return (*enemy);
}


void Ia::moveToEnemy(const int id)
{
  AObj* enemy = getCloserEnemy();
  std::pair<float, float> pos = enemy->getPos();
  float xDist;
  float yDist;

  xDist = _x - pos.first;
  yDist = _y - pos.second;
  xDist = yDist - xDist;
}

std::string	Ia::getMap() const
{
  std::string map("########");

  return (map);
}

AObj*		Ia::getCloserEnemy() const
{
  std::map<float, AObj *>   enemy = getMapEnemy();

  std::map<float, AObj *>::iterator it = enemy.begin();
  return (it->second);
}

long int   Ia::getDistance(long int id) const
{
  // todo !
  (void)id;
  return (0);
}

long int   Ia::getCloserAvailableEnemy() const
{
  std::map<float, AObj *>   enemy = getMapEnemy();
  long int id(0);

  for (auto it = enemy.begin(); it != enemy.end(); ++it)
  {
    int tmp = getDistance((*it).first);
    if (id == 0)
      id = tmp;
    if (tmp < id)
      id =tmp;
  }
  if (id == 0 && enemy.size() > 0)
      id = enemy.begin()->first;
  return (id);
}

void	Ia::setPosById(const long int id)
{
  std::map<float, AObj *>   enemy = getMapEnemy();

  _enemyX = enemy[id]->getPos().first;
  return ;
}
