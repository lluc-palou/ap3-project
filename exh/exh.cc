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
#include <string>
#include <cassert>
#include <ctime>
#include <vector>
#include <iomanip>
#include <unordered_map>
using namespace std;

// Declaration of output file.
string output_file;

// Definition of player data structure.
struct Player {
  string name;
  string position;
  int price;
  string team;
  int points;
};

// Definition of player database data structure.
struct Player_database {
  vector<Player> porters;
  vector<Player> defenses;
  vector<Player> migcampistes;
  vector<Player> davanters;
};

// Definition and goalkeeper initialisation of query data structure.
struct Query {
  int def;
  int mig;
  int dav;
  int por = 1;
  int total_limit;
  int player_limit;
};

// Definition and initialisation of partial solution data structure.
struct Partial_solution {
  int time;
  int def_count = 0;
  int mig_count = 0;
  int dav_count = 0;
  int por_count = 0;
  int current_price = 0;
  int current_points = 0;
  int best_points = 0;
  vector<Player> players;
};

// Definition of used players data structure.
struct Used_players {
  vector<bool> porters;
  vector<bool> defenses;
  vector<bool> migcampistes;
  vector<bool> davanters;
};

// Timing.
double start_time, end_time;
double now() { return clock() / double(CLOCKS_PER_SEC); }

// Compares two soccer players based on their points.
bool compare_players_by_points(const Player &a, const Player &b) {
  return a.points > b.points;
}

// Sorts the players having into accoutn its position by points in ascending order.
void sort_players_by_points(Player_database& data_base) {
  sort(data_base.porters.begin(), data_base.porters.end(), compare_players_by_points);
  sort(data_base.defenses.begin(), data_base.defenses.end(), compare_players_by_points);
  sort(data_base.migcampistes.begin(), data_base.migcampistes.end(), compare_players_by_points);
  sort(data_base.davanters.begin(), data_base.davanters.end(), compare_players_by_points);
}

// Reads the soccer player database.
Player_database read_data_base(string data_base) {
  Player_database database;

  ifstream in(data_base);
  while (not in.eof()) {
    Player player;

    // Reads raw soccer player data.
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

    // Stores players in separate vectors based on their positions.
    if (player.position == "por") {
      database.porters.push_back(player);
    } else if (player.position == "def") {
      database.defenses.push_back(player);
    } else if (player.position == "mig") {
      database.migcampistes.push_back(player);
    } else if (player.position == "dav") {
      database.davanters.push_back(player);
    }
  }
  in.close();
  sort_players_by_points(database);

  return database;
}

// Reads the given query, aka tactic configuration and price constraints.
Query read_query(string query) {
  Query query_constraints;

  ifstream in;
  in.open(query);
  in >> query_constraints.def >> query_constraints.mig >> query_constraints.dav >> 
  query_constraints.total_limit >> query_constraints.player_limit;
  in.close();

  return query_constraints;
}

// Initialises the used players data structure.
Used_players initialise_used_players(const Player_database& database) {
  Used_players used;

  used.defenses = vector<bool>(database.defenses.size(), false);
  used.migcampistes = vector<bool>(database.migcampistes.size(), false);
  used.davanters = vector<bool>(database.davanters.size(), false);
  used.porters = vector<bool>(database.porters.size(), false);

  return used;
}

// Compares two soccer players based on their positions in lexicographical order.
bool compare_players_by_position(const Player &a, const Player &b) {
        return a.position < b.position;
}

// Writes the best solution found by the algorithm till now in the output file.
void write_solution(Partial_solution feasible_solution) {

  // Sorts the solution based on soccer player positions for better formatting.
  sort(feasible_solution.players.begin(), feasible_solution.players.end(), compare_players_by_position);

  ofstream out(output_file, ios::app);
  out.setf(ios::fixed);
  out.precision(2);

  // Outputs the time for this solution.
  out << feasible_solution.time << endl;

  unordered_map<string, vector<string>> players_position;

  // Fills the map.
  for (const Player &player : feasible_solution.players) {
    players_position[player.position].push_back(player.name);
  }

  // Writes the tactic soccer players by positions.
  out << "POR: " << players_position["por"][0] << endl;

  out << "DEF: ";
  for (auto it = players_position["def"].begin(); it != players_position["def"].end(); ++it) {
    out << *it;
      if (next(it) != players_position["def"].end()) {
        out << ";";
      }
  }
  out << endl;

  out << "MIG: ";
  for (auto it = players_position["mig"].begin(); it != players_position["mig"].end(); ++it) {
    out << *it;
      if (next(it) != players_position["mig"].end()) {
        out << ";";
      }
  }
  out << endl;

  out << "DAV: ";
  for (auto it = players_position["dav"].begin(); it != players_position["dav"].end(); ++it) {
    out << *it;
      if (next(it) != players_position["dav"].end()) {
        out << ";";
      }
  }
  out << endl;

  // Writes best solution achieved points and price.
  out << "Punts: " << feasible_solution.best_points << endl;
  out << "Preu: " << feasible_solution.current_price << endl;
  out << endl;
  out.close();
}

// Checks whether the query constraints are satisfied.
bool satisfies_query_constraints(const Query& query_constraints, const Partial_solution& feasible_solution) {
  return feasible_solution.def_count == query_constraints.def and 
         feasible_solution.mig_count == query_constraints.mig and 
         feasible_solution.dav_count == query_constraints.dav and 
         feasible_solution.por_count == 1 and
         feasible_solution.current_price <= query_constraints.total_limit and 
         feasible_solution.current_points > feasible_solution.best_points;
}

// References the appropriate player vector based on player position.
vector<Player> get_players(const Player_database& database, string position) {
  if (position == "def") return database.defenses;
  else if (position == "mig") return database.migcampistes;
  else if (position == "dav") return database.davanters;
  else if (position == "por") return database.porters;
}

// References the appropriate player amount based on player position.
int& get_count(Partial_solution& feasible_solution, string position) {
  if (position == "def") return feasible_solution.def_count;
  else if (position == "mig") return feasible_solution.mig_count;
  else if (position == "dav") return feasible_solution.dav_count;
  else if (position == "por") return feasible_solution.por_count;
}

// References the appropriate query constraint based on player position.
int get_query_constraint(const Query& query_constraints, string position) {
  if (position == "def") return query_constraints.def;
  else if (position == "mig") return query_constraints.mig;
  else if (position == "dav") return query_constraints.dav;
  else if (position == "por") return query_constraints.por;
}

vector<bool>& get_used_players(Used_players& used, string position) {
  if (position == "def") return used.defenses;
  else if (position == "mig") return used.migcampistes;
  else if (position == "dav") return used.davanters;
  else if (position == "por") return used.porters;
}

void backtracking(const Player_database& database, const Query& query_constraints,
                    Used_players& used, Partial_solution& feasible_solution, 
                    const vector<string>& positions, int idx) {
  // Base case: a partial solution has been extended to a feasible solution and can be 
  // considered as a final problem solution. The following condition checks whether 
  // current solution satisfies the query constraints, and updates the best solution found 
  // till now.
  if(satisfies_query_constraints(query_constraints, feasible_solution)) {
    // Updates feasible solution atributes.
    end_time = now();
    feasible_solution.time = end_time - start_time;
    feasible_solution.best_points = feasible_solution.current_points;
    write_solution(feasible_solution);
    // Just for checking while production step.
    cout << "Running time: " << feasible_solution.time << endl;
    cout << feasible_solution.def_count << " " << feasible_solution.mig_count << " " << 
    feasible_solution.dav_count << " " << feasible_solution.por_count << " " << 
    feasible_solution.current_price << " " << feasible_solution.current_points << endl;
    return;
  }

  
  
  // Pruning condition.
  if (feasible_solution.current_price > query_constraints.total_limit) return;

  // Recursive case:
  // Extends the partial solution including a soccer player whether satisfies the query constraints.
  string position = positions[idx];
  const vector<Player>& players = get_players(database, position);
  if(get_count(feasible_solution, position) < get_query_constraint(query_constraints, position)) {
    for (int i = 0; i < int(players.size()); ++i) {
      // Pruning condition: checks whether adding the player exceeds the remaining budget.
        if (feasible_solution.current_price + players[i].price <= query_constraints.total_limit) {
          if (not get_used_players(used, position)[i] and (players[i].price <= query_constraints.player_limit)) {
          // Updates soccer player position counter, price, and points.
          feasible_solution.players.push_back(players[i]);
          get_used_players(used, position)[i] = true;
          get_count(feasible_solution, position)++;
          feasible_solution.current_price += players[i].price;
          feasible_solution.current_points += players[i].points;

          backtracking(database, query_constraints, used, feasible_solution, positions, idx);

          // Undo changes made during the recursive call.
          feasible_solution.current_price -= players[i].price;
          feasible_solution.current_points -= players[i].points;
          get_count(feasible_solution, position)--;
          get_used_players(used, position)[i] = false;
          feasible_solution.players.pop_back();
        }
      }
    }
  } 
  
  // Recursive call to the next player type.
  else if (idx+1 < int(positions.size())) {
    backtracking(database, query_constraints, used, feasible_solution, positions, idx+1);
  }
}

// Main algorithm concerning exhaustive search and backtracking.
void exhaustive_search(const Player_database& database, const Query& query_constraints, 
                       Used_players& used, Partial_solution& feasible_solution) {
  // Defines player positions.
  vector<string> positions = {"por", "def", "mig", "dav"};

  // Start the backtracking with the first player type.
  backtracking(database, query_constraints, used, feasible_solution, positions, 0);
}

int main(int argc, char **argv) {
  // Definition of arguments passed in execution.
  string data_base;
  string query;

  // Files implied in code execution.
  data_base = argv[1];
  query = argv[2];
  output_file = argv[3];

  // Reads the input files.
  Player_database database = read_data_base(data_base);
  Used_players used = initialise_used_players(database);
  Query query_constraints = read_query(query);
  
  // Algorithm execution, solution writting, and timing.
  start_time = now();
  Partial_solution feasible_solution;
  exhaustive_search(database, query_constraints, used, feasible_solution);
}