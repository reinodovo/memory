#include <Rules.h>
#include <stdlib.h>
#include <random>

std::mt19937 rng;

long random(long a, long b) {
    return rng() % (b - a + 1) + a;
}

Rule generate_rule(int stage) {
    int rule;
    if (stage == 0)
        rule = random(0, 1);
    else
        rule = random(0, 3);
    if (rule == POSITION) {
        return Rule(POSITION, random(0, 3));
    }
    else if (rule == LABEL) {
        return Rule(LABEL, random(0, 3));
    }
    else if (rule == SAME_POSITION) {
        return Rule(SAME_POSITION, random(0, stage - 1));
    }
    else if (rule == SAME_LABEL) {
        return Rule(SAME_LABEL, random(0, stage - 1));
    }
    return Rule();
}

Rule** generate_rules(int seed) {
    rng = std::mt19937(seed); 
    Rule** rules = new Rule*[STAGES];
    for (int stage = 0; stage < STAGES; stage++) {
        rules[stage] = new Rule[BUTTONS];
        for (int button = 0; button < BUTTONS; button++) {
            rules[stage][button] = generate_rule(stage);
        }
    }
    return rules;
}