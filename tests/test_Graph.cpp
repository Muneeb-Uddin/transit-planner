#include <cassert>
#include <climits>
#include "../include/Graph.h"

int main() {
    Graph graph;
    const int start = graph.addStop();
    const int via = graph.addStop();
    const int first = graph.addStop();
    const int second = graph.addStop();
    const int end = graph.addStop();
    const int isolated = graph.addStop();

    graph.addUndirectedEdge(start, via, 1, 40, 1);
    graph.addUndirectedEdge(via, end, 1, 40, 2);
    const int directLine = graph.addConnection(start, first, 10, 5, 30, true);
    graph.addUndirectedEdge(first, second, 10, 5, 30);
    graph.addUndirectedEdge(second, end, 10, 5, 30);
    graph.addUndirectedEdge(first, end, 10, 30, 99);

    const RouteResult fastest = graph.dijkstra(start, end, FASTEST);
    assert(fastest.found && fastest.time == 2 && fastest.fare == 80 && fastest.transfers == 1);
    const RouteResult cheapest = graph.dijkstra(start, end, CHEAPEST);
    assert(cheapest.found && cheapest.totalCost == 15 && cheapest.time == 30);
    const RouteResult fewest = graph.fewestTransfers(start, end);
    assert(fewest.found && fewest.stops.size() == 4 && fewest.transfers == 0);
    assert(graph.bfsFewestStops(start, end).totalCost == 2);
    assert(!graph.fewestTransfers(start, isolated).found);
    const RouteResult same = graph.fewestTransfers(start, start);
    assert(same.found && same.stops.size() == 1 && same.connections.isEmpty());
    assert(same.time == 0 && same.fare == 0 && same.transfers == 0);

    const RouteChoices choices = graph.alternatives(start, end, FASTEST, fastest);
    assert(!choices.searchLimited && choices.routes.size() == 2);
    assert(choices.routes[0].time == 20 && choices.routes[1].time == 30);

    SpanningForest forest = graph.minimumSpanningForest();
    assert(forest.components == 2 && forest.connections.size() == 4 && forest.fare == 55);

    graph.setConnectionStatus(directLine, BLOCKED);
    assert(graph.fewestTransfers(start, end).transfers == 1);
    assert(graph.fewestTransfers(end, start).transfers == 1);
    graph.setConnectionStatus(directLine, OPEN);
    graph.setStopStatus(first, BLOCKED);
    assert(graph.dijkstra(start, end, CHEAPEST).fare == 80);
    assert(!graph.dijkstra(first, end, FASTEST).found);
    graph.setStopStatus(first, OPEN);
    assert(graph.fewestTransfers(end, start).transfers == 0);

    Graph parallel;
    const int a = parallel.addStop();
    const int b = parallel.addStop();
    const int c = parallel.addStop();
    parallel.addUndirectedEdge(a, b, 1, 1, 0);
    const int continued = parallel.addConnection(a, b, 2, 2, 7, true);
    parallel.addUndirectedEdge(b, c, INT_MAX, INT_MAX, 7);
    const RouteResult route = parallel.fewestTransfers(a, c);
    assert(route.found && route.connections[0] == continued && route.transfers == 0);
    assert(parallel.dijkstra(a, c, FASTEST).totalCost == static_cast<long long>(INT_MAX) + 1);

    parallel.removeEdge(a, b);
    assert(!parallel.dijkstra(a, c, FASTEST).found);
    assert(parallel.dijkstra(c, a, FASTEST).found);

    Graph loop;
    const int x = loop.addStop(), y = loop.addStop(), z = loop.addStop();
    loop.addUndirectedEdge(x, y, 1, 1, 4);
    loop.addUndirectedEdge(y, z, 1, 1, 4);
    loop.addUndirectedEdge(z, x, 1, 1, 4);
    assert(loop.lineLoop(x, 4).size() == 3);
    loop.setConnectionStatus(0, BLOCKED);
    assert(loop.lineLoop(x, 4).isEmpty());
}
