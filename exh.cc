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

int main(int argc, char **argv) {
  ifstream in(argv[1]); // Reading database

  vector<Player> players;

  while (not in.eof()) {
    Player player;
    getline(in, player.name, ';');
    if (player.name == "")
      break;
    cout << "Name: " << player.name << endl;

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

  for (const auto &player : players) {
    cout << "Nom: " << player.name << endl;
    cout << "Posició: " << player.position << endl;
    cout << "Preu: " << player.price << endl;
    cout << "Club: " << player.team << endl;
    cout << "Punts: " << player.points << endl;
    cout << endl;
  }

  cout << players.size() << endl;
  in.close();

  in.open(argv[2]); // Reading input
  uint goal = 1;
  uint def, mid, str, t, j;
  in >> def >> mid >> str >> t >> j;
  in.close();
}
