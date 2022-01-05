#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <variant>
#include <stack>
#include <queue>
#include <string>
#include <functional>
#include <algorithm>
#include <memory>

#include "compiler.cpp"
#include "interpreter.cpp"

#define TYPES std::variant<uint8_t, uint16_t, uint32_t, uint64_t,int8_t, int16_t, int32_t, int64_t, float, double, long double>

std::string code = 
"label .main;\n"
"  int8 .count 100;\n"
"  while [$count 0 >=] {\n"
"    $count [$count 1 -];\n"
"    print $count;\n"
"  };\n"
"  print \"count finished.\";";

// debug
void printTree(syntaxNode& node, int depth) {
  std::cout << std::string(depth, '\t') << (int)node.mode << " " << node.value << "\n";
  for (std::unique_ptr<syntaxNode>& child: node.children) {
    printTree(*(child.get()), depth + 1);
  }
}

int main() {
  syntaxNode tree = generateTree(code);
  printTree(tree, 0);
}