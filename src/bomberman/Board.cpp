//
// Board.cpp for Bomberman in /home/jibb/rendu/cpp_bomberman
//
// Made by Jean-Baptiste Grégoire
// Login   <jibb@epitech.net>
//
// Started on  Wed May  6 13:21:36 2015 Jean-Baptiste Grégoire
// Last update Sat Jun 13 18:59:51 2015 Jean-Baptiste Grégoire
//

#include "Board.hh"
#include "Player.hh"
#include "Crate.hh"
#include "UnbreakableWall.hh"
#include "Explosion.hh"
#include "Cube.hh"
#include "GameEngine.hh"
#include "Bonus.hh"

Board::Board(size_t xLength, size_t yLength) : _xLength(xLength + 2), _yLength(yLength + 2)
{
  _board.resize((xLength + 2) * (yLength + 2));
}

bool Board::initialize()
{
  int x = 0;

  int true_x;
  int true_y;
  for (auto it = _board.begin(); it != _board.end(); it++)
    {
      //      std::cout << "size = " << (*it).size() << std::endl;
      for (auto internIt = (*it).begin(); internIt != (*it).end(); internIt++)
	{
	  AGameObject*  obj;
	  true_x = (_xLength / 2) - (x % _xLength);
	  true_y = (_yLength / 2) - (x / _xLength);
	  switch ((*internIt)->getType())
	    {
	    case CRATE:
	      {
		obj = new Cube(glm::vec3(true_x, 0.5, true_y));
		obj->setTexture(*texCrate);
		(*internIt)->setGameObj(obj);
		break;
	      }
	    case UNBREAKABLE_WALL:
	      {
		obj = new Cube(glm::vec3(true_x, 0.5, true_y));
		// obj = new Cube(glm::vec3(-5 + (x / _xLength), 0, -5 + (x % _yLength)));
		//		obj = new Cube(glm::vec3((_xLength / 2 ) - (x / _xLength), 1, (x % _yLength) / 2 - (x % _yLength) / 2));
		obj->setTexture(*texWall);
		(*internIt)->setGameObj(obj);
		break;
	      }
	    case PLAYER:
	      {
		obj = new Character(glm::vec3(true_x, 0.5, true_y), "./assets/models/marvin.fbx");
		obj->scale(glm::vec3(0.0025f, 0.00025f, 0.0025f));
		obj->setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		(*internIt)->setGameObj(obj);
		reinterpret_cast<Character *>(obj)->setCurrentAnim(0);
		_players.push_back(reinterpret_cast<Player *>(*internIt));
		break;
	      }
	    default:
	      break;
	    }
	}
      ++x;
    }
  initGameObjects();
  return true;
}

void Board::initGameObjects()
{
  for (auto it = _board.begin(); it != _board.end(); it++)
    {
      for (auto internIt = (*it).begin(); internIt != (*it).end(); internIt++)
	{
	  if ((*internIt)->getType())
	    (*internIt)->getGameObj()->initialize();
	}
    }
}

AObj  *Board::createEntity(Board &board, entityType type)
{
	switch (type)
	{
	case PLAYER:
	  return (new Player(board));
	case CRATE:
		return (new Crate(board));
	case UNBREAKABLE_WALL:
		return (new UnbreakableWall(board));
	default:
		return (NULL);
	}
}

bool Board::placeEntity(float x, float y, entityType type, long int id, Direction dir)
{
  int to = static_cast<int>(y) * _xLength + static_cast<int>(x);
  AObj  *obj;

  if (_board[to].empty())
    {
      obj = createEntity(*this, type);
      if (!obj)
	return (false);
      if (type != PLAYER)
	obj->setPos(x, y);
      else
	{
	  reinterpret_cast<Player *>(obj)->playerSpawn(x, y, dir, id);
	  _players.push_back(reinterpret_cast<Player *>(obj));
	}
      _board[to].push_back(obj);
      return (true);
    }
  return (false);
}

bool Board::placeEntity(float x, float y, AObj *entity)
{
	int to = static_cast<int>(y) * _xLength + static_cast<int>(x);

	// if (!_board[to].empty())
	//   return (false);
	_board[to].push_back(entity);
	return (true);
}

void Board::popEntity(int x, int y, long int id)
{
  for (std::vector<AObj *>::iterator it = _board[y * _xLength + x].begin(); it != _board[y * _xLength + x].end(); ++it)
    {
      if ((*it)->getId() == id)
	_board[y * _xLength + x].erase(it);
    }
}

std::vector<AObj *> &Board::getCase(int at)
{
  return (_board[at]);
}

void Board::deleteEntity(float x, float y, long int id, bool breakWall)
{
  int posx = static_cast<int>(x), posy = static_cast<int>(y);
  std::vector<AObj *>   &tmp = getCase(posy * _xLength + posx);

  for (std::vector<AObj *>::iterator it = tmp.begin(); it != tmp.end(); )
    {
      if (id == 0)
	{
	  if ((*it)->getId() == Wall && breakWall == false)
	    {
	      ++it;
	      continue;
	    }
	  else if ((*it)->getId() == CrateID && reinterpret_cast<Crate *>(*it)->getBonus() != Crate::NONE &&
		   !(reinterpret_cast<Crate *>(*it)->isBreak()))
	    {
	      reinterpret_cast<Crate *>(*it)->breakIt();
	      // (*it)->setGameObj(new Bonus((*it)->getGameObj()->getPosition()));
	      // reinterpret_cast<Bonus *>((*it)->getGameObj())->load("./assets/models/bonusPower.fbx");
	      // (*it)->getGameObj()->setColor(glm::vec4(1.0f, 0.8f, 0.8f, 1.0f));
	      ++it;
	    }
	  else
	    {
	      //				delete *it;
	      if ((*it)->getType() == PLAYER)
		removePlayer((*it)->getId());
	      it = tmp.erase(it);
	    }
	}
      else if (id == (*it)->getId())
	{
	  //			delete *it;
	  if ((*it)->getType() == PLAYER)
	    removePlayer((*it)->getId());
	  it = tmp.erase(it);
	}
    }
}

void	Board::setExplosion(float x, float y)
{
  int posx = static_cast<int>(x), posy = static_cast<int>(y);
  int pos = static_cast<int>(posy) * _xLength + static_cast<int>(posx);
  int true_x = (_xLength / 2) - x;
  int true_y = (_yLength / 2) - y;
  Explosion     *exp = new Explosion(*this);
  exp->setGameObj(new AFX(glm::vec3(true_x, 1, true_y)));
  exp->getGameObj()->setScale(glm::vec3(2, 2, 2));
  reinterpret_cast<AFX *>(exp->getGameObj())->resetFrame();
  if (_board[pos].empty())
    _board[pos].push_back(exp);
}

AObj    *Board::removeFromSquare(int x, int y, long int id)
{
  AObj  *tmp;

  for (std::vector<AObj *>::iterator it = _board[y * _xLength + x].begin(); it != _board[y * _xLength + x].end(); ++it)
    {
      if ((*it)->getId() == id)
	{
	  tmp = *it;
	  _board[y * _xLength + x].erase(it);
	  break;
	}
    }
  for (std::vector<AObj *>::iterator it = _board[y * _xLength + x + 1].begin(); it != _board[y * _xLength + x + 1].end(); ++it)
    {
      if ((*it)->getId() == id)
	{
	  tmp = *it;
	  _board[y * _xLength + x + 1].erase(it);
	  break;
	}
    }
  for (std::vector<AObj *>::iterator it = _board[((y + 1) * _xLength) + x].begin(); it != _board[(y + 1) * _xLength + x].end(); ++it)
    {
      if ((*it)->getId() == id)
	{
	  tmp = *it;
	  _board[(y + 1) * _xLength + x].erase(it);
	  break;
	}
    }
  for (std::vector<AObj *>::iterator it = _board[((y + 1) * _xLength) + x + 1].begin(); it != _board[(y + 1) * _xLength + x + 1].end(); ++it)
    {
      if ((*it)->getId() == id)
	{
	  tmp = *it;
	  _board[(y + 1) * _xLength + x + 1].erase(it);
	  break;
	}
    }
  return (tmp);
}

void Board::updatePos(float x, float y, AObj *obj)
{
	float intPartX, intPartY;
	int intx = static_cast<int>(x), inty = static_cast<int>(y);

	std::modf(x, &intPartX);
	std::modf(x, &intPartY);
  obj->setPos(x, y);
	if (x < intPartX + 0.5)
		_board[inty * _xLength + intx].push_back(obj);
	else if (x > intPartX + 0.5)
		_board[inty * _xLength + intx + 1].push_back(obj);
	else
	{
		_board[inty * _xLength + intx].push_back(obj);
		_board[inty * _xLength + intx + 1].push_back(obj);
	}
	if (y < intPartY + 0.5)
		_board[inty * _xLength + intx].push_back(obj);
	else if (y > intPartY + 0.5)
		_board[inty + 1* _xLength + intx].push_back(obj);
	else
	{
		_board[inty * _xLength + intx].push_back(obj);
		_board[((inty + 1) * _xLength) + intx].push_back(obj);
	}
}

bool Board::moveEntity(float fromX, float fromY, float toX, float toY, long int id)
{
	AObj *tmp = removeFromSquare(fromX, fromY, id);
	updatePos(toX, toY, tmp);
	return (true);
}

bool Board::moveEntity(float x, float y, long int id, Direction dir)
{
  float toX = x, toY = y;
  int posX = static_cast<int>(x), posY = static_cast<int>(y);
  AObj  *tmp = NULL;

  switch (dir)
    {
    case North:
      if (y > 0)
	toY -= 0.1;
      else
	toY = -1.0;
      break ;
    case South:
      if (y < _yLength)
	toY += 0.1;
      else
	toY = -1.0;
      break ;
    case East:
      if (x > 0)
	toX -= 0.1;
      else
	toX = -1.0;
      break ;
    case West:
      if (x < _xLength)
	toX += 0.1;
      else
	toX = -1.0;
      break ;
    default: return(false); break;
    }
  if (toX == -1.0 || toY == -1.0) {
    return (false); }
  tmp = removeFromSquare(posX, posY, id);
  updatePos(toX, toY, tmp);
  return (true);
}

void Board::makeSomePlace(int x, int y, int id, Direction dir, Direction r1, Direction r2)
{
  if (r1 == West)
    _board[((y * _yLength) + x - 1)].clear();
  else if (r1 == East)
    _board[((y * _yLength) + x + 1)].clear();
  else if (r1 == North)
    _board[((y * _yLength) + x - _xLength)].clear();
  else if (r1 == South)
    _board[((y * _yLength) + x + _xLength)].clear();
  if (r2 == West)
    _board[((y * _yLength) + x - 1)].clear();
  else if (r2 == East)
    _board[((y * _yLength) + x + 1)].clear();
  else if (r2 == North)
    _board[((y * _yLength) + x - _xLength)].clear();
  else if (r2 == South)
    _board[((y * _yLength) + x + _xLength)].clear();

  _board[(y * _yLength) + x].clear();

  placeEntity(x, y, ::PLAYER, id, dir);
}

void Board::spawnPlayers(unsigned int nb)
{
  if (nb > ((_xLength - 2) * (_yLength - 2) - 10))
    throw std::logic_error("Map can't hold that much players.");

  unsigned int play = 1;

  makeSomePlace(1, 1, play, South, South, East);
  if (play++ == nb)
    return;
  makeSomePlace(_xLength - 2, _yLength - 2, play, North, West, North);
  if (play++ == nb)
    return;
  makeSomePlace(1, _yLength - 2, play, North, North, East);
  if (play++ == nb)
    return;
  makeSomePlace(_xLength - 2, 1, play, South, West, South);
  if (play++ == nb)
    return;
  makeSomePlace(_xLength - 2, (_yLength - 2) / 2, play, West, West, North);
  if (play++ == nb)
    return;
  makeSomePlace(1, (_yLength - 2) / 2, play, East, South, East);
  if (play++ == nb)
    return;
  makeSomePlace((_xLength - 2) / 2, 1, play, South, South, East);
  if (play++ == nb)
    return;
  makeSomePlace((_xLength - 2) / 2, _yLength - 2, play, North, West, North);
  if (play++ == nb)
    return;
}

void Board::removePlayer(long int id)
{
  for (std::vector<Player *>::iterator it = _players.begin(); it != _players.end(); ++it)
    {
      if ((*it)->getId() == id)
	{
	  (*it)->triggerAlive();
	  _players.erase(it);
	  return ;
	}
    }
}

bool Board::checkOneCollision(std::vector<AObj *> field, AObj *player)
{
  auto it = field.begin();
  AGameObject   *playObj = player->getGameObj();

  while (it != field.end())
    {
      if (player->getType() != (*it)->getType() && (*it)->getId() != -4)
	{
	  if ((*it)->getId() == -2 && reinterpret_cast<Crate *>((*it))->isBreak())
	    return (false);
	  if (playObj->collide((*(*it)->getGameObj())))
	    return (true);
	}
      it++;
    }
  return (false);
}

bool Board::collideAround(AObj *player, long int x, long int y)
{
	bool collider = false;

	x = (x - (static_cast<long int>(_xLength) / 2)) < 0 ? (x - (static_cast<long int>(_xLength) / 2)) * -1 : (x - (static_cast<long int>(_xLength) / 2));
	y = (y - (static_cast<long int>(_yLength) / 2)) < 0 ? (y - (static_cast<long int>(_yLength) / 2)) * -1 : (y - (static_cast<long int>(_yLength) / 2));
	if (x < 1 || y < 1 || x > static_cast<long int>(_xLength) || y > static_cast<long int>(_yLength))
		collider = !collider;
	if (!collider && y > 0)
		collider = (checkOneCollision(_board[(y - 1) * static_cast<long int>(_xLength) + x], player)) ? true :
		           ((x > 0) ? ((checkOneCollision(_board[(y - 1) * _xLength + x - 1], player)) ? true :
		                       (x < (static_cast<long int>(_xLength) - 1)) ? ((checkOneCollision(_board[(y - 1) * static_cast<long int>(_xLength) + x + 1], player)) ? true : false) : false) : false);
	if (!collider && y < (static_cast<long int>(_yLength) - 1))
		collider = (checkOneCollision(_board[(y + 1) * static_cast<long int>(_xLength) + x], player)) ? true :
		           ((x > 0) ? ((checkOneCollision(_board[(y + 1) * static_cast<long int>(_xLength) + x - 1], player)) ? true :
		                       (x < (static_cast<long int>(_xLength) - 1)) ? ((checkOneCollision(_board[(y + 1) * static_cast<long int>(_xLength) + x + 1], player)) ? true : false) : false) : false);
	if (!collider)
		collider = (x > 0) ? ((checkOneCollision(_board[(y) * static_cast<long int>(_xLength) + x - 1], player)) ? true :
		                      ((x < (static_cast<long int>(_xLength) - 1)) ? checkOneCollision(_board[(y) * static_cast<long int>(_xLength) + x + 1], player) : false)) : false;
	if (!collider)
		collider = checkOneCollision(_board[(y) * static_cast<long int>(_xLength) + x], player);
	return (collider);
}

std::vector<AObj *> &Board::getSquareObjects(size_t x, size_t y)
{
	return (_board[y * _xLength + x]);
}

std::vector<AObj *> &Board::operator[](size_t at)
{
	return _board[at];
}

size_t Board::size() const
{
	return _board.size();
}

std::vector<Player *>  &Board::getPlayers()
{
	return (_players);
}

size_t Board::getWidth() const
{
	return _xLength;
}

size_t Board::getHeight() const
{
	return _yLength;
}

std::vector<std::vector<AObj * > >        &Board::getFullBoard()
{
	return (_board);
}

Board::~Board()
{
  while (!_board.empty())
    {
      std::vector<AObj *> v = _board.back();
      while (!v.empty())
	{
	  //	  delete v.back();
	  v.pop_back();
	}
      _board.pop_back();
    }
}
