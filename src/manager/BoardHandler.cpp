//
// BoardHandler.cpp for Bomberman in
// /home/chambo_e/epitech/cpp_bomberman/src/bomberman/
//
// Made by Emmanuel Chambon
// Login   <chambo_e@epitech.eu>
//
// Started on  Wed May 20 15:53:08 2015 Emmanuel Chambon
// Last update Sat Jun 13 14:16:39 2015 Emmanuel Chambon
//

#include "BoardHandler.hh"

BoardHandler::BoardHandler()
{
	generate();
	load();
}

/*
** Save board to name.json
*/
void BoardHandler::save(std::shared_ptr<Board> const &board, std::string const &name) const
{
	rapidjson::Document doc;
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();

	doc.AddMember("name", rapidjson::Value(name.c_str(), name.length()), allocator);
	doc.AddMember("height", rapidjson::Value(board->getHeight() - 2), allocator);
	doc.AddMember("width", rapidjson::Value(board->getWidth() - 2), allocator);

	std::string tbPath("./assets/thumbnail/" + name + ".jpg");
	doc.AddMember("thumbnail", rapidjson::Value(tbPath.c_str(), tbPath.length()), allocator);

	rapidjson::Value map(rapidjson::kArrayType);

	for (size_t i = 0; i < board->size(); i++) {
		if ((*board)[i].size() == 0) {
			map.PushBack(::NONE, allocator);
			continue;
		}
		for (auto cas : (*board)[i]) {
			if (cas->getType() == CRATE || cas->getType() == UNBREAKABLE_WALL)
				map.PushBack(cas->getType(), allocator);
		}
	}

	doc.AddMember("map", map, allocator);

	std::string outputPath("./assets/maps/" + name + ".json");
	FILE* fp = fopen(outputPath.c_str(), "w");
	if (fp) {
		char buff[65536] = {0};
		rapidjson::FileWriteStream os(fp, buff, sizeof(buff));
		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		doc.Accept(writer);
		fclose(fp);
	}
}

/*
** Generate a random Board with the width and the height passed as parameters
** and insert it at the first place in the vector
*/
void BoardHandler::generate(int x, int y)
{
	if (x <= 10 && y <= 10)
		throw std::logic_error("Map width and height must be > 10");

	// Random init
	std::random_device rd;
	std::default_random_engine engine(rd());
	std::uniform_int_distribution<int> dist(::CRATE, ::END);
	std::uniform_int_distribution<int> wall(0, 10);

	// Board init
	BoardHandler::board_t board;

	board.name      = "Random";
	board.thumbnail = "./assets/random.jpg";
	board.board     = std::make_shared<Board>(x, y);

	// Board filling
	for (int y_ = 0; y_ < (y + 2); y_++) {
		for (int x_ = 0; x_ < (x + 2); x_++) {
			if (x_ == 0 || x_ == x + 1 || y_ == 0 || y_ == y + 1) {
				board.board->placeEntity(static_cast<float>(x_),
				                         static_cast<float>(y_),
				                         static_cast<::entityType>(::UNBREAKABLE_WALL),
				                         0);
				continue;
			}
			if (wall(engine) != wall(engine)) {
				board.board->placeEntity(static_cast<float>(x_),
				                         static_cast<float>(y_),
				                         static_cast<::entityType>(dist(engine)),
				                         0);
			}
			else {
				board.board->placeEntity(static_cast<float>(x_),
				                         static_cast<float>(y_),
				                         static_cast<::entityType>(::UNBREAKABLE_WALL),
				                         0);
			}
		}
	}
	// board.board->spawnPlayers();
	if (_boards.size() == 0)
		_boards.push_back(board);
	else
		_boards[0] = board;
}

/*
** Return vector of all the boards available
*/
std::vector<BoardHandler::board_t>              BoardHandler::getBoards() const
{
	return _boards;
}

/*
** Return vector of all the boards names available
*/
std::vector<std::string>              BoardHandler::getBoardsNames() const
{
	std::vector<std::string> ret;

	for (size_t i = 0; i < _boards.size(); i++)
		ret.push_back(_boards[i].name);
	return ret;
}

/*
** Translate a json map value into a Board Object
*/
std::shared_ptr<Board> BoardHandler::loadMap(rapidjson::Value const &map, int x, int y)
{
	if (map.Size() != static_cast<rapidjson::SizeType>((x + 2) * (y + 2)))
		throw std::invalid_argument("Corrupted JSON map. Will not be loaded.");

	std::shared_ptr<Board> board = std::make_shared<Board>(x, y);

	for (int y_ = 0; y_ < y + 2; y_++) {
		for (int x_ = 0; x_ < x + 2; x_++) {
			if (!map[(y * y_) + x_].IsNumber()
			    || static_cast<::entityType>(map[((y + 2) * y_) + x_].GetInt()) > ::END)
				throw std::invalid_argument("Corrupted JSON map. Will not be loaded.");
			board->placeEntity(static_cast<float>(x_),
			                   static_cast<float>(y_),
			                   static_cast<::entityType>(map[((y + 2) * y_) + x_].GetInt()),
			                   0);
		}
	}

	return board;
}

/*
** Translate a json map file into a board structure
*/
void BoardHandler::loadBoard(std::string const &file)
{
	size_t pos;

	if ((pos = file.find_last_of(".")) != std::string::npos && file.substr(pos) == ".json") {
		FILE* fp = fopen(std::string(file).c_str(), "r");
		if (fp) {
			char buff[65536] = {0};
			rapidjson::FileReadStream is(fp, buff, sizeof(buff));
			rapidjson::Document d;

			try {
				if (d.ParseStream(is).HasParseError()
				    || !d.HasMember("map")
				    || !d.HasMember("width")
				    || !d.HasMember("height"))
					throw std::invalid_argument(file + ": Invalid JSON file. Will not be loaded.");
				else {
					BoardHandler::board_t board;

					board.name = d.HasMember("name") && d["name"].IsString()
					             ? d["name"].GetString() : "";
					board.thumbnail = d.HasMember("thumbnail") && d["thumbnail"].IsString()
					                  ? d["thumbnail"].GetString() : "";
					if (!d["map"].IsArray() || !d["width"].IsNumber() || !d["height"].IsNumber())
						throw std::invalid_argument(file + ": Invalid JSON map. Will not be loaded.");
					try {
						board.board = loadMap(d["map"], d["width"].GetInt(), d["height"].GetInt());
					} catch(std::invalid_argument &e) {
						throw std::invalid_argument(file + ": " + e.what());
					}

					std::lock_guard<std::mutex> guard(_mutex);
					_boards.push_back(board);
				}
				fclose(fp);
			} catch (std::exception &e) {
				fclose(fp);
				throw;
			}
		}
	}
}

/*
** Load savec maps saved into the vector of available boards
*/
void BoardHandler::load()
{
	DIR *dir;
	struct dirent *ent;
	// Container for async calls
	std::vector<std::future<void>>  f;

	if ((dir = opendir ("./assets/maps")) != NULL) {
		// bind function to avoid static function
		auto func = std::bind(&BoardHandler::loadBoard, this, std::placeholders::_1);
		while ((ent = readdir (dir)) != NULL) {
			if (std::string(ent->d_name)[0] != '.')
				// Asynchronous load
				f.push_back(std::async (std::launch::async, func,
				                        "assets/maps/" + std::string(ent->d_name)));
		}
		try {
			for (auto i = f.begin(); i != f.end(); i++)
				// Asynchronous termination
				i->get();
		} catch (std::invalid_argument &e) {
			std::cerr << e.what() << std::endl;
		}
		closedir (dir);
	} else {
		std::cerr << "Cannot open maps folder. They will not be available" << std::endl;
	}
}

/*
** Alias and public for loadBoard()
*/
void BoardHandler::load(std::string const &file)
{
	loadBoard(file);
}

/*
** Access the Board Object at [at]
**
** Support reverse index (::[-1] will return the last element)
**
** !! Return only a Board instance and not a board_t structure.
*/
Board                          &BoardHandler::operator[](long int at) const
{
	if (static_cast<std::vector<BoardHandler::board_t>::size_type>(std::abs(at)) > _boards.size())
		throw std::out_of_range("Out of range query on BoardHandler");
	return (at < 0) ? *_boards[_boards.size() - at].board : *_boards[at].board;
}

/*
** Access the Board Object [name]
**
** !! Return only a Board instance and not a board_t structure.
*/
Board                          &BoardHandler::operator[](std::string const &name) const
{
	for (auto i : _boards)
		if (i.name == name)
			return *i.board;
	throw std::out_of_range("Out of range query on BoardHandler");
}

/*
** Access the board_t structure at [at]
**
** Support reverse index (::at(-1) will return the last element)
**
** !! Return only a board structure and not a Board instance.
*/
BoardHandler::board_t BoardHandler::at(long int at) const
{
	if (static_cast<std::vector<BoardHandler::board_t>::size_type>(std::abs(at)) > _boards.size())
		throw std::out_of_range("Out of range query on BoardHandler");
	return (at < 0) ? _boards[_boards.size() - at] : _boards[at];
}
