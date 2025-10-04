#ifndef HIGHSCOREDB_H
#define HIGHSCOREDB_H

#include <string>
#include <vector>
#include "GameConstants.h"
#include "GameMode.h"

struct HighScore {
    int id;
    std::string playerName;
    std::string timestamp;
    int score;
    GameMode gameMode;
};

class HighScoreDB {
private:
    std::string dbPath;
    
    void createTable();
    
public:
    HighScoreDB();
    virtual ~HighScoreDB() = default; // Destructor virtual
    
    virtual bool initialize();
    virtual bool addHighScore(const std::string& playerName, int score, GameMode mode);
    virtual std::vector<HighScore> getTopScores(int limit = 5);
};

#endif