//
// SoundHandler.cpp for  in /home/chambo_e/epitech/cpp_bomberman/src/bomberman/
//
// Made by Emmanuel Chambon
// Login   <chambo_e@epitech.eu>
//
// Started on  Sun May 24 08:37:30 2015 Emmanuel Chambon
// Last update Sun Jun 14 02:50:44 2015 Emmanuel Chambon
//

#include "SoundHandler.hh"

SoundHandler::SoundHandler()
{
	// Create FMOD environment
	fmodError(FMOD::System_Create(&_system));
	// Init FMOD with 32 channels capacity
	fmodError(_system->init(32, FMOD_INIT_NORMAL, 0));
	load();
}

SoundHandler::~SoundHandler()
{
	for (auto i : _sounds)
		fmodError(i.second->release());
	_sounds.clear();
	fmodError(_system->close());
	fmodError(_system->release());
}

/*
** Load sound located at [file] path
*/
void SoundHandler::loadSound(std::string const &file)
{
	FMOD::Sound         *sound = nullptr;

	// Open a stream to a sound.
	// A stream is used to avoid slowdown when huge files are opened
	fmodError(_system->createSound(file.c_str(), FMOD_DEFAULT | FMOD_CREATESTREAM, nullptr, &sound));

	std::smatch m;

	if (std::regex_search(file, m, std::regex("[^/]*$")))
	{
		std::string res(m[0]);
		std::regex sep("\\.");
		std::regex_token_iterator<std::string::iterator> end;
		std::regex_token_iterator<std::string::iterator> token(res.begin(),
		                                                       res.end(), sep, -1);

		if (std::distance(token, end) != 2)
			throw std::invalid_argument(file + ": Invalid file.");
		std::string asset(*token);

		// Avoid race condition on _sounds
		std::lock_guard<std::mutex> lock(_mutex);
		_sounds[asset] = sound;
	}
}

/*
** Load asynchronously all sounds located at assets/sounds
*/
void SoundHandler::load()
{
	DIR                 *dir;
	struct dirent       *ent;
	std::vector<std::future<void>>  f;

	if ((dir = opendir ("./assets/sounds")) != NULL) {
		auto func = std::bind(&SoundHandler::loadSound, this, std::placeholders::_1);
		while ((ent = readdir (dir)) != NULL) {
			if (std::string(ent->d_name)[0] != '.')
				f.push_back(std::async (std::launch::async, func,
				                        "assets/sounds/" + std::string(ent->d_name)));
		}
		try {
			for (auto i = f.begin(); i != f.end(); i++)
				i->get();
		} catch (std::invalid_argument &e) {
			std::cerr << e.what() << std::endl;
		}
		closedir (dir);
	} else {
		std::cerr << "Cannot open sounds folder. They will not be available" << std::endl;
	}
}

/*
** Pause the background channel
*/
void SoundHandler::pause() const
{
	bool paused;

	fmodError(_channel->getPaused(&paused));
	fmodError(_channel->setPaused(!paused));
}

/*
** Resume the background channel
*/
void SoundHandler::resume() const
{
	pause();
}

/*
** Stop the current sound on the current channel
*/
void SoundHandler::stop() const
{
	bool paused;

	fmodError(_channel->isPlaying(&paused));
	if (paused)
		fmodError(_channel->stop());
}

/*
** Play a sound
** if [sample] is true play it in the foreground channel
** if [loop] is true sound is played indefinitly
*/
void SoundHandler::play(std::string const &sound, bool sample, bool loop)
{
	if (loop)
		fmodError(_sounds[sound]->setMode(FMOD_LOOP_NORMAL));

	if (sample) {
		fmodError(_system->playSound(_sounds[sound], 0, false, &_channelSample));
		fmodError(_channelSample->setVolume(_foregroundVolume));
	} else {
		fmodError(_system->playSound(_sounds[sound], 0, false, &_channel));
		fmodError(_channel->setVolume(_backgroundVolume));
	}
}
/*
** Check if a sound is played on the background channel
*/
bool SoundHandler::isPlaying() const
{
	bool paused;

	fmodError(_channel->isPlaying(&paused));
	return paused;
}

/*
** Set the volume on the foreground channel
*/
void SoundHandler::setForegroundVolume(float volume)
{
	_foregroundVolume = volume;
	fmodError(_channelSample->setVolume(_foregroundVolume));
}

/*
** Set the volume on the background channel
*/
void SoundHandler::setBackgroundVolume(float volume)
{
	_backgroundVolume = volume;
	fmodError(_channel->setVolume(_backgroundVolume));
}

/*
** Public accessor for sounds
*/
FMOD::Sound             *SoundHandler::operator[](std::string const &at)
{
	if (_sounds.find(at) == _sounds.end())
		throw std::out_of_range("Out of range ThemeHandler.");
	return _sounds[at];
}

/*
** Error handler for FMOD
*/
void SoundHandler::fmodError(FMOD_RESULT res) const
{
	if (res != FMOD_OK)
		throw std::runtime_error("A problem happened with fmod. Please check your sounds assets.");
}
