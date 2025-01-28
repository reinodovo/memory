#include <inja.hpp>
#include <rules.h>
#include <stdio.h>
#include <stdlib.h>

const char *ordinals[4] = {"first", "second", "third", "fourth"};

int main(int argc, char **argv) {
  int seed = atoi(argv[1]);
  Rule **rules = generateRules(seed);

  inja::Environment env;
  env.set_trim_blocks(true);
  env.set_lstrip_blocks(true);
  inja::Template temp = env.parse_template("./manual/template.html");

  nlohmann::json data;
  data["stages"] = nlohmann::json::array();
  for (int stage = 0; stage < STAGES; stage++) {
    nlohmann::json stage_data;
    stage_data["rules"] = nlohmann::json::array();
    for (int button = 0; button < BUTTONS; button++) {
      nlohmann::json rule_data;
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

  std::cout << env.render(temp, data) << std::endl;
}