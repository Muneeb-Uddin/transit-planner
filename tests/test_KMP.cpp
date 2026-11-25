#include <cassert>
#include "../include/KMP.h"

int main() {
    const KmpPattern stop("nazim");
    assert(stop.matches("North Nazimabad"));
    assert(!stop.matches("Numaish"));

    const KmpPattern overlap("ABABC");
    assert(overlap.matches("abababc"));
    assert(!overlap.matches("abababa"));
    assert(KmpPattern("").matches(""));
    assert(!KmpPattern("x").matches(""));
}
