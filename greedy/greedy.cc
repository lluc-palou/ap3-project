/* IDEA: Ordenar database per ratio punts/preu + posició?
   - Modificar la funció compare_players_efficiency per a deixar al final de la
     database aquelles posicions ja cobertes.
   - Ordenació de la database iterativa.
*/

#include <algorithm>
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

// Arguments passed in code execution.
string data_base;
string query;
string output_file;

// General data data structures, global variables and timing.
vector<Player> players;
vector<bool> used;
int best_points = 0;
int def, mig, dav, por, total_limit, player_limit;
double start_time, end_time;
double now() { return clock() / double(CLOCKS_PER_SEC); }

/* Returns the player with largest points per price ratio, used to sort the
player database. */
bool compare_players_efficiency(const Player &a, const Player &b) {
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
    double efficiency_a = (a.points * 1.0) / (a.price * 1.0);
    double efficiency_b = (b.points * 1.0) / (b.price * 1.0);

    return efficiency_a > efficiency_b;
  }
}

// Reads the soccer player database.
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

    if (player.price <= player_limit)
      players.push_back(player);
  }
  in.close();

  // Vector to mark used soccer players in the exhaustive search.
  used = vector<bool>(players.size(), false);

  // Sort database from most points scored to least
  sort(players.begin(), players.end(), compare_players_efficiency);
}

// Reads the given query, aka player configuration and price condavaints.
void read_query() {
  ifstream in;
  in.open(query);
  in >> def >> mig >> dav >> total_limit >> player_limit;
  in.close();
}

// Writes the best solution found by the algorithm till now in the output file.
void write_solution(const int &current_price, const int &current_points,
                    const vector<Player> &partial_solution) {
  /*
  // Sorts the solution based on soccer player positions for better formatting.
  sort(partial_solution.begin(), partial_solution.end(),
  compare_players_position);

  // Returns the player by lexicographical order.
  bool compare_players_position(const Player &a, const Player &b) {
      return a.position < b.position;
  }
  */

  ofstream out(output_file);
  out.setf(ios::fixed);
  out.precision(1);

  // Solution timing.
  end_time = now();
  double time = end_time - start_time;

  // Outputs the time for this solution.
  cout << "Found a solution in " << time << " seconds with " << current_points
       << " points!" << endl;
  out << time << endl;

  // Creates a map to group soccer players by position.
  unordered_map<string, vector<string>> players_position;

  // Fills the map.
  for (const Player &player : partial_solution) {
    players_position[player.position].push_back(player.name);
  }

  // Writes the tactic soccer players by positions.
  out << "POR: " << players_position["por"][0] << endl;

  out << "DEF: ";
  for (const string &name : players_position["def"]) {
    out << name << ";";
  }
  out << endl;

  out << "MIG: ";
  for (const string &name : players_position["mig"]) {
    out << name << ";";
  }
  out << endl;

  out << "DAV: ";
  for (const string &name : players_position["dav"]) {
    out << name << ";";
  }
  out << endl;

  // Writes best solution achieved points and price.
  out << "Punts: " << best_points << endl;
  out << "Preu: " << current_price << endl;

  out.close();
}

// Checks whether the query constraints are satisfied.
bool satisfies_query_constraints(int &def_count, int &mig_count, int &dav_count,
                                 int &por_count, int &current_price,
                                 int &current_points) {
  return (def_count == def) and (mig_count == mig) and (dav_count == dav) and
         (por_count == 1) and (current_price <= total_limit) and
         (current_points > best_points);
}

// Main algorithm concerning a greedy approach.
void greedy_search(int idx, int def_count, int mig_count, int dav_count,
                   int por_count, int current_price, int current_points,
                   vector<bool> &used, vector<Player> &partial_solution) {
  // Base case: a partial solution has been extended and can be considered as
  // a final problem solution.
  if (satisfies_query_constraints(def_count, mig_count, dav_count, por_count,
                                  current_price, current_points)) {
    best_points = current_points;
    write_solution(current_price, current_points, partial_solution);
    return;
  }

  // Recursive case:
  // Iterate through the players starting from index 'idx'.
  for (int i = idx; i < int(players.size()); ++i) {
    // Skip used players.
    if (not used[i] and players[i].price <= player_limit and
        current_price + players[i].price < total_limit) {
      // Check if adding the player satisfies the position constraints.
      if ((players[i].position == "def" and def_count < def) or
          (players[i].position == "mig" and mig_count < mig) or
          (players[i].position == "dav" and dav_count < dav) or
          (players[i].position == "por" and por_count == 0)) {

        // Add the player to the team.
        used[i] = true;
        partial_solution.push_back(players[i]);

        // Update counters, prices, and points.
        def_count += (players[i].position == "def");
        mig_count += (players[i].position == "mig");
        dav_count += (players[i].position == "dav");
        por_count += (players[i].position == "por");
        current_price += players[i].price;
        current_points += players[i].points;

        // Recursive call to consider the next player.
        greedy_search(i + 1, def_count, mig_count, dav_count, por_count,
                      current_price, current_points, used, partial_solution);

        // Backtrack: remove the last added player.
        used[i] = false;
        partial_solution.pop_back();
        def_count -= (players[i].position == "def");
        mig_count -= (players[i].position == "mig");
        dav_count -= (players[i].position == "dav");
        por_count -= (players[i].position == "por");
        current_price -= players[i].price;
        current_points -= players[i].points;
      }
    }
  }
}

int main(int argc, char **argv) {
  // Arguments passed in code execution.
  data_base = argv[1];
  query = argv[2];
  output_file = argv[3];

  // Reads the input files.
  read_query();
  read_data_base();

  // Algorithm execution, solution writting, and timing.
  start_time = now();
  vector<Player> partial_solution;
  greedy_search(0, 0, 0, 0, 0, 0, 0, used, partial_solution);
}
