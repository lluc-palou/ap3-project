// Greedy Algorithm.
// Authors: Lluc Palou and Ramon Ventura.

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

// Definition of soccer player class.
struct Player {
  string name;
  string position;
  int price;
  string team;
  int points;
};

string data_base;
string query;
string output_file;

// General data data structures, global variables and timing.
vector<Player> players;
int def, mig, dav, por, total_limit, player_limit;
double start_time, end_time;
double now() { return clock() / double(CLOCKS_PER_SEC); }

/* Returns the most efficient player based on the following criteria, used to
sort the player database. */
bool compare_players_efficiency(const Player &a, const Player &b) {
  /* Ordering criteria: Efficciency points/price ratio with a penalization to
     expensive players. By manually experimenting with roots, the power
     function (0.35) has shown to be very effective. */

  // If both players have 0 points, order them based on price.
  if (a.points == 0 and b.points == 0) {
    return a.price < b.price;
  }

  // If one player has 0 points, it should be considered less efficient.
  if (a.points == 0) {
    return false;
  } else if (b.points == 0) {
    return true;

  } else {
    double efficiency_a = (a.points * 1.0) / pow(a.price, 0.35);
    double efficiency_b = (b.points * 1.0) / pow(b.price, 0.35);
    return efficiency_a > efficiency_b;
  }
}

/* Reads and stores the soccer player database. Discards players that surpass
   the player limit price. */
void read_data_base() {
  ifstream in(data_base);
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

    // Price restriction
    if (player.price <= player_limit)
      players.push_back(player);
  }
  in.close();

  // Sorts database accordingly to the ordering criteria.
  sort(players.begin(), players.end(), compare_players_efficiency);
}

// Reads the given query. That is, player configurations and price condavaints.
void read_query() {
  ifstream in;
  in.open(query);
  in >> def >> mig >> dav >> total_limit >> player_limit;
  in.close();
}

/* Auxiliar printing function. Given a position prints all the players from the
solution that belong to it, in the correct format. */
void aux_write_solution(ofstream &out,
                        unordered_map<string, vector<string>> &players_position,
                        const string &position) {
  bool first = true;
  for (const string &name : players_position[position]) {
    if (!first) {
      out << ";" << name;
    } else {
      out << name;
      first = false;
    }
  }
  out << endl;
}

// Given a solution, ends timing and prints both in the required format.
void write_solution(const int &current_price, const int &current_points,
                    const vector<Player> &partial_solution) {
  ofstream out(output_file);
  out.setf(ios::fixed);
  out.precision(1);

  end_time = now();
  double time = end_time - start_time;
  out << time << endl;

  // Creates a map to group soccer players by position and fills the map.
  unordered_map<string, vector<string>> players_position;
  for (const Player &player : partial_solution) {
    players_position[player.position].push_back(player.name);
  }

  out << "POR: ";
  aux_write_solution(out, players_position, "por");
  out << "DEF: ";
  aux_write_solution(out, players_position, "def");
  out << "MIG: ";
  aux_write_solution(out, players_position, "mig");
  out << "DAV: ";
  aux_write_solution(out, players_position, "dav");

  out << "Punts: " << current_points << endl;
  out << "Preu: " << current_price << endl;

  out.close();
}

/* Main algorithm concerning a greedy approach. Finds the first 11 players,
   ordered by defined criteria (efficiency ratio) that meet the constraints. */
void greedy_search(int def_count, int mig_count, int dav_count, int por_count,
                   int current_price, int current_points, vector<bool> &used,
                   vector<Player> &partial_solution) {
  int selected_players = 0;
  while (selected_players < 11) {
    for (int i = 0; i < int(players.size()); ++i) {
      /* Skips used players, as well as the ones that do not meet the price
         constraints. */
      if (not used[i] and current_price + players[i].price < total_limit) {
        // Checks if adding the player satisfies the position constraints.
        if ((players[i].position == "def" and def_count < def) or
            (players[i].position == "mig" and mig_count < mig) or
            (players[i].position == "dav" and dav_count < dav) or
            (players[i].position == "por" and por_count == 0)) {

          // Adds the player to the team.
          used[i] = true;
          partial_solution.push_back(players[i]);

          // Updates counters, prices, and points.
          def_count += (players[i].position == "def");
          mig_count += (players[i].position == "mig");
          dav_count += (players[i].position == "dav");
          por_count += (players[i].position == "por");
          current_price += players[i].price;
          current_points += players[i].points;

          ++selected_players;
        }
      }
    }
  }
  write_solution(current_price, current_points, partial_solution);
}

int main(int argc, char **argv) {
  data_base = argv[1];
  query = argv[2];
  output_file = argv[3];

  /* Firstly reads the query to store player limit. Allows us to filter them
     during the database reading process. */
  read_query();
  read_data_base();

  // Algorithm execution, solution writting, and timing.
  start_time = now();
  vector<Player> partial_solution;
  vector<bool> used(players.size(), false);
  greedy_search(0, 0, 0, 0, 0, 0, used, partial_solution);
}
