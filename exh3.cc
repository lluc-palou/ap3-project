/*
===============================================================================
Description:
  Given a database of soccer player information and a query with constraints
  about a tactic concerning the soccer players, this code will provide the 
  tactic configuration that maximizes the tactic points while satisfying the 
  imposed constraints. (Combinatorial optimization problem)

Input: 
  
  Three files given in order:

  1. The data base of soccer players containing its information: name, 
  position, price, team and points. (data_base)
  
  2. The query containing a tactic realization with the corresponding 
  constariants: the numer of defenders (def), midfielders (mig), strikers (dav),
  and an implicit goalkeeper, the total limit price of the tactic (total_limit) 
  and the maximum limit price for a single player (player_limit).

  3. The output file in which the code will write the best solutions that finds 
  while running. (output_file)

Output:

  Corresponds to the best solution found while running the code with the 
  following information: a double with only one decimal indicating the time 
  needed to find the best solution, the name of: the goalkeeper, defenders,
  midfielders and strikers; poitns, and total tactic price. All these 
  information will be written in the output file with proper format. 

Algorithms:
  Exhaustive search with backtracking.
===============================================================================
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <vector>
#include <iomanip>
#include <unordered_map>
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

// Returns the player with most points, used to sort the player database.
bool compare_players_price(const Player &a, const Player &b) {
  if (a.points != b.points) {
    return a.points > b.points;
  } else {
    // In case of same amount of points, sorting by price in ascending order.
    return a.price < b.price;
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

    players.push_back(player);
  }
  in.close();

  // Vector to mark used soccer players in the exhaustive search.
  used = vector<bool>(players.size(), false);
  // Sort database from most points scored to least
  // (greedy approach, can it be used in exh?)
  // sort(players.begin(), players.end(), compare_players_price);
}

// Reads the given query, aka player configuration and price condavaints.
void read_query() {
  ifstream in;
  in.open(query);
  in >> def >> mig >> dav >> total_limit >> player_limit;
  in.close();
}

// Writes the best solution found by the algorithm till now in the output file.
void write_solution(int& current_price, int& current_points, 
                    const vector<Player>& partial_solution) {
    // S'ha de resoldre aquesta part per imprimir en ordre els jugadors per posició.
    /*
    // Sorts the solution based on soccer player positions for better formatting.
    sort(partial_solution.begin(), partial_solution.end(), compare_players_position);

    // Returns the player by lexicographical order.
    bool compare_players_position(const Player &a, const Player &b) {
        return a.position < b.position;
    }
    */

    ofstream out(output_file, ios::app);
    out.setf(ios::fixed);
    out.precision(1);

    // Solution timing.
    end_time = now();
    double time = end_time - start_time;

    // Outputs the time for this solution.
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

    // Adds a separator between solutions.
    out << "------------------------------------" << endl;
    out.close();
}

// Checks whether the query constraints are satisfied.
bool satisfies_query_constraints(int& def_count, int& mig_count, int& dav_count, 
                                 int& por_count, int& current_price, int& current_points) {
  return def_count == def and mig_count == mig and dav_count == dav and por_count == 1 and
  current_price <= total_limit and current_points > best_points;
}

// Main algorithm concerning exhaustive search and backtracking.
void exhaustive_search(int idx, int def_count, int mig_count, int dav_count, 
                       int por_count, int current_price, int current_points,
                       vector<bool>& used, vector<Player>& partial_solution) {
  // Base case: a partial solution has been extended and can be considered as 
  // a final problem solution.
  if (idx == int(partial_solution.size())) {
    // Checks whether current solution satisfies the query constraints,
    // and updates the best solution found till now.
    if(satisfies_query_constraints(def_count, mig_count, dav_count, por_count, 
    current_price, current_points)) {
      best_points = current_points;
      write_solution(current_price, current_points, partial_solution);
      return;
    }
  }

  // Recursive case:
  // Extends the partial solution excluding the same soccer player.
  exhaustive_search(idx+1, def_count, mig_count, dav_count, por_count, current_price, 
  current_points, used, partial_solution);

  // Extends the partial solution including the next soccer player whether 
  // satisfies the query constraints.
  if (not used[idx] and ((players[idx].position == "def" and def_count < def) or
                      (players[idx].position == "mig" && mig_count < mig) or
                      (players[idx].position == "dav" && dav_count < dav) or
                      (players[idx].position == "por" && por_count == 0))) {
    
    // Soccer player is set to used.
    used[idx] = true;

        // Updates soccer player position counters, prices, and points.        
        def_count += (players[idx].position == "def");
        mig_count += (players[idx].position == "mig");
        dav_count += (players[idx].position == "dav");
        por_count += (players[idx].position == "por");
        current_price += players[idx].price;
        current_points += players[idx].points;
    
    exhaustive_search(idx+1, def_count, mig_count, dav_count, por_count, current_price,
    current_points, used, partial_solution);

    // Soccer player is set to unused.
    used[idx] = false;
  }
}

int main(int argc, char **argv) {
  // Arguments passed in code execution.
  data_base = argv[1];
  query = argv[2];
  output_file = argv[3];

  // Reads the input files.
  read_data_base();
  read_query();

  // Algorithm execution, solution writting, and timing.
  start_time = now();
  vector<Player> partial_solution;
  exhaustive_search(0, 0, 0, 0, 0, 0, 0, used, partial_solution);
}
