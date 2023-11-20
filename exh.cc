#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

struct Player {
  string name;
  string position;
  int price;
  string team;
  int points;
};

vector<Player> players;
uint goal = 1;
uint def, mid, str, total_limit, player_limit;

void read_input(const string &database, const string &input_file) {
  ifstream in(database); // Reading database

  while (not in.eof()) {
    Player player;
    getline(in, player.name, ';');
    if (player.name == "")
      break;
    getline(in, player.position, ';');
    in >> player.price;
    char aux;
    in >> aux;
    getline(in, player.team, ';');
    in >> player.points;
    string aux2;
    getline(in, aux2);

    players.push_back(player);
  }

  /*for (const auto &player : players) {
    cout << "Nom: " << player.name << endl;
    cout << "Posició: " << player.position << endl;
    cout << "Preu: " << player.price << endl;
    cout << "Club: " << player.team << endl;
    cout << "Punts: " << player.points << endl;
    cout << endl;
  }*/

  cout << players.size() << " players" << endl;
  in.close();

  in.open(input_file); // Reading input
  in >> def >> mid >> str >> total_limit >> player_limit;
  in.close();
}

int main(int argc, char **argv) {
  string database = argv[1];
  string input_file = argv[2];
  string output_file = argv[3];

  read_input(database, input_file);
}
