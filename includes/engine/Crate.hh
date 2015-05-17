//
// Crate.hh for Bomberman in /home/jibb/rendu/cpp_bomberman
//
// Made by Jean-Baptiste Grégoire
// Login   <jibb@epitech.net>
//
// Started on  Wed May  6 11:59:37 2015 Jean-Baptiste Grégoire
// Last update Wed May  6 13:24:03 2015 Jean-Baptiste Grégoire
//

#ifndef CRATE_HH_
# define CRATE_HH_

# include <cstdlib>
# include "AObj.hh"

class		Crate : public AObj
{
public:
  enum	BonusType
    {
      SPEED = 0, RANGE, ADD, NONE
    };

public:
  Crate(Board &board);
  ~Crate();

public:
  BonusType	getBonus() const;
  bool      isBreak() const;

private:
  BonusType	_bonus;
  bool			_break;
};

#endif // !CRATE_HH_