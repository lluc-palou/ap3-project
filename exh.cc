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

struct Team {
  int points = 0;
  int price = 0;
  // int/string/vector<int> tactic;
};

string database;
string input_file;
string output_file;
vector<Player> players;
uint goal = 1;
uint def, mid, str, total_limit, player_limit;
double start_time, end_time;
double now() { return clock() / double(CLOCKS_PER_SEC); }

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

void print_output(string &output_file) {
  ofstream out(output_file);
  out.setf(ios::fixed);
  out.precision(3);

  end_time = now();
  double time = end_time - start_time;

  // Escriu solució
  out << time << endl;
  cout << time << endl;

  out.close();
}

void exhaustive_search() { print_output(output_file); }

int main(int argc, char **argv) {
  database = argv[1];
  input_file = argv[2];
  output_file = argv[3];

  read_input(database, input_file);

  start_time = now();
  exhaustive_search();
}
