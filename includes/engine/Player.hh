//
// Player.hh for Bomberman in /home/tran_0/rendu/cpp_bomberman/include
//
// Made by David Tran
// Login   <tran_0@epitech.net>
//
// Started on  Sun May  3 01:20:37 2015 David Tran
// Last update Sun Jun 14 15:29:49 2015 Hugo Prenat
//

#ifndef PLAYER_HH_
# define PLAYER_HH_

# include <vector>
# include <utility>
# include <mutex>
# include "Board.hh"
# include "Bomb.hh"
# include "AObj.hh"
# include "Crate.hh"
# include "BasicBomb.hh"
# include "ABomb.hh"

class	Bomb;

enum	keyActions
  {
    SPACE = 1, PAUSE = 2, NACT = 0, ERROR = -1
  };

class	Player : public AObj
{
public:
  Player(Board &Board);
  ~Player();
public: // Principal functions
  bool					playerSpawn(float x, float y, Board::Direction direction, int id);
  bool					turnLeft();
  bool					turnRight();
  bool					goAhead();
  bool					goBack();
  bool					selectDirection(Board::Direction);
  void					run_user();
  bool					userAction();
  keyActions				commandValue();
  void					checkPosPowerUp();
  void					update_bombs(gdl::Clock const &, Binput &);
public: // Player Safety
  bool					is_Alive() const;
  void					triggerAlive();

public: // Player Bomb tools
  void					addBomb();
  std::vector<Bomb *>			getBombs() const;
  std::vector<Bomb *>::const_iterator	getBombIt() const;
  bool					triggerOneBomb();
  void					powerUpRange();
  void					goAllExplosions();
  size_t				getScore() const;
  void					setScore(size_t add);

private:
  bool			_isAlive;
  std::vector<Bomb *>	_bombs;
  Board::Direction	_dir;
  size_t		_score;
};

#endif // !PLAYER_HH
