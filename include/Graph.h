#ifndef GRAPH_H
#define GRAPH_H

#include "DoublyLinkedList.h"
#include "DynamicArray.h"
#include "LinkedPriorityQueue.h"
#include "MinHeap.h"
#include "Queue.h"
#include "Stack.h"
#include <climits>
#include <stdexcept>

enum NetworkStatus { OPEN, BLOCKED, REMOVED };
enum RouteMode { FASTEST, CHEAPEST, FEWEST_TRANSFERS };

struct Connection {
    int from, to, time, fare, line;
    bool twoWay;
    NetworkStatus status;

    Connection(int a = 0, int b = 0, int minutes = 0, int rupees = 0,
               int service = 0, bool bidirectional = true)
        : from(a), to(b), time(minutes), fare(rupees), line(service),
          twoWay(bidirectional), status(OPEN) {}
};

struct RouteResult {
    DynamicArray<int> stops;
    DynamicArray<int> connections;
    long long totalCost, time, fare;
    int transfers;
    bool found;

    RouteResult() : totalCost(0), time(0), fare(0), transfers(0), found(false) {}
};

struct RouteChoices {
    DynamicArray<RouteResult> routes;
    bool searchLimited;
    RouteChoices() : searchLimited(false) {}
};

struct SpanningForest {
    DynamicArray<int> connections;
    long long fare;
    int components;
    SpanningForest() : fare(0), components(0) {}
};

class Graph {
    struct Arc {
        int to, connection;
        bool active;
        Arc(int destination = 0, int id = 0)
            : to(destination), connection(id), active(true) {}
    };

    DynamicArray<DynamicArray<Arc> > adj;
    DynamicArray<Connection> connections;
    DynamicArray<NetworkStatus> stopStatus;

    bool valid(int stop) const { return stop >= 0 && stop < adj.size(); }

    bool usable(const Arc& arc) const {
        return arc.active && connections[arc.connection].status == OPEN && isOpen(arc.to);
    }

    bool openPair(int start, int end) const { return isOpen(start) && isOpen(end); }

    void pushLeg(RouteResult& result, int stop, int id) const {
        const Connection& edge = connections[id];
        if (!result.connections.isEmpty() &&
            connections[result.connections[result.connections.size() - 1]].line != edge.line)
            ++result.transfers;
        result.connections.push_back(id);
        result.stops.push_back(stop);
        result.time += edge.time;
        result.fare += edge.fare;
    }

    static long long cost(const RouteResult& route, RouteMode mode) {
        if (mode == FASTEST) return route.time;
        if (mode == CHEAPEST) return route.fare;
        return route.transfers;
    }

    RouteResult buildPath(int startState, int endState, const DynamicArray<int>& parent,
                          const DynamicArray<int>& via, long long total, int statesPerStop = 1) const {
        Stack<int> path;
        for (int state = endState; state != startState; state = parent[state])
            path.push(state);
        RouteResult result;
        result.stops.push_back(startState / statesPerStop);
        while (!path.isEmpty()) {
            const int state = path.pop();
            pushLeg(result, state / statesPerStop, via[state]);
        }
        result.totalCost = total;
        result.found = true;
        return result;
    }

    template <typename PriorityQueue>
    RouteResult weightedRoute(int start, int end, RouteMode mode) const {
        if (!openPair(start, end)) return RouteResult();
        DynamicArray<long long> distance;
        DynamicArray<int> parent, via;
        for (int i = 0; i < stopCount(); ++i) {
            distance.push_back(LLONG_MAX);
            parent.push_back(-1);
            via.push_back(-1);
        }

        PriorityQueue pending;
        distance[start] = 0;
        pending.insert(start, 0);
        while (!pending.isEmpty()) {
            HeapItem item = pending.extractMin();
            const int stop = item.node;
            if (item.priority != distance[stop]) continue;
            if (stop == end) return buildPath(start, end, parent, via, distance[end]);

            for (int i = 0; i < adj[stop].size(); ++i) {
                const Arc& arc = adj[stop][i];
                if (!usable(arc)) continue;
                const Connection& edge = connections[arc.connection];
                const long long candidate = distance[stop] + (mode == FASTEST ? edge.time : edge.fare);
                if (candidate >= distance[arc.to]) continue;
                distance[arc.to] = candidate;
                parent[arc.to] = stop;
                via[arc.to] = arc.connection;
                pending.insert(arc.to, candidate);
            }
        }
        return RouteResult();
    }

    static bool sameRoute(const RouteResult& a, const RouteResult& b) {
        if (a.connections.size() != b.connections.size()) return false;
        for (int i = 0; i < a.connections.size(); ++i)
            if (a.connections[i] != b.connections[i]) return false;
        return true;
    }

    void searchAlternatives(int stop, int end, RouteMode mode, const RouteResult& primary,
                            DynamicArray<bool>& visited, RouteResult& path,
                            RouteChoices& result, int& budget) const {
        if (budget == 0 || result.routes.size() == 256 || path.stops.size() > 128) {
            result.searchLimited = true;
            return;
        }
        --budget;
        if (stop == end) {
            if (!sameRoute(path, primary)) {
                path.totalCost = cost(path, mode);
                result.routes.push_back(path);
            }
            return;
        }

        for (int i = 0; i < adj[stop].size() && budget > 0 && result.routes.size() < 256; ++i) {
            const Arc& arc = adj[stop][i];
            if (!usable(arc) || visited[arc.to]) continue;
            const int previousTransfers = path.transfers;
            pushLeg(path, arc.to, arc.connection);
            visited[arc.to] = true;
            searchAlternatives(arc.to, end, mode, primary, visited, path, result, budget);
            visited[arc.to] = false;
            path.stops.pop_back();
            path.connections.pop_back();
            path.time -= connections[arc.connection].time;
            path.fare -= connections[arc.connection].fare;
            path.transfers = previousTransfers;
        }
    }

    static void mergeSort(DynamicArray<RouteResult>& routes, DynamicArray<RouteResult>& buffer,
                          int first, int last) {
        if (last - first < 2) return;
        const int middle = first + (last - first) / 2;
        mergeSort(routes, buffer, first, middle);
        mergeSort(routes, buffer, middle, last);
        int left = first, right = middle;
        for (int i = first; i < last; ++i) {
            if (right == last || (left < middle && routes[left].totalCost <= routes[right].totalCost))
                buffer[i] = routes[left++];
            else
                buffer[i] = routes[right++];
        }
        for (int i = first; i < last; ++i) routes[i] = buffer[i];
    }

    void addForestFrontier(int stop, const DynamicArray<bool>& visited, MinHeap& frontier) const {
        for (int i = 0; i < adj[stop].size(); ++i) {
            const Arc& arc = adj[stop][i];
            const Connection& edge = connections[arc.connection];
            if (edge.twoWay && usable(arc) && !visited[arc.to])
                frontier.insert(arc.connection, edge.fare);
        }
    }

public:
    int addStop() {
        adj.push_back(DynamicArray<Arc>());
        stopStatus.push_back(OPEN);
        return adj.size() - 1;
    }

    int stopCount() const { return adj.size(); }
    int connectionCount() const { return connections.size(); }
    const Connection& connection(int id) const { return connections[id]; }
    NetworkStatus status(int stop) const { return stopStatus[stop]; }
    bool isOpen(int stop) const { return valid(stop) && status(stop) == OPEN; }
    void setStopStatus(int stop, NetworkStatus value) { stopStatus[stop] = value; }
    void setConnectionStatus(int id, NetworkStatus value) { connections[id].status = value; }

    int addConnection(int from, int to, int time, int fare, int line, bool twoWay) {
        if (!valid(from) || !valid(to) || time < 0 || fare < 0)
            throw out_of_range("Invalid edge");
        const int id = connections.size();
        connections.push_back(Connection(from, to, time, fare, line, twoWay));
        adj[from].push_back(Arc(to, id));
        if (twoWay && from != to) adj[to].push_back(Arc(from, id));
        return id;
    }

    void addEdge(int from, int to, int time, int fare, int line) {
        addConnection(from, to, time, fare, line, false);
    }

    void addUndirectedEdge(int a, int b, int time, int fare, int line) {
        addConnection(a, b, time, fare, line, true);
    }

    void removeEdge(int from, int to) {
        if (!valid(from)) return;
        for (int i = 0; i < adj[from].size(); ++i)
            if (adj[from][i].to == to) adj[from][i].active = false;
    }

    RouteResult dijkstra(int start, int end, RouteMode mode) const {
        if (mode == FEWEST_TRANSFERS) return fewestTransfers(start, end);
        if (mode == CHEAPEST) return weightedRoute<LinkedPriorityQueue>(start, end, mode);
        return weightedRoute<MinHeap>(start, end, mode);
    }

    RouteResult bfsFewestStops(int start, int end) const {
        if (!openPair(start, end)) return RouteResult();
        DynamicArray<int> parent, via, distance;
        for (int i = 0; i < stopCount(); ++i) {
            parent.push_back(-1);
            via.push_back(-1);
            distance.push_back(-1);
        }
        Queue<int> pending;
        distance[start] = 0;
        pending.enqueue(start);
        while (!pending.isEmpty()) {
            const int stop = pending.dequeue();
            if (stop == end) return buildPath(start, end, parent, via, distance[end]);
            for (int i = 0; i < adj[stop].size(); ++i) {
                const Arc& arc = adj[stop][i];
                if (!usable(arc) || distance[arc.to] != -1) continue;
                distance[arc.to] = distance[stop] + 1;
                parent[arc.to] = stop;
                via[arc.to] = arc.connection;
                pending.enqueue(arc.to);
            }
        }
        return RouteResult();
    }

    RouteResult fewestTransfers(int start, int end) const {
        if (!openPair(start, end)) return RouteResult();

        // Convert arbitrary line IDs to compact indexes. Index zero means no line boarded.
        DynamicArray<int> lines, lineIndex;
        lines.push_back(0);
        for (int i = 0; i < connectionCount(); ++i) {
            const int line = connection(i).line;
            int index = 1;
            while (index < lines.size() && lines[index] != line) ++index;
            if (index == lines.size()) lines.push_back(line);
            lineIndex.push_back(index);
        }

        const int statesPerStop = lines.size();
        if (stopCount() > INT_MAX / statesPerStop)
            throw length_error("Too many stop and line combinations");
        DynamicArray<int> distance, parent, via;
        DynamicArray<bool> settled;
        for (int i = 0; i < stopCount() * statesPerStop; ++i) {
            distance.push_back(INT_MAX);
            parent.push_back(-1);
            via.push_back(-1);
            settled.push_back(false);
        }
        DoublyLinkedList<int> pending;
        const int startState = start * statesPerStop;
        distance[startState] = 0;
        pending.push_front(startState);

        while (!pending.isEmpty()) {
            const int state = pending.pop_front();
            if (settled[state]) continue;
            settled[state] = true;
            const int stop = state / statesPerStop;
            const int currentLine = state % statesPerStop;
            if (stop == end)
                return buildPath(startState, state, parent, via, distance[state], statesPerStop);

            for (int i = 0; i < adj[stop].size(); ++i) {
                const Arc& arc = adj[stop][i];
                if (!usable(arc)) continue;
                const int nextLine = lineIndex[arc.connection];
                const int next = arc.to * statesPerStop + nextLine;
                const int extra = currentLine != 0 && currentLine != nextLine;
                if (distance[state] + extra >= distance[next]) continue;

                distance[next] = distance[state] + extra;
                parent[next] = state;
                via[next] = arc.connection;
                if (extra == 0) pending.push_front(next);
                else pending.push_back(next);
            }
        }
        return RouteResult();
    }

    RouteChoices alternatives(int start, int end, RouteMode mode, const RouteResult& primary) const {
        RouteChoices result;
        if (!openPair(start, end) || !primary.found) return result;
        DynamicArray<bool> visited;
        for (int i = 0; i < stopCount(); ++i) visited.push_back(false);
        visited[start] = true;
        RouteResult path;
        path.stops.push_back(start);
        path.found = true;
        int budget = 20000;
        searchAlternatives(start, end, mode, primary, visited, path, result, budget);
        if (budget == 0 || result.routes.size() == 256) result.searchLimited = true;

        DynamicArray<RouteResult> buffer(result.routes);
        mergeSort(result.routes, buffer, 0, result.routes.size());
        return result;
    }

    SpanningForest minimumSpanningForest() const {
        SpanningForest result;
        DynamicArray<bool> visited;
        for (int i = 0; i < stopCount(); ++i) visited.push_back(false);
        for (int root = 0; root < stopCount(); ++root) {
            if (!isOpen(root) || visited[root]) continue;
            ++result.components;
            MinHeap frontier;
            visited[root] = true;
            addForestFrontier(root, visited, frontier);
            while (!frontier.isEmpty()) {
                const int id = frontier.extractMin().node;
                const Connection& edge = connections[id];
                const int next = visited[edge.from] ? edge.to : edge.from;
                if (visited[next]) continue;
                visited[next] = true;
                result.connections.push_back(id);
                result.fare += edge.fare;
                addForestFrontier(next, visited, frontier);
            }
        }
        return result;
    }

    DynamicArray<int> lineLoop(int start, int line) const {
        DynamicArray<int> path;
        if (!isOpen(start)) return path;

        int stop = start, incoming = -1;
        do {
            path.push_back(stop);
            int degree = 0, nextStop = -1, nextEdge = -1;
            for (int i = 0; i < adj[stop].size(); ++i) {
                const Arc& arc = adj[stop][i];
                const Connection& edge = connections[arc.connection];
                if (!usable(arc) || !edge.twoWay || edge.line != line || arc.to == stop) continue;
                ++degree;
                if (arc.connection == incoming) continue;
                nextStop = arc.to;
                nextEdge = arc.connection;
            }
            if (degree != 2 || path.size() > stopCount()) {
                path.clear();
                return path;
            }
            stop = nextStop;
            incoming = nextEdge;
        } while (stop != start);
        if (path.size() < 3) path.clear();
        return path;
    }
};

#endif
