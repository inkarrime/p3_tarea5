#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <vector>

class Node {
private:
    int id;
    std::vector<int> neighbors;

public:
    explicit Node(int id) : id(id) {}

    int getId() const { return id; }
    const std::vector<int>& getNeighbors() const { return neighbors; }
    void addNeighbor(int neighbor) { neighbors.push_back(neighbor); }
};

class Graph {
private:
    std::map<int, Node> nodes;

    Node& getOrCreate(int id) {
        auto it = nodes.find(id);
        if (it == nodes.end()) {
            it = nodes.emplace(id, Node(id)).first;
        }
        return it->second;
    }

    bool contains(int id) const { return nodes.find(id) != nodes.end(); }

    const std::vector<int>& neighborsOf(int id) const {
        return nodes.at(id).getNeighbors();
    }

    friend class BFSIterator;
    friend class DFSIterator;

public:
    void addEdge(int from, int to) {
        getOrCreate(to);
        getOrCreate(from).addNeighbor(to);
    }
};

class GraphIterator {
public:
    virtual bool hasNext() const = 0;
    virtual int next() = 0;
    virtual ~GraphIterator() = default;
};

class BFSIterator : public GraphIterator {
private:
    const Graph& graph;
    std::queue<int> pending;
    std::set<int> visited;

public:
    BFSIterator(const Graph& graph, int start) : graph(graph) {
        if (graph.contains(start)) {
            pending.push(start);
            visited.insert(start);
        }
    }

    bool hasNext() const override { return !pending.empty(); }

    int next() override {
        int current = pending.front();
        pending.pop();
        for (int neighbor : graph.neighborsOf(current)) {
            if (visited.insert(neighbor).second) {
                pending.push(neighbor);
            }
        }
        return current;
    }
};

class DFSIterator : public GraphIterator {
private:
    const Graph& graph;
    std::stack<int> pending;
    std::set<int> visited;

    void skipVisited() {
        while (!pending.empty() && visited.count(pending.top()) > 0) {
            pending.pop();
        }
    }

public:
    DFSIterator(const Graph& graph, int start) : graph(graph) {
        if (graph.contains(start)) {
            pending.push(start);
        }
    }

    bool hasNext() const override { return !pending.empty(); }

    int next() override {
        int current = pending.top();
        pending.pop();
        visited.insert(current);
        const std::vector<int>& neighbors = graph.neighborsOf(current);
        for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
            if (visited.count(*it) == 0) {
                pending.push(*it);
            }
        }
        skipVisited();
        return current;
    }
};

int main() {
    Graph g;
    g.addEdge(1,2);
    g.addEdge(1,3);
    g.addEdge(2,4);
    std::unique_ptr<GraphIterator> it =
        std::make_unique<BFSIterator>(g, 1);
    while(it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;

    it = std::make_unique<DFSIterator>(g, 1);
    while (it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;

    g.addEdge(4, 1);
    it = std::make_unique<BFSIterator>(g, 1);
    while (it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;
}
