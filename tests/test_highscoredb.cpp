#include "catch_amalgamated.hpp"
#include "../include/HighScoreDB.h"
#include <cstdio>
#include <iostream>
#include <sqlite3.h> // INCLUDE ADICIONADO

// Mock para usar banco de dados em memória para testes
class TestHighScoreDB : public HighScoreDB {
private:
    std::string testDbPath;
    
public:
    TestHighScoreDB() : testDbPath(":memory:") {}
    
    bool initialize() override {
        sqlite3* db;
        if (sqlite3_open(testDbPath.c_str(), &db) != SQLITE_OK) {
            return false;
        }
        sqlite3_close(db);
        createTable();
        return true;
    }
    
    void createTable() override {
        sqlite3* db;
        if (sqlite3_open(testDbPath.c_str(), &db) == SQLITE_OK) {
            const char* sql = "CREATE TABLE IF NOT EXISTS highscores ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "player_name TEXT NOT NULL, "
                             "timestamp TEXT NOT NULL, "
                             "score INTEGER NOT NULL, "
                             "game_mode INTEGER NOT NULL)";
            sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
            sqlite3_close(db);
        }
    }
    
    bool addHighScore(const std::string& playerName, int score, GameMode mode) override {
        sqlite3* db;
        if (sqlite3_open(testDbPath.c_str(), &db) != SQLITE_OK) {
            return false;
        }

        // Timestamp fixo para testes
        std::string timestamp = "2024-01-01 12:00:00";

        const char* sql = "INSERT INTO highscores (player_name, timestamp, score, game_mode) "
                         "VALUES (?, ?, ?, ?)";
        
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, playerName.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, timestamp.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, score);
            sqlite3_bind_int(stmt, 4, static_cast<int>(mode));
            
            bool success = sqlite3_step(stmt) == SQLITE_DONE;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return success;
        }
        
        sqlite3_close(db);
        return false;
    }
    
    std::vector<HighScore> getTopScores(int limit = 5) override {
        std::vector<HighScore> scores;
        sqlite3* db;
        
        if (sqlite3_open(testDbPath.c_str(), &db) == SQLITE_OK) {
            const char* sql = "SELECT id, player_name, timestamp, score, game_mode "
                             "FROM highscores ORDER BY score DESC LIMIT ?";
            
            sqlite3_stmt* stmt;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, limit);
                
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    HighScore hs;
                    hs.id = sqlite3_column_int(stmt, 0);
                    hs.playerName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                    hs.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                    hs.score = sqlite3_column_int(stmt, 3);
                    hs.gameMode = static_cast<GameMode>(sqlite3_column_int(stmt, 4));
                    
                    scores.push_back(hs);
                }
                sqlite3_finalize(stmt);
            }
            sqlite3_close(db);
        }
        return scores;
    }
    
    void clearAllScores() {
        sqlite3* db;
        if (sqlite3_open(testDbPath.c_str(), &db) == SQLITE_OK) {
            const char* sql = "DELETE FROM highscores";
            sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
            sqlite3_close(db);
        }
    }
};