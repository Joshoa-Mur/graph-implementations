#include <vector>
#include <iostream>
#include <cassert>
#include <functional>
#include <queue>
#include <deque>
#include <set>
// НЕ ЗАБЫТЬ ПРОВЕРИТЬ РАБОТУ КОНСТРУКТОРОВ КОПИРОВАНИЯ 

struct IGraph {
    virtual ~IGraph() = default;
    
    virtual void AddEdge(int from, int to) = 0;
    virtual int VerticesCount() const = 0;
    virtual std::vector<int> GetNextVertices(int vertex) const = 0;
    virtual std::vector<int> GetPrevVertices(int vertex) const = 0;
};

class ListGraph : public IGraph {
    std::vector<std::vector<int>> adjacencyLists;

public:
    explicit ListGraph(int size) : adjacencyLists(size) {}

    explicit ListGraph(const IGraph& graph) {
        adjacencyLists.resize(graph.VerticesCount());
        for(int i = 0; i < graph.VerticesCount(); ++i)
            adjacencyLists[i] = graph.GetNextVertices(i);
    }

    void AddEdge(int from, int to) override {
        assert(from >= 0 && from < adjacencyLists.size());
        assert(to >= 0 && to < adjacencyLists.size());
        adjacencyLists[from].push_back(to);
    }

    int VerticesCount() const override { return adjacencyLists.size(); }

    std::vector<int> GetNextVertices(int vertex) const override {
        assert(vertex >= 0 && vertex < adjacencyLists.size());
        return adjacencyLists[vertex];
    }
    
    std::vector<int> GetPrevVertices(int vertex) const override {
        std::vector<int> result;
        for(size_t from = 0; from < adjacencyLists.size(); ++from)
            for(int to : adjacencyLists[from])
                if(to == vertex) result.push_back(from);
        return result;
    }
};

class MatrixGraph : public IGraph {
    std::vector<std::vector<bool>> adjacencyMatrix;

public:
    explicit MatrixGraph(int size) : adjacencyMatrix(size, std::vector<bool>(size, false)) {}

    explicit MatrixGraph(const IGraph& graph) {
        int size = graph.VerticesCount();
        adjacencyMatrix.resize(size, std::vector<bool>(size, false));
        for(int from = 0; from < size; ++from)
            for(int to : graph.GetNextVertices(from))
                adjacencyMatrix[from][to] = true;
    }

    void AddEdge(int from, int to) override {
        assert(from >= 0 && from < adjacencyMatrix.size());
        assert(to >= 0 && to < adjacencyMatrix.size());
        adjacencyMatrix[from][to] = true;
    }

    int VerticesCount() const override { return adjacencyMatrix.size(); }

    std::vector<int> GetNextVertices(int vertex) const override {
        std::vector<int> result;
        for(size_t to = 0; to < adjacencyMatrix.size(); ++to)
            if(adjacencyMatrix[vertex][to]) result.push_back(to);
        return result;
    }

    std::vector<int> GetPrevVertices(int vertex) const override {
        std::vector<int> result;
        for(size_t from = 0; from < adjacencyMatrix.size(); ++from)
            if(adjacencyMatrix[from][vertex]) result.push_back(from);
        return result;
    }
};

class SetGraph : public IGraph {
    std::vector<std::set<int>> adjacencySets;

public:
    explicit SetGraph(int size) : adjacencySets(size) {}

    explicit SetGraph(const IGraph& graph) {
        adjacencySets.resize(graph.VerticesCount());
        for(int from = 0; from < graph.VerticesCount(); ++from)
            for(int to : graph.GetNextVertices(from))
                adjacencySets[from].insert(to);
    }

    void AddEdge(int from, int to) override {
        assert(from >= 0 && from < adjacencySets.size());
        assert(to >= 0 && to < adjacencySets.size());
        adjacencySets[from].insert(to);
    }

    int VerticesCount() const override { return adjacencySets.size(); }

    std::vector<int> GetNextVertices(int vertex) const override {
        return std::vector<int>(adjacencySets[vertex].begin(), adjacencySets[vertex].end());
    }

    std::vector<int> GetPrevVertices(int vertex) const override {
        std::vector<int> result;
        for(size_t from = 0; from < adjacencySets.size(); ++from)
            if(adjacencySets[from].count(vertex)) result.push_back(from);
        return result;
    }
};

class ArcGraph : public IGraph {
    std::vector<std::pair<int, int>> edges;
    int verticesCount;

public:
    explicit ArcGraph(int size) : verticesCount(size) {}

    explicit ArcGraph(const IGraph& graph) : verticesCount(graph.VerticesCount()) {
        for(int from = 0; from < verticesCount; ++from)
            for(int to : graph.GetNextVertices(from))
                edges.emplace_back(from, to);
    }

    void AddEdge(int from, int to) override {
        assert(from >= 0 && from < verticesCount);
        assert(to >= 0 && to < verticesCount);
        edges.emplace_back(from, to);
    }

    int VerticesCount() const override { return verticesCount; }

    std::vector<int> GetNextVertices(int vertex) const override {
        std::vector<int> result;
        for(const auto& [from, to] : edges)
            if(from == vertex) result.push_back(to);
        return result;
    }

    std::vector<int> GetPrevVertices(int vertex) const override {
        std::vector<int> result;
        for(const auto& [from, to] : edges)
            if(to == vertex) result.push_back(from);
        return result;
    }
};



namespace GraphAlgorithms {
    void BFS(const IGraph& graph, int vertex, std::vector<bool>& visited, 
            const std::function<void(int)>& func) {
        std::queue<int> q;
        q.push(vertex);
        visited[vertex] = true;

        while(!q.empty()) {
            int current = q.front();
            q.pop();
            func(current);

            for(int next : graph.GetNextVertices(current)) {
                if(!visited[next]) {
                    visited[next] = true;
                    q.push(next);
                }
            }
        }
    }

    void mainBFS(const IGraph& graph, const std::function<void(int)>& func) {
        std::vector<bool> visited(graph.VerticesCount(), false);
        for(int i = 0; i < graph.VerticesCount(); ++i)
            if(!visited[i]) BFS(graph, i, visited, func);
    }

    void DFS(const IGraph& graph, int vertex, std::vector<bool>& visited, 
           const std::function<void(int)>& func) {
        visited[vertex] = true;
        func(vertex);
        
        for(int next : graph.GetNextVertices(vertex))
            if(!visited[next]) DFS(graph, next, visited, func);
    }

    void mainDFS(const IGraph& graph, const std::function<void(int)>& func) {
        std::vector<bool> visited(graph.VerticesCount(), false);
        for(int i = 0; i < graph.VerticesCount(); ++i)
            if(!visited[i]) DFS(graph, i, visited, func);
    }

    void topologicalSortInternal(const IGraph& graph, int vertex, 
                                std::vector<bool>& visited, std::deque<int>& sorted) {
        visited[vertex] = true;
        for(int next : graph.GetNextVertices(vertex))
            if(!visited[next]) topologicalSortInternal(graph, next, visited, sorted);
        sorted.push_front(vertex);
    }

    std::deque<int> topologicalSort(const IGraph& graph) {
        std::deque<int> sorted;
        std::vector<bool> visited(graph.VerticesCount(), false);
        for(int i = 0; i < graph.VerticesCount(); ++i)
            if(!visited[i]) topologicalSortInternal(graph, i, visited, sorted);
        return sorted;
    }
}

class TestGraph : public IGraph {
public:
    TestGraph() {
        AddEdge(0, 1);
        AddEdge(0, 2);
        AddEdge(1, 3);
        AddEdge(1, 4);
        AddEdge(2, 4);
    }
    
    void AddEdge(int from, int to) override {}
    int VerticesCount() const override { return 5; }
    std::vector<int> GetNextVertices(int vertex) const override {
        switch(vertex) {
            case 0: return {1, 2};
            case 1: return {3, 4};
            case 2: return {4};
            default: return {};
        }
    }
    std::vector<int> GetPrevVertices(int vertex) const override {
        switch(vertex) {
            case 1: return {0};
            case 2: return {0};
            case 3: return {1};
            case 4: return {1, 2};
            default: return {};
        }
    }
};

template<typename T>
void test_algorithm_consistency() {
    TestGraph base_graph;
    T graph(base_graph);
    
    
    std::vector<int> bfs_result;
    GraphAlgorithms::mainBFS(graph, [&](int v) { bfs_result.push_back(v); });
    
    
    assert((bfs_result == std::vector<int>{0, 1, 2, 3, 4}));

    
    auto sorted = GraphAlgorithms::topologicalSort(graph);
    
    // Допустимые варианты топологической сортировки
    const std::vector<std::deque<int>> valid_sorts = {
        {0, 1, 2, 4, 3},
        {0, 2, 1, 4, 3},
        {0, 1, 2, 3, 4},
        {0, 2, 1, 3, 4}
    };
    
    bool is_valid = false;
    for(const auto& valid : valid_sorts) {
        if(sorted == valid) {
            is_valid = true;
            break;
        }
    }
    assert(is_valid);
}

int main() {
    // Тестируем все реализации
    test_algorithm_consistency<ListGraph>();
    test_algorithm_consistency<MatrixGraph>();
    test_algorithm_consistency<SetGraph>();
    test_algorithm_consistency<ArcGraph>();

    std::cout << "All tests passed!" << std::endl;

    ListGraph graph(7);
    graph.AddEdge(0, 1);
    graph.AddEdge(0, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(1, 4);
    graph.AddEdge(2, 5);
    graph.AddEdge(2, 6);

    MatrixGraph matrixGraph(graph);
    SetGraph setGraph(matrixGraph);
    ArcGraph arcGraph(setGraph);


    std::cout << "MatrixGraph BFS: ";
    GraphAlgorithms::mainBFS(matrixGraph, [](int v) { std::cout << v << " "; });
    std::cout << "\n";
    
    std::cout << "SetGraph BFS:    ";
    GraphAlgorithms::mainBFS(setGraph, [](int v) { std::cout << v << " "; });
    std::cout << "\n";
    
    std::cout << "ArcGraph BFS:    ";
    GraphAlgorithms::mainBFS(arcGraph, [](int v) { std::cout << v << " "; });
    std::cout << "\n\n";
    return 0;
}