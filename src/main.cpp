#include "../include/CircularLinkedList.h"
#include "../include/TransitNetwork.h"
#include <iostream>
#include <sstream>
#ifdef _WIN32
#include <conio.h>
#else
#include <unistd.h>
#endif

namespace {
const char* const menuOptions[] = {
    "Exit", "Fastest route", "Cheapest route", "Fewest transfers",
    "Suggest stop names", "List stops alphabetically", "Search stop names",
    "Ranked alternative routes", "Edit network or closures", "Previous search",
    "Next search", "Undo last network change", "Minimum spanning tree",
    "Save network", "Show a loop line"
};
const int menuSize = sizeof(menuOptions) / sizeof(menuOptions[0]);

bool interactiveConsole() {
#ifdef _WIN32
    DWORD inputMode, outputMode;
    return GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &inputMode) &&
           GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &outputMode);
#else
    return isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
#endif
}

void clearScreen() {
    if (!interactiveConsole()) return;
#ifdef _WIN32
    cout << flush;
    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    if (!GetConsoleScreenBufferInfo(output, &screen)) return;

    const COORD origin = {0, 0};
    const DWORD cells = static_cast<DWORD>(screen.dwSize.X) * screen.dwSize.Y;
    DWORD written;
    FillConsoleOutputCharacterA(output, ' ', cells, origin, &written);
    FillConsoleOutputAttribute(output, screen.wAttributes, cells, origin, &written);
    SetConsoleCursorPosition(output, origin);
#else
    cout << "\033[2J\033[H";
#endif
}

void pauseScreen() {
    if (!interactiveConsole() || !cin.good()) return;
#ifdef _WIN32
    cout << "\nPress any key to continue..." << flush;
    const int key = _getch();
    if (key == 0 || key == 224) _getch();
#else
    cout << "\nPress Enter to continue..." << flush;
    string unused;
    getline(cin, unused);
#endif
}

void printCentered(const string& line, size_t width) {
    if (line.size() < width) cout << string((width - line.size()) / 2, ' ');
    cout << line << '\n';
}

void showHeader(const string& section) {
    clearScreen();
    const size_t width = 72;
    const string border(width, '=');
    cout << '\n' << border << '\n';
    printCentered("KARACHI TRANSIT ROUTE PLANNER", width);
    printCentered("Muneeb Uddin  |  25K-0765  |  BCS-3E", width);
    cout << border << "\n\n" << section << '\n' << string(width, '-') << '\n';
}

const char* statusName(NetworkStatus status) {
    if (status == BLOCKED) return "blocked";
    if (status == REMOVED) return "removed";
    return "open";
}

const char* modeName(RouteMode mode) {
    if (mode == FASTEST) return "Fastest";
    if (mode == CHEAPEST) return "Cheapest";
    return "Fewest transfers";
}

bool readText(const string& prompt, string& value) {
    cout << prompt;
    return static_cast<bool>(getline(cin, value));
}

bool readNumber(const string& prompt, int& value) {
    string row;
    if (!readText(prompt, row)) return false;
    istringstream input(row);
    string extra;
    if (!(input >> value) || input >> extra) {
        cout << "Enter a whole number.\n";
        return false;
    }
    return true;
}

bool readStopName(const string& prompt, const TransitNetwork& network, string& name) {
#ifdef _WIN32
    DWORD inputMode;
    CONSOLE_SCREEN_BUFFER_INFO screen;
    if (GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &inputMode) &&
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &screen)) {
        name.clear();
        size_t previousWidth = 0;
        for (;;) {
            const DynamicArray<string> matches = name.empty()
                ? DynamicArray<string>() : network.suggestions(name, 3);
            string line = prompt + name;
            if (!matches.isEmpty()) {
                line += "  [Tab: " + matches[0];
                for (int i = 1; i < matches.size(); ++i) line += " | " + matches[i];
                line += ']';
            }
            const size_t width = static_cast<size_t>(screen.dwSize.X - 1);
            if (line.size() > width) line.resize(width);
            cout << '\r' << line;
            if (previousWidth > line.size()) cout << string(previousWidth - line.size(), ' ');
            cout << flush;
            previousWidth = line.size();

            const int key = _getch();
            if (key == '\r' || key == 26 || key == 3) {
                cout << '\r' << string(previousWidth, ' ') << '\r' << prompt << name << '\n';
                if (key == '\r') return true;
                cin.setstate(ios::eofbit);
                return false;
            }
            if (key == 0 || key == 224) {
                _getch();
            } else if (key == '\b' && !name.empty()) {
                name.pop_back();
            } else if (key == '\t' && !matches.isEmpty()) {
                name = matches[0];
            } else if (key >= 32 && key <= 126) {
                name += static_cast<char>(key);
            }
        }
    }
#else
    (void)network;
#endif
    return readText(prompt, name);
}

bool readStop(const string& prompt, const TransitNetwork& network, int& stop) {
    string name;
    if (!readStopName(prompt, network, name)) return false;
    if (network.findStop(name, stop)) return true;
    cout << "Stop not found. Use stop search to check the name.\n";
    return false;
}

bool readEndpoints(const TransitNetwork& network, int& from, int& to) {
    return readStop("Starting stop: ", network, from) && readStop("Destination: ", network, to);
}

void printRoute(const TransitNetwork& network, const RouteResult& route) {
    if (!route.found) {
        cout << "No route found. Check whether either stop or a connection is closed.\n";
        return;
    }
    cout << "Route: " << network.name(route.stops[0]);
    for (int i = 0; i < route.connections.size(); ++i) {
        const Connection& edge = network.graph.connection(route.connections[i]);
        cout << " --[Line " << edge.line << "]--> " << network.name(route.stops[i + 1]);
    }
    cout << "\nTime: " << route.time << " min | Fare: Rs " << route.fare
         << " | Transfers: " << route.transfers << '\n';
}

struct Search {
    int from, to;
    RouteMode mode;
};

class SearchHistory {
    DoublyLinkedList<Search> past;
    DoublyLinkedList<Search> future;

public:
    void record(const Search& search) {
        past.push_back(search);
        future.clear();
    }

    bool move(bool forward, Search& search) {
        if (forward) {
            if (future.isEmpty()) return false;
            past.push_back(future.pop_back());
        } else {
            if (past.size() < 2) return false;
            future.push_back(past.pop_back());
        }
        search = past.back();
        return true;
    }
};

void runSearch(const TransitNetwork& network, const Search& search) {
    cout << modeName(search.mode) << ": " << network.name(search.from)
         << " to " << network.name(search.to) << '\n';
    printRoute(network, network.graph.dijkstra(search.from, search.to, search.mode));
}

void showAlternatives(const TransitNetwork& network) {
    int choice, from, to;
    if (!readNumber("Rank by 1. Time, 2. Fare, 3. Transfers: ", choice)) return;
    if (choice < 1 || choice > 3) {
        cout << "Choose a ranking from 1 to 3.\n";
        return;
    }
    if (!readEndpoints(network, from, to)) return;
    const RouteMode mode = static_cast<RouteMode>(choice - 1);
    const RouteResult primary = network.graph.dijkstra(from, to, mode);
    printRoute(network, primary);
    if (!primary.found) return;

    const RouteChoices alternatives = network.graph.alternatives(from, to, mode, primary);
    if (alternatives.routes.isEmpty()) cout << "No alternative found.\n";
    const int shown = alternatives.routes.size() < 5 ? alternatives.routes.size() : 5;
    for (int i = 0; i < shown; ++i) {
        cout << "\nAlternative " << i + 1 << '\n';
        printRoute(network, alternatives.routes[i]);
    }
    if (alternatives.searchLimited)
        cout << "Search limit reached. Alternatives are ranked among the paths found.\n";
}

void searchStops(const TransitNetwork& network) {
    string term;
    if (!readText("Part of the stop name: ", term)) return;
    if (term.empty()) {
        cout << "Enter at least one character.\n";
        return;
    }
    const KmpPattern pattern(term);
    bool found = false;
    for (int i = 0; i < network.graph.stopCount(); ++i) {
        if (network.graph.status(i) != REMOVED && pattern.matches(network.name(i))) {
            cout << network.name(i) << '\n';
            found = true;
        }
    }
    if (!found) cout << "No matching stops.\n";
}

void suggestStops(const TransitNetwork& network) {
    string prefix;
    if (!readText("Beginning of the stop name: ", prefix)) return;
    const DynamicArray<string> matches = network.suggestions(prefix, 10);
    if (matches.isEmpty()) cout << "No suggestions.\n";
    for (int i = 0; i < matches.size(); ++i) cout << matches[i] << '\n';
}

void printConnection(const TransitNetwork& network, int id) {
    const Connection& edge = network.graph.connection(id);
    cout << '#' << id << "  " << network.name(edge.from) << " <-> " << network.name(edge.to)
         << "  |  Line " << edge.line << "  |  " << edge.time << " min, Rs " << edge.fare;
}

void listConnections(const TransitNetwork& network) {
    bool found = false;
    for (int id = 0; id < network.graph.connectionCount(); ++id) {
        const Connection& edge = network.graph.connection(id);
        if (edge.status == REMOVED || network.graph.status(edge.from) == REMOVED ||
            network.graph.status(edge.to) == REMOVED) continue;
        printConnection(network, id);
        cout << "  |  " << statusName(edge.status);
        if (!network.graph.isOpen(edge.from) || !network.graph.isOpen(edge.to))
            cout << "  |  stop closed";
        cout << '\n';
        found = true;
    }
    if (!found) cout << "No connections to show.\n";
}

bool applyNetworkEdit(TransitNetwork& network, int action) {
    if (action == 1) {
        string name;
        if (!readText("New stop name: ", name)) return false;
        if (!network.addStop(name)) {
            cout << "Enter a unique, nonempty stop name without tabs.\n";
            return false;
        }
        cout << "Stop added.\n";
        return true;
    }
    if (action == 2) {
        int from, to, time, fare, line;
        if (!readEndpoints(network, from, to)) return false;
        if (from == to) {
            cout << "Choose two different stops.\n";
            return false;
        }
        if (!readNumber("Travel time in minutes: ", time) ||
            !readNumber("Fare in rupees: ", fare) || !readNumber("Line number: ", line)) return false;
        if (time < 0 || fare < 0 || line < 0) {
            cout << "Time, fare, and line number must be nonnegative.\n";
            return false;
        }
        cout << "Connection #" << network.addConnection(from, to, time, fare, line) << " added.\n";
        return true;
    }
    if (action == 5) {
        listConnections(network);
        return false;
    }
    if (action != 3 && action != 4) {
        cout << "Invalid network option.\n";
        return false;
    }

    int id;
    if (action == 3) {
        if (!readStop("Stop name: ", network, id)) return false;
        cout << "Current status: " << statusName(network.graph.status(id)) << '\n';
    } else {
        listConnections(network);
        if (!readNumber("Connection number: ", id)) return false;
        if (id < 0 || id >= network.graph.connectionCount() ||
            network.graph.connection(id).status == REMOVED) {
            cout << "Connection not found.\n";
            return false;
        }
    }

    int state;
    if (!readNumber("0. Reopen, 1. Block, 2. Remove: ", state)) return false;
    if (state < OPEN || state > REMOVED) {
        cout << "Invalid status.\n";
        return false;
    }
    const NetworkStatus status = static_cast<NetworkStatus>(state);
    const bool changed = action == 3 ? network.changeStop(id, status) : network.changeConnection(id, status);
    cout << (changed ? "Network updated.\n" : "No change needed.\n");
    return changed;
}

bool editNetwork(TransitNetwork& network) {
    cout << "1. Add stop\n2. Add two-way connection\n3. Block, reopen, or remove stop\n"
            "4. Block, reopen, or remove connection\n5. List connections\n0. Return\n";
    int action;
    if (!readNumber("Network option: ", action)) {
        pauseScreen();
        return false;
    }
    if (action == 0) return false;

    const bool changed = applyNetworkEdit(network, action);
    pauseScreen();
    return changed;
}

void showSpanningTree(const TransitNetwork& network) {
    const SpanningForest forest = network.graph.minimumSpanningForest();
    if (forest.components == 0) {
        cout << "No open stops.\n";
        return;
    }
    if (forest.components == 1) cout << "Minimum spanning tree by connection fare\n";
    else cout << "Minimum spanning forest by connection fare (" << forest.components << " components)\n";
    for (int i = 0; i < forest.connections.size(); ++i) {
        printConnection(network, forest.connections[i]);
        cout << '\n';
    }
    cout << "Total connection fare: Rs " << forest.fare << '\n';
}

void showLoop(const TransitNetwork& network) {
    int start, line;
    if (!readStop("Stop on the loop: ", network, start) || !readNumber("Line number: ", line)) return;
    const DynamicArray<int> stops = network.graph.lineLoop(start, line);
    if (stops.isEmpty()) {
        cout << "No simple open loop for that stop and line.\n";
        return;
    }
    CircularLinkedList<int> loop;
    for (int i = 0; i < stops.size(); ++i) loop.push_back(stops[i]);
    for (int i = 0; i <= loop.size(); ++i) {
        if (i != 0) cout << " -> ";
        cout << network.name(loop.front());
        loop.rotate();
    }
    cout << '\n';
}
}

int main(int argc, char* argv[]) {
    if (argc != 1 && (argc != 3 || string(argv[1]) != "--data-dir" || !argv[2][0])) {
        cerr << "Usage: transit_planner [--data-dir folder]\n";
        return 1;
    }
    TransitNetwork network(argc == 3 ? argv[2] : "data");
    string error;
    if (!network.load(error)) {
        cerr << error << '\n';
        return 1;
    }

    SearchHistory history;
    bool unsaved = false;
    bool exitSelected = false;
    for (;;) {
        showHeader("Main menu");
        for (int i = 1; i < menuSize; ++i)
            cout << i << ". " << menuOptions[i] << '\n';
        cout << "0. " << menuOptions[0] << "\n\n";

        int choice;
        if (!readNumber("Choice: ", choice)) {
            if (cin.fail()) break;
            pauseScreen();
            continue;
        }
        if (choice < 0 || choice >= menuSize) {
            cout << "Invalid choice.\n";
            pauseScreen();
            continue;
        }
        showHeader(menuOptions[choice]);

        if (choice == 0) {
            if (unsaved) {
                string answer;
                if (!readText("Save network changes before exiting? (y/n): ", answer)) break;
                answer = lowercase(answer);
                if (answer != "y" && answer != "n") {
                    cout << "Enter y to save or n to exit without saving.\n";
                    pauseScreen();
                    continue;
                }
                if (answer == "y" && !network.save()) {
                    cout << "Save failed. Check that the data folder is writable.\n";
                    pauseScreen();
                    continue;
                }
            }
            exitSelected = true;
            break;
        }
        if (choice >= 1 && choice <= 3) {
            Search search;
            search.mode = static_cast<RouteMode>(choice - 1);
            if (readEndpoints(network, search.from, search.to)) {
                history.record(search);
                runSearch(network, search);
            }
        } else if (choice == 4) {
            suggestStops(network);
        } else if (choice == 5) {
            network.printSorted();
        } else if (choice == 6) {
            searchStops(network);
        } else if (choice == 7) {
            showAlternatives(network);
        } else if (choice == 8) {
            if (editNetwork(network)) unsaved = true;
            continue;
        } else if (choice == 9 || choice == 10) {
            Search search;
            if (history.move(choice == 10, search)) runSearch(network, search);
            else cout << "No search in that direction.\n";
        } else if (choice == 11) {
            if (network.undo()) {
                unsaved = true;
                cout << "Last network change undone.\n";
            } else {
                cout << "No network change to undo.\n";
            }
        } else if (choice == 12) {
            showSpanningTree(network);
        } else if (choice == 13) {
            if (network.save()) {
                unsaved = false;
                cout << "Network saved to " << network.savePath() << ".\n";
            } else {
                cout << "Save failed. Check that the data folder is writable.\n";
            }
        } else if (choice == 14) {
            showLoop(network);
        }
        pauseScreen();
    }

    if (exitSelected) showHeader("Thank you for using the planner");
    cout << "Goodbye.\n";
    if (exitSelected) pauseScreen();
    return 0;
}
