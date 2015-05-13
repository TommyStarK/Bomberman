//
// EThread.cpp for Bomberman in /home/tran_0/rendu/cpp_bomberman
// 
// Made by David Tran
// Login   <tran_0@epitech.net>
// 
// Started on  Wed May  6 17:11:54 2015 David Tran
// Last update Wed May  6 17:42:35 2015 David Tran
//

#include "EThread.hh"

EThread::EThread()
{

}

EThread::~EThread()
{}

int	EThread::launch(void *(*ptr)(void *), void *arg)
{
  if (pthread_create(&_thread, NULL, ptr, arg) != 0)
    {
      _running = false;
      return (EXIT_FAILURE);
    }
  _running = true;
  return (EXIT_SUCCESS);
}

bool	EThread::is_running() const
{
  return (_running);
}

int	EThread::waitThread()
{
  int	wait;

  wait = pthread_join(_thread, NULL);
  _running = false;
  return (wait);
}

void	*launch_player(void *arg)
{
  Core		*corpse = static_cast<Core *>(arg);
  Player	newone(1, true, corpse->getMap());

  newone.run_user();
  return (NULL);
}