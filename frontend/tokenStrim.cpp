#include "tokenStrim.h"

TokenStream::TokenStream(Lexer *lex)
    : lexer(lex), head(nullptr), tail(nullptr), scan_done(false) {}

TokenStream::~TokenStream() {
  StreamNode *cur = head;
  while (cur) {
    StreamNode *tmp = cur;
    p = p->next;
    delete tmp;
  }
}

TokenStream::TokenStreamIterator TokenStream::begin() {
  if (!head) {
    StreamNode *p = new StreamNode(nullptr, nullptr);
    lexer->next(p->token);
    scan_done = p->token.getKind() == tok::EndOfFile;
    tail = head = p;
  }
  return TokenStreamIterator(this, head);
}

TokenStream::TokenStreamIterator TokenStream::next(StreamNode &cur_pos) {
  if (!cur_pos->next) {
    if (scan_done) {
      return cur_pos;
    }
    StreamNode *p = new StreamNode(nullptr, tail);
    lexer->next(p->token);
    scan_done = p->token.getKind() == tok::EndOfFile;
    tail->next = p;
    tail = p;
    return tail;
  } else {
    return cur_pos->next;
  }
}