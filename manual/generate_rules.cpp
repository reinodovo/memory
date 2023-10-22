#include <Rules.h>
#include <stdio.h>
#include <stdlib.h>

const char* ordinals[4] = {"first", "second", "third", "fourth"};

int main(int argc, char** argv) {
    freopen("./manual/rules.html", "w", stdout);
    
    int seed = atoi(argv[1]);
    Rule** rules = generate_rules(seed);

    for (int stage = 0; stage < STAGES; stage++) {
        printf("<p><u><strong>Stage %d:</strong></u>\n", stage + 1);
        for (int button = 0; button < BUTTONS; button++) {
            printf("<br>\nIf the display is %d, press the button ", button + 1);
            Rule rule = rules[stage][button];
            switch (rule.rule) {
                case POSITION:
                    printf("in the %s position.", ordinals[rule.value]);
                    break;
                case LABEL:
                    printf("labeled %d.", rule.value + 1);
                    break;
                case SAME_POSITION:
                    printf("in the same position you pressed in stage %d.", rule.value + 1);
                    break;
                case SAME_LABEL:
                    printf("with the same label you pressed in stage %d.", rule.value + 1);
                    break;
            }
        }
        printf("</p>\n");
    }
}