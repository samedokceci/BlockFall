#include "Leaderboard.h"
#include <iostream>
#include <ncurses.h>

void Leaderboard::insert_new_entry(LeaderboardEntry * new_entry) {
    

    if(head_leaderboard_entry==nullptr || head_leaderboard_entry->score < new_entry->score) {
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
        size++;
        return;
    }
    
    LeaderboardEntry* entry = head_leaderboard_entry;

    while(entry->next_leaderboard_entry!=nullptr && new_entry->score < entry->next_leaderboard_entry->score){
        entry = entry->next_leaderboard_entry;
    }
    new_entry->next_leaderboard_entry=entry->next_leaderboard_entry;
    entry->next_leaderboard_entry = new_entry;

    size++;
    if(size==11){
        entry = head_leaderboard_entry;
        while(entry->next_leaderboard_entry->next_leaderboard_entry!=nullptr){
            entry=entry->next_leaderboard_entry;
        }
        LeaderboardEntry* temp = entry->next_leaderboard_entry;
        entry->next_leaderboard_entry = nullptr;
        delete temp;
        size--;
    }
}

void Leaderboard::write_to_file(const string& filename) {

    std::fstream leaderboardFile;
    leaderboardFile.open(filename, std::ios::out);
    if (!leaderboardFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    
    LeaderboardEntry* entry = head_leaderboard_entry;

    while(entry!=nullptr) {
        leaderboardFile << entry->score << " " << entry->last_played << " " << entry->player_name << '\n';
        entry = entry->next_leaderboard_entry;
    }

    leaderboardFile.close();
}

void Leaderboard::read_from_file(const string& filename) {
    std::fstream leaderboardFile;
    leaderboardFile.open(filename, std::ios::in);
    if (!leaderboardFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    std::string line;
    while (std::getline(leaderboardFile, line)) {
        std::vector<std::string> splittedString;
        splitString(line, ' ', splittedString);
        if(!line.empty() || line!="\n"){
            insert_new_entry(new LeaderboardEntry(std::stoi(splittedString[0]), std::stoi(splittedString[1]), splittedString[2]));
        }
    }

    leaderboardFile.close();
}

void Leaderboard::splitString(const std::string& input, char delimiter, std::vector<std::string> &splittedString) {
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, delimiter)) {
        splittedString.push_back(token);
    }
}


void Leaderboard::print_leaderboard() {
    LeaderboardEntry* entry = head_leaderboard_entry;
    int h, w;
    getmaxyx(stdscr, h, w);
    mvprintw(h/2-6,w/2+15,"Leaderboard");
    mvprintw(h/2-5,w/2+15,"----------------------");
    char date[11];
    for(int i = h/2-4; entry!=nullptr; i++){
        strftime(date, sizeof(date), "%m/%d/%Y", localtime(&entry->last_played));
        const char* name = entry->player_name.c_str();
        mvprintw(i,w/2+15,"%s %d %s",name,(int)entry->score,date);
        entry = entry->next_leaderboard_entry;
    }
}

Leaderboard::~Leaderboard() {
    LeaderboardEntry* currentEntry = head_leaderboard_entry;
    while(currentEntry!=nullptr) {
        LeaderboardEntry* nextEntry = currentEntry->next_leaderboard_entry;
        delete currentEntry;
        currentEntry = nextEntry;
    }
}
