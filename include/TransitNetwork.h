#ifndef TRANSIT_NETWORK_H
#define TRANSIT_NETWORK_H

#include "AVLTree.h"
#include "Graph.h"
#include "HashMap.h"
#include "KMP.h"
#include "Trie.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

class TransitNetwork {
    struct Change {
        bool isStop;
        int id;
        NetworkStatus previous;
        Change(bool stop = false, int target = 0, NetworkStatus old = OPEN)
            : isStop(stop), id(target), previous(old) {}
    };

    string dataDir;
    DynamicArray<string> names;
    HashMap ids;
    Trie prefixes;
    AVLTree sorted;
    Stack<Change> undoStack;

    static bool readRow(istream& input, string& row) {
        if (!getline(input, row)) return false;
        if (!row.empty() && row.back() == '\r') row.pop_back();
        return true;
    }

    static bool validName(const string& name) {
        return name.find_first_not_of(" \t\r\n") != string::npos &&
               name.find_first_of("\r\n\t") == string::npos;
    }

    void indexStop(int id) {
        ids.put(lowercase(names[id]), id);
        prefixes.insert(lowercase(names[id]));
        sorted.insert(names[id]);
    }

    void rebuildIndexes() {
        ids = HashMap();
        prefixes.clear();
        sorted.clear();
        for (int i = 0; i < graph.stopCount(); ++i)
            if (graph.status(i) != REMOVED) indexStop(i);
    }

    int appendStop(const string& name) {
        const int id = graph.addStop();
        names.push_back(name);
        return id;
    }

    static bool parseStatus(int value) { return value >= OPEN && value <= REMOVED; }

    bool readConnection(const string& row, bool saved) {
        istringstream input(row);
        int a, b, time, fare, line, state = OPEN;
        string extra;
        if (!(input >> a >> b >> time >> fare >> line) ||
            (saved && (!(input >> state) || !parseStatus(state))) || input >> extra)
            return false;
        if (a < 0 || a >= graph.stopCount() || b < 0 || b >= graph.stopCount() ||
            time < 0 || fare < 0) return false;
        const int id = graph.addConnection(a, b, time, fare, line, true);
        graph.setConnectionStatus(id, static_cast<NetworkStatus>(state));
        return true;
    }

    bool readSaved(istream& input) {
        string row;
        if (!readRow(input, row) || row != "KARACHI_TRANSIT_V1") return false;
        bool readingStops = true;
        while (readRow(input, row)) {
            if (row == "ROUTES" && readingStops) {
                readingStops = false;
            } else if (row == "END" && !readingStops) {
                string extra;
                return !(input >> extra) && input.eof();
            } else if (readingStops) {
                if (row.size() < 3 || row[1] != '\t' || !parseStatus(row[0] - '0'))
                    return false;
                const string name = row.substr(2);
                const NetworkStatus state = static_cast<NetworkStatus>(row[0] - '0');
                int existing;
                if (!validName(name) || (state != REMOVED && findStop(name, existing))) return false;
                const int stop = appendStop(name);
                graph.setStopStatus(stop, state);
                if (state != REMOVED) indexStop(stop);
            } else if (!readConnection(row, true)) {
                return false;
            }
        }
        return false;
    }

public:
    Graph graph;

    explicit TransitNetwork(const string& directory = "data") : dataDir(directory) {}

    bool load(string& error) {
        ifstream saved((dataDir + "/network.txt").c_str());
        if (saved.is_open()) {
            if (readSaved(saved)) return true;
            error = "Invalid saved network in " + dataDir + "/network.txt.";
            return false;
        }
        ifstream stops((dataDir + "/stops.txt").c_str());
        ifstream routes((dataDir + "/routes.txt").c_str());
        if (!stops || !routes) {
            error = "Could not open stops.txt and routes.txt in " + dataDir + ".";
            return false;
        }

        string row;
        int lineNumber = 0;
        while (readRow(stops, row)) {
            ++lineNumber;
            if (row.empty()) continue;
            int existing;
            if (!validName(row) || findStop(row, existing)) {
                error = "Invalid or duplicate stop at " + dataDir + "/stops.txt line " + to_string(lineNumber) + ".";
                return false;
            }
            indexStop(appendStop(row));
        }
        if (graph.stopCount() == 0 || !stops.eof()) {
            error = "Could not read stop data.";
            return false;
        }
        lineNumber = 0;
        while (readRow(routes, row)) {
            ++lineNumber;
            if (row.find_first_not_of(" \t\r") == string::npos) continue;
            if (!readConnection(row, false)) {
                error = "Invalid connection at " + dataDir + "/routes.txt line " + to_string(lineNumber) + ".";
                return false;
            }
        }
        if (routes.eof()) return true;
        error = "Could not read route data.";
        return false;
    }

    bool save() const {
#ifdef _WIN32
        const unsigned long process = GetCurrentProcessId();
#else
        const unsigned long process = static_cast<unsigned long>(getpid());
#endif
        const string temporary = dataDir + "/network-" + to_string(process) + ".tmp";
        const string destination = dataDir + "/network.txt";
        ofstream output(temporary.c_str());
        if (!output) return false;
        output << "KARACHI_TRANSIT_V1\n";
        for (int i = 0; i < graph.stopCount(); ++i)
            output << graph.status(i) << '\t' << names[i] << '\n';
        output << "ROUTES\n";
        for (int i = 0; i < graph.connectionCount(); ++i) {
            const Connection& edge = graph.connection(i);
            output << edge.from << ' ' << edge.to << ' ' << edge.time << ' '
                   << edge.fare << ' ' << edge.line << ' ' << edge.status << '\n';
        }
        output << "END\n";
        output.close();
        if (output) {
#ifdef _WIN32
            if (MoveFileExA(temporary.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING)) return true;
#else
            if (std::rename(temporary.c_str(), destination.c_str()) == 0) return true;
#endif
        }
        std::remove(temporary.c_str());
        return false;
    }

    string savePath() const { return dataDir + "/network.txt"; }
    const string& name(int id) const { return names[id]; }
    bool findStop(const string& name, int& id) const { return ids.get(lowercase(name), id); }
    void printSorted() const { sorted.printSorted(); }

    DynamicArray<string> suggestions(const string& prefix, int limit) const {
        DynamicArray<string> matches = prefixes.suggestions(lowercase(prefix), limit);
        for (int i = 0; i < matches.size(); ++i) {
            int id;
            if (findStop(matches[i], id)) matches[i] = name(id);
        }
        return matches;
    }

    bool addStop(const string& name) {
        int id;
        if (!validName(name) || findStop(name, id)) return false;
        id = appendStop(name);
        indexStop(id);
        undoStack.push(Change(true, id, REMOVED));
        return true;
    }

    int addConnection(int from, int to, int time, int fare, int line) {
        const int id = graph.addConnection(from, to, time, fare, line, true);
        undoStack.push(Change(false, id, REMOVED));
        return id;
    }

    bool changeStop(int id, NetworkStatus status) {
        const NetworkStatus previous = graph.status(id);
        if (previous == status || previous == REMOVED) return false;
        undoStack.push(Change(true, id, previous));
        graph.setStopStatus(id, status);
        if (status == REMOVED) rebuildIndexes();
        return true;
    }

    bool changeConnection(int id, NetworkStatus status) {
        const NetworkStatus previous = graph.connection(id).status;
        if (previous == status || previous == REMOVED) return false;
        undoStack.push(Change(false, id, previous));
        graph.setConnectionStatus(id, status);
        return true;
    }

    bool undo() {
        if (undoStack.isEmpty()) return false;
        const Change change = undoStack.pop();
        if (change.isStop) {
            graph.setStopStatus(change.id, change.previous);
            rebuildIndexes();
        } else {
            graph.setConnectionStatus(change.id, change.previous);
        }
        return true;
    }
};

#endif
