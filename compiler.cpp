#include "header.hpp"

// Compiler


struct syntaxNode {
  syntaxNode(syntaxNode* initParent) {
    parent = initParent;
  }
  syntaxNode* parent;
  std::vector<std::unique_ptr<syntaxNode>> children;
  std::string value;
  uint8_t mode;
};

bool validBackslash(std::string string) {
  bool valid = false;
  if (!string.empty()) {
    for (uint16_t i = string.length(); i > 0; i--) {
      if (string[i - 1] != '\\') {
        break;
      }
      valid = !valid;
    }
  }
  return valid;
}

syntaxNode generateTree(std::string code) {
  {
    bool spaceFilter = false;
    bool string = false;
    for (uint64_t i = 0; i < code.length(); i++) {
      uint8_t character = code[i];
      if (!string) {
        bool firstCharPreserve = false;
        if (!(character == ' ' || character == '\n') && spaceFilter) {
          spaceFilter = false;
        } else if (character == ';' || character == '{') {
          spaceFilter = true;
          firstCharPreserve = true;
        } 
        
        if (character == '"' && validBackslash(code.substr(0, i))) {
          string = true;
        }

        if ((character == '\n' || spaceFilter) && !firstCharPreserve) { 
          code.erase(i, 1);
          i--;
          firstCharPreserve = false;
        }
      } else if (character == '"' && !validBackslash(code.substr(0, i))) {
        string = false;
      }
    }
  }
  syntaxNode root = syntaxNode(nullptr);
  root.mode = 3;
  syntaxNode* currentParent = &root;

  currentParent->children.push_back(std::make_unique<syntaxNode>(currentParent));
  
  syntaxNode* currentNode;
  {
    syntaxNode* currentLine = currentParent->children[0].get();
    currentLine->mode = 1;
    currentLine->children.push_back(std::make_unique<syntaxNode>(currentLine));
    currentNode = currentLine->children[0].get();
  }

  const char terminator1[] = {
    ' ',
    ';',
    ')',
    ']',
  };

  std::string value;
  std::stack<uint8_t> braces;
  uint8_t ignoreMode = 0;
  for (uint64_t i = 0; i < code.length(); i++) {
    uint8_t character = code[i];
    if (ignoreMode == 0 || (
      ignoreMode == 1 && std::find(
        std::begin(terminator1), 
        std::end(terminator1), 
        character
      ) != std::end(terminator1)) || (
        ignoreMode == 2 && character == '"' && !validBackslash(value)
      )
    ) {
      ignoreMode = 0;
      switch (character) {
        case ';': {
          currentNode->value = value;
          value = "";
          if (i != code.length() - 1 && code[i + 1] != '}') {
            currentParent->children.push_back(std::make_unique<syntaxNode>(currentParent));
            syntaxNode* currentLine = (currentParent->children.back()).get();
            currentLine->mode = 1;
            currentLine->children.push_back(std::make_unique<syntaxNode>(currentLine));
            currentNode = currentLine->children[0].get();
          }  
          break;
        }
        case '(': case '[': {
          currentNode->mode = ((character == '(') ? 1 : 2);
          currentNode->children.push_back(std::make_unique<syntaxNode>(currentNode));
          currentNode = currentNode->children[0].get();
          break;
        }
        case ')': case ']': {
          currentNode->value = value;
          value = "";
          currentNode = currentNode->parent;
          break;
        }
        case '{': {
          currentParent = currentNode;
          currentParent->children.push_back(std::make_unique<syntaxNode>(currentParent));
          currentParent->mode = 3;
          syntaxNode* currentLine = currentParent->children.back().get();
          currentLine->mode = 1;
          currentLine->children.push_back(std::make_unique<syntaxNode>(currentLine));
          currentNode = currentLine->children[0].get();
          break;
        }
        case '}': {
          currentParent = currentParent->parent;
          while (currentParent->mode != 3) {
            currentParent = currentParent->parent; // search for next highest instance of mode 3 in hierarchy 
          }
          currentNode = currentParent->children.back().get();
          break;
        }
        case ' ': {
          currentNode->value = value;
          value = "";
          currentNode->parent->children.push_back(std::make_unique<syntaxNode>(currentNode->parent));
          currentNode = currentNode->parent->children.back().get();
          break;
        }
        case '"': {
          if (value.empty()) {
            ignoreMode = 2;
            currentNode->mode = 4;
          }
          break; 
        }
        default: {
          ignoreMode = 1;
          value.push_back(character);
          currentNode->mode = 0;
          break;
        }
      }
    } else {
      if (validBackslash(value)) {
        switch (character) {
          case '"': {
            value.push_back('"');
            character = 0;
            break;
          } 
          case '\\': {
            value.push_back('\\');
            break;
          }
          case 'n': {
            value.push_back('\n');
            break;
          }
          case 't': {
            value.push_back('\t');
            break;
          }
        }
      } else {
        if (character != '"') {
          value.push_back(character);
        }
      }
    }
  }
  return root;
}

void compile(std::string code, std::vector<uint8_t>& bytecode) {
  std::vector<TYPES> parameters;
  syntaxNode tree = generateTree(code);

  std::string typeNum[11] = { 
    "uint8",
    "uint16",
    "uint32",
    "uint64",
    "int8",
    "int16",
    "int32",
    "int64",
    "float",
    "double",
    "ldouble",
  };

  struct block {
    uint64_t start;
    uint64_t end;
  };

  const std::map<std::string, std::function<void()>> stringInstruction = {
    {"set", [](){

    }}
  };
  std::stack<TYPES> count;
  std::stack<uint8_t> braces;
}


