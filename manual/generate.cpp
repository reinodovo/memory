#include <manual.h>
#include <rules.h>
#include <stdio.h>
#include <stdlib.h>

const char *ordinals[4] = {"first", "second", "third", "fourth"};

int main(int argc, char **argv) {
  int code = atoi(argv[1]);
  Rule **rules = generateRules(code);

  std::cout << APP_VERSION << std::endl;

  manual::json data =
      manual::init("Memory", "Memory",
                   "Memory is a fragile thing but so is everything "
                   "else when a bomb goes off, so pay attention!",
                   APP_VERSION);

  data["stages"] = manual::json::array();
  for (int stage = 0; stage < STAGES; stage++) {
    manual::json stage_data;
    stage_data["rules"] = manual::json::array();
    for (int button = 0; button < BUTTONS; button++) {
      manual::json rule_data;
      Rule rule = rules[stage][button];
      switch (rule.rule) {
      case POSITION:
        rule_data["type"] = "position";
        rule_data["value"] = ordinals[rule.value];
        break;
      case LABEL:
        rule_data["type"] = "label";
        rule_data["value"] = rule.value + 1;
        break;
      case SAME_POSITION:
        rule_data["type"] = "same position";
        rule_data["value"] = rule.value + 1;
        break;
      case SAME_LABEL:
        rule_data["type"] = "same label";
        rule_data["value"] = rule.value + 1;
        break;
      }
      stage_data["rules"].push_back(rule_data);
    }
    data["stages"].push_back(stage_data);
  }

  manual::save(data, code);
}