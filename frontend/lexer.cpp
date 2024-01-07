#include "lexer.h"

namespace charinfo {

inline bool isWhitespace(const char &ch) {
  switch (ch) {
  case '\n':
  case '\r':
  case ' ':
  case '\t':
  case '\v':
  case '\f':
    return true;
  }
  return false;
}

inline bool isDigit(const char &ch) { return ('0' <= ch && ch <= '9'); }

inline bool isIdentifierHead(const char &ch) {
  return (ch == '_' || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

inline bool isIdentifierBody(const char &Ch) {
  return isIdentifierHead(Ch) || isDigit(Ch);
}

} // namespace charinfo

NamesMap Lexer::ids_map;

Name *NamesMap::getName(llvm::StringRef ID) {
  return addName(ID, tok::Identifier);
}

Name *NamesMap::addName(llvm::StringRef ID, tok::TokenKind code) {
  auto key = hash_table.insert(std::make_pair(ID, Name()));
  if (!key.second) {
    return &key.first->getValue();
  }
  Name *name = &key.first->getValue();
  name->ID = key.first->getKeyData();
  name->kind = code;
  name->lenght = key.first->getKeyLength();
  return name;
}

void Lexer::next(Token &result) {
  result.kind = tok::Invalid;

  const char *p = cur_pos;

  while (result.kind == tok::Invalid) {
    const char *token_start = p;
#define CHECK_ONE(CHR, TOK)                                                    \
  case CHR:                                                                    \
    result.lenght = 1;                                                         \
    result.kind = TOK;                                                         \
    break

#define CHECK_TWO(CH1, CH2, T1, T2)                                            \
  case CH1:                                                                    \
    if (*p == CH2) {                                                           \
      ++p;                                                                     \
      result.lenght = 2;                                                       \
      result.kind = T1;                                                        \
    } else {                                                                   \
      result.lenght = 1;                                                       \
      result.kind = T2;                                                        \
    }                                                                          \
    break

    switch (char ch = *p++) {
    case 0:
      --p;
      result.kind = tok::EndOfFile;
      break;

    case '\n':
    case '\r':
    case ' ':
    case '\t':
    case '\v':
    case '\f':
      while (charinfo::isWhitespace(*p)) {
        p++;
      }
      continue;
      CHECK_ONE('~', tok::Tilda);
      CHECK_ONE('*', tok::Mul);
      CHECK_ONE('%', tok::Mod);
      CHECK_ONE('^', tok::BitXor);
      CHECK_ONE(',', tok::Comma);
      CHECK_ONE('?', tok::Question);
      CHECK_ONE(':', tok::Colon);
      CHECK_ONE(';', tok::Semicolon);
      CHECK_ONE('(', tok::OpenParen);
      CHECK_ONE(')', tok::CloseParen);
      CHECK_ONE('{', tok::BlockStart);
      CHECK_ONE('}', tok::BlockEnd);
      CHECK_TWO('-', '-', tok::MinusMinus, tok::Minus);
      CHECK_TWO('+', '+', tok::PlusPlus, tok::Plus);
      CHECK_TWO('!', '=', tok::NotEqual, tok::Not);
      CHECK_TWO('=', '=', tok::Equal, tok::Assign);
      CHECK_TWO('|', '|', tok::LogOr, tok::BitOr);
      CHECK_TWO('&', '&', tok::LogAnd, tok::BitAnd);

    case '/':
      if (*p == '/') {
        p++;
        while (*p && (*p != '\r' || *p != '\n')) {
          p++;
        }
        break;
      } else if (*p == '*') {
        unsigned level = 1;
        p++;
        while (*p && level) {
          if (*p == '/' && p[1] == '*') {
            p += 2;
            level++;
          } else if (*p == '*' && p[1] == '/' && level) {
            p += 2;
            level--;
          } else {
            p++;
          }
        }
        if (level) {
          // Diags.print("ERROR");
          std::cerr << "BAD COMMENT\n";
        }
        continue;
      } else {
        result.lenght = 1;
        result.kind = tok::Div;
        break;
      }
    case '<':
      if (*p == '=') {
        p++;
        result.lenght = 2;
        result.kind = tok::LessEqual;
        break;
      } else if (*p == '<') {
        p++;
        result.lenght = 2;
        result.kind = tok::LShift;
        break;
      } else {
        result.lenght = 1;
        result.kind = tok::Less;
        break;
      }
    case '>':
      if (*p == '=') {
        p++;
        result.lenght = 2;
        result.kind = tok::GreaterEqual;
        break;
      } else if (*p == '>') {
        p++;
        result.lenght = 2;
        result.kind = tok::RShift;
        break;
      } else {
        result.lenght = 1;
        result.kind = tok::Greater;
        break;
      }

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      while (charinfo::isDigit(*p)) {
        p++;
      }

    case '0':
      result.kind = tok::IntNumber;
      if (*p == '.') {
        const char *firstDigit = p++;
        while (charinfo::isDigit(*p)) {
          p++;
        }
        if (*p == 'e' || *p == 'E') {
          p++;
          if (*p == '-' || *p == '+') {
            p++;
          }
          firstDigit = p;
          while (charinfo::isDigit(*p)) {
            p++;
          }
          if (p == firstDigit) {
            std::cerr << "ERR_FloatingPointNoDigitsInExponent\n";
          }
        }
        result.kind = tok::FloatNumber;
      }
      result.lenght = (int)(p - token_start);
      result.literal = new char[result.lenght + 1];
      memcpy(result.literal, token_start, result.lenght);
      result.literal[result.lenght] = 0;
      break;

    default:
      if (charinfo::isIdentifierHead(ch)) {
        while (charinfo::isIdentifierBody(*p)) {
          p++;
        }
        size_t length = (size_t)(p - token_start);
        Name *name = ids_map.getName(llvm::StringRef(token_start, length));
        result.ID = name;
        result.kind = (tok::TokenKind)name->kind;
        result.lenght = name->lenght;
        break;
      } else {
        std::cerr << "Undifine symbol\n";
        break;
      }
    }
    result.Ptr = token_start;
  }
  cur_pos = p;
}

int main() { return 0; }