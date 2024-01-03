#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SMLoc.h>
#include <llvm/Support/SourceMgr.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

namespace tok {
enum TokenKind : unsigned short {
#define TOK(ID, TEXT) ID,
#include "tokenKind.def"
  // #define TOK(ID, TEXT) ID,
};
} // namespace tok

struct Name {
  const char *ID;
  int kind;
  size_t lenght;
};

class NamesMap {
  bool is_init;
  llvm::StringMap<Name> hash_table;
  Name *addName(llvm::StringRef ID, tok::TokenKind code);

public:
  NamesMap() : is_init(false) {}
  void addKeywords();
  Name *getName(llvm::StringRef ID);
};

class Token {
  friend class Lexer;

  const char *Ptr;
  size_t lenght;
  tok::TokenKind kind;
  union {
    Name *ID;
    char *literal;
  };
};

namespace llvm {
class DiagnosticEngine {};
} // namespace llvm

class Lexer {
  llvm::SourceMgr &SrcMgr;
  llvm::DiagnosticInfo &Diags;

  llvm::StringRef buffer_start;
  const char *cur_pos;
  unsigned cur_buffer = 0;

  static NamesMap ids_map;

  llvm::SMLoc getLoc(const char *pos) const {
    return llvm::SMLoc::getFromPointer(pos);
  }

public:
  Lexer(llvm::SourceMgr &SrcMgr, llvm::DiagnosticInfo &Diags)
      : SrcMgr(SrcMgr), Diags(Diags) {
    cur_buffer = SrcMgr.getMainFileID();
    buffer_start = SrcMgr.getMemoryBuffer(cur_buffer)->getBuffer();
    cur_pos = buffer_start.begin();
    ids_map.addKeywords();
  }

  llvm::DiagnosticInfo &getDiagnostics() const { return Diags; }

  void next(Token &result);
};