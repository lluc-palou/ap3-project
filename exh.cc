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

string database;
string input_file;
string output_file;
vector<Player> players;
vector<bool> used;
vector<Player> solution;
int current_points, current_price, best_points = 0;
int goal = 1;
int def, mid, str, total_limit, player_limit;
double start_time, end_time;
double now() { return clock() / double(CLOCKS_PER_SEC); }

void read_input() {
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

  used = vector<bool>(players.size());
  cout << players.size() << " players loaded" << endl;
  in.close();

  in.open(input_file); // Reading input
  in >> def >> mid >> str >> total_limit >> player_limit;
  in.close();
}

void print_output() {
  ofstream out(output_file);
  out.setf(ios::fixed);
  out.precision(3);

  end_time = now();
  double time = end_time - start_time;

  // Quedaria esbrinar quina posició te cada jugador per a printar per posicions
  cout << "Found a solution in " << time << " seconds!" << endl;
  out << time << endl;
  out << "Team:" << endl;
  for (const Player &player : solution) {
    out << "Name: " << player.name << ", Position: " << player.position
        << ", Price: " << player.price << ", Team: " << player.team
        << ", Points: " << player.points << endl;
  }
  out << "Total Points: " << best_points << ", Total Price: " << current_price
      << endl;
  out.close();
}

void exhaustive_search(int k) {
  // Equip complet, comprova si la solució és millor i escriu
  if (k == 11) {
    if (goal == 0 and def == 0 and mid == 0 and str == 0 and
        current_price <= total_limit) {
      if (current_points > best_points) {
        best_points = current_points;
        print_output();
      }
      return;
    }
  }

  else if (goal >= 0 and def >= 0 and mid >= 0 and str >= 0) {
    for (int i = 0; i < int(players.size()); i += 20) {
      if (not used[i]) {
        used[i] = true;
        solution.push_back(players[i]);
        current_points += players[i].points;
        current_price += players[i].price;
        if (players[i].position == "def") {
          --def;
        } else if (players[i].position == "mig") {
          --mid;
        } else if (players[i].position == "dav") {
          --str;
        } else
          --goal;
        exhaustive_search(k + 1);
        solution.pop_back();
        current_points -= players[i].points;
        current_price -= players[i].price;
        used[i] = false;
        if (players[i].position == "def") {
          ++def;
        } else if (players[i].position == "mig") {
          ++mid;
        } else if (players[i].position == "dav") {
          ++str;
        } else
          ++goal;
      }
    }
  }
}

int main(int argc, char **argv) {
  database = argv[1];
  input_file = argv[2];
  output_file = argv[3];

  read_input();

  start_time = now();
  exhaustive_search(0);

  return 0;
}
