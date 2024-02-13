#include "lexer.h"

class TokenStream {
  struct StreamNode {
    Token token;
    StreamNode *next;
    StreamNode *prev;

    StreamNode(StreamNode *ne, StreamNode *pre) : next(ne), prev(pre) {}
  };

  class TokenStreamIterator {
  public:
    TokenStreamIterator &operator=(TokenStreamIterator &other) {
      if (other != this) {
        cur_pos = other.cur_pos;
        cur_stream = other.cur_stream;
      }
      return *this;
    }
    bool empty() { return ((cur_stream == nullptr) && (cur_pos == nullptr)); }
    llvm::SMLok getLocation() {
      asser(cur_pos);
      cur_pos->token.getLocation();
    }
    bool operator==(tok::TokenKind tok) {
      return (cur_pos->token.getKind() == tok);
    }
    bool operator!=(tok::TokenKind tok) {
      return (cur_pos->token.getKind() != tok);
    }
    const Token &operator*() { return cur_pos->token; }
    const Token *operator->() { return &cur_pos->token; }
    TokenStreamIterator operator++(int) {
      TokenStreamIterator tmp = *this;
      ++(*this);
      return tmp;
    }
    TokenStreamIterator &operator++() {
      cur_pos = cur_stream->next(cur_pos);
      return this;
    }
    TokenStreamIterator operator--(int) {
      TokenStreamIterator tmp = *this;
      --(*this);
      return tmp;
    }
    TokenStreamIterator &operator--() {
      if (cur_pos->prev) {
        cur_pos = cur_pos->prev;
        return *this;
      }
    }
    TokenStreamIterator operator+(int val) {
      TokenStreamIterator tmp = *this;
      while (val--) {
        tmp++;
      }
      return tmp;
    }
    TokenStreamIterator operator-(int val) {
      TokenStreamIterator tmp = *this;
      while (val--) {
        tmp--;
      }
      return tmp;
    }

    friend class TokenStream;

  private:
    TokenStream *cur_stream;
    StreamNode *cur_pos;

    TokenStreamIterator(TokenStream *source = nullptr,
                        StreamNode *pos = nullptr)
        : cur_stream(source), cur_pos(pos) {}
  };
  Lexer *lexer;
  StreamNode *head;
  StreamNode *tail;
  bool scan_done;

  StreamNode *next(StreamNode &cur_pos);

public:
  typedef TokenStreamIterator iterator;
  TokenStream(Lexer *lexer);
  ~TokenStream();
  TokenStreamIterator begin();
};