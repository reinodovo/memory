#ifndef RULES_H
#define RULES_H

const int STAGES = 5;
const int BUTTONS = 4;

enum Rules {
    POSITION,
    LABEL,
    SAME_POSITION,
    SAME_LABEL,
};

struct Rule {
    Rules rule;
    int value;
    Rule(Rules rule, int value) : rule(rule), value(value) {}
    Rule() {}
};

Rule** generate_rules(int seed);

#endif // RULES_H