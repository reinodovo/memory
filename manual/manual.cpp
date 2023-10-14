#include <Rules.h>
#include <stdio.h>
#include <stdlib.h>

const char* ordinals[4] = {"first", "second", "third", "fourth"};

int main(int argc, char** argv) {
    freopen("manual.md", "w", stdout);
    
    int seed = atoi(argv[1]);
    Rule** rules = generate_rules(seed);

    printf("![Diagram](./manual/diagram.svg){.diagram}\n\n");

    printf("## On the Subject of Memory\n");
    printf("> Memory is a fragile thing but so is everything else when a bomb goes off, so pay attention!\n");
    printf("- Press the correct button to progress the module to the next stage. Complete all stages to disarm the module.\n");
    printf("- Pressing an incorrect button will reset the module back to stage 1.\n");
    printf("- Button positions are ordered from left to right.\n\n");

    for (int stage = 0; stage < STAGES; stage++) {
        printf("_**Stage %d:**_  \n", stage + 1);
        for (int button = 0; button < BUTTONS; button++) {
            printf("If the display is %d, press the button ", button + 1);
            Rule rule = rules[stage][button];
            switch (rule.rule) {
                case POSITION:
                    printf("in the %s position.  \n", ordinals[rule.value]);
                    break;
                case LABEL:
                    printf("labeled %d.  \n", rule.value + 1);
                    break;
                case SAME_POSITION:
                    printf("in the same position you pressed in stage %d.  \n", rule.value + 1);
                    break;
                case SAME_LABEL:
                    printf("with the same label you pressed in stage %d.  \n", rule.value + 1);
                    break;
            }
        }
        printf("  \n");
    }
}