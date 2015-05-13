//
// Player.hh for Bomberman in /home/tran_0/rendu/cpp_bomberman/include
//
// Made by David Tran
// Login   <tran_0@epitech.net>
//
// Started on  Sun May  3 01:20:37 2015 David Tran
// Last update Wed May 13 15:53:58 2015 Jean-Baptiste Grégoire
//

#ifndef PLAYER_HH_
# define PLAYER_HH_

# include <vector>
# include "Map.hh"
# include "Bomb.hh"

# define PLAYER_CHAR	'p'

class	Player
{
public:
  Player(char Speed, bool alive, Map &map);
  ~Player();
public: // Principal functions
  bool					playerSpawn(int x, int y, Map::Direction direction);
  bool					turnLeft();
  bool					turnRight();
  bool					goAhead();
  bool					goBack();
  void					run_user();
public: // Player Safety
  bool					is_Alive() const;
  void					triggerAlive();

public: // Player Bomb tools
  void					addBomb();
  std::vector<Bomb*>			getBombs() const;
  std::vector<Bomb*>::const_iterator	getBombIt() const;
  bool					triggerOneBomb();
  void					powerUpRange();
public: // Speed Power-up
  char					getSpeed() const;
  void					setSpeed(char const &);

private:
  int			_x;
  int			_y;
  bool			_isAlive;
  std::vector<Bomb *>	_bombs;
  char			_speed;
  Map			&_map;
  Map::Direction	_dir;
};

#endif // !PLAYER_HH
