#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <stack>
#include <chrono>

std::unordered_set<int> parse_dot_graph(const std::string& dot_graph) {
    std::unordered_set<int> city_numbers;
    size_t pos = 0;
    while (pos < dot_graph.size()) {
        size_t num_start = dot_graph.find_first_of("0123456789", pos);
        if (num_start == std::string::npos) {
            break;
        }
        size_t num_end = dot_graph.find_first_not_of("0123456789", num_start);
        int num = std::stoi(dot_graph.substr(num_start, num_end - num_start));
        city_numbers.insert(num);
        pos = num_end;
    }
    return city_numbers;
}

std::vector<std::vector<int>> build_adjacency_list(const std::string& dot_graph, int num_vertices) {
    std::vector<std::vector<int>> adjacency_list(num_vertices);
    std::istringstream iss(dot_graph);
    std::string line;
    while (std::getline(iss, line)) {
        size_t arrow_pos = line.find("->");
        if (arrow_pos != std::string::npos) {
            int source = std::stoi(line.substr(0, arrow_pos));
            size_t semicolon_pos = line.find(";", arrow_pos);
            while (semicolon_pos != std::string::npos) {
                size_t target_start = line.find_first_of("0123456789", arrow_pos + 2);
                size_t target_end = line.find_first_not_of("0123456789", target_start);
                int target = std::stoi(line.substr(target_start, target_end - target_start));
                adjacency_list[target].push_back(source);
                arrow_pos = semicolon_pos + 1;
                semicolon_pos = line.find(";", arrow_pos);
            }
        }
    }
    return adjacency_list;
}

void dfs(const std::vector<std::vector<int>>& graph, int start, std::vector<bool>& visited) {
    std::stack<int> stack;
    stack.push(start);

    while (!stack.empty()) {
        int node = stack.top();
        stack.pop();
        visited[node] = true;

        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                stack.push(neighbor);
            }
        }
    }
}

std::unordered_set<int> get_reachable_cities(const std::string& dot_graph) {
    std::unordered_set<int> city_numbers = parse_dot_graph(dot_graph);
    int num_vertices = *std::max_element(city_numbers.begin(), city_numbers.end()) + 1;
    std::vector<std::vector<int>> adjacency_list = build_adjacency_list(dot_graph, num_vertices);

    std::unordered_set<int> reachable_cities;

    for (int city : city_numbers) {
        std::vector<bool> visited(num_vertices, false);
        dfs(adjacency_list, city, visited);
        if (std::all_of(city_numbers.begin(), city_numbers.end(), [&visited](int num) { return visited[num]; })) {
            reachable_cities.insert(city);
        }
    }

    return reachable_cities;
}

int main() {
    auto start_time = std::chrono::steady_clock::now();

    std::ifstream file("sredni2.dot");
    std::string dot_graph((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::unordered_set<int> reachable_cities = get_reachable_cities(dot_graph);

    std::vector<int> sorted_cities(reachable_cities.begin(), reachable_cities.end());
    std::sort(sorted_cities.begin(), sorted_cities.end());

    auto end_time = std::chrono::steady_clock::now();
    auto execution_time = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time).count();

    std::cout << "Reachable cities:";
    for (int city : sorted_cities) {
        std::cout << " " << city;
    }
    std::cout << std::endl;
    std::cout << "Execution time: " << execution_time << " seconds" << std::endl;

    return 0;
}