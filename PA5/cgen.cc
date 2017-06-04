
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"
#include <iterator>
#include <unistd.h>
#include <map>
#include <stack>
#include <algorithm>
extern void emit_string_constant(std::ostream& str, char *s);
extern int cgen_debug;
//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
       arg,
       arg2,
       Bool,
       concat,
       cool_abort,
       copy,
       Int,
       in_int,
       in_string,
       IO,
       length,
       Main,
       main_meth,
       No_class,
       No_type,
       Object,
       out_int,
       out_string,
       prim_slot,
       self,
       SELF_TYPE,
       Str,
       str_field,
       substr,
       type_name,
       val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
  arg         = idtable.add_string("arg");
  arg2        = idtable.add_string("arg2");
  Bool        = idtable.add_string("Bool");
  concat      = idtable.add_string("concat");
  cool_abort  = idtable.add_string("abort");
  copy        = idtable.add_string("copy");
  Int         = idtable.add_string("Int");
  in_int      = idtable.add_string("in_int");
  in_string   = idtable.add_string("in_string");
  IO          = idtable.add_string("IO");
  length      = idtable.add_string("length");
  Main        = idtable.add_string("Main");
  main_meth   = idtable.add_string("main");
//   _no_class is a symbol that can't be the name of any 
//   user-defined class.
  No_class    = idtable.add_string("_no_class");
  No_type     = idtable.add_string("_no_type");
  Object      = idtable.add_string("Object");
  out_int     = idtable.add_string("out_int");
  out_string  = idtable.add_string("out_string");
  prim_slot   = idtable.add_string("_prim_slot");
  self        = idtable.add_string("self");
  SELF_TYPE   = idtable.add_string("SELF_TYPE");
  Str         = idtable.add_string("String");
  str_field   = idtable.add_string("_str_field");
  substr      = idtable.add_string("substr");
  type_name   = idtable.add_string("type_name");
  val         = idtable.add_string("_val");
}

static char *gc_init_names[] =
  { "_NoGC_Init", "_GenGC_Init", "_ScnGC_Init" };
static char *gc_collect_names[] =
  { "_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect" };


//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(std::ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  cgen_Memmgr = GC_GENGC;
  //cgen_Memmgr_Test = GC_TEST;
  cgen_Memmgr_Debug = GC_DEBUG;

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);

  os << "\n# end of generated code\n";
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////
static void emit_load(char *dest_reg, int offset, char *source_reg, std::ostream& s)
{ s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")"<<endl; }

static void emit_store(char *source_reg, int offset, char *dest_reg, std::ostream& s)
{ s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")" << endl; }

static void emit_load_imm(char *dest_reg, int val, std::ostream& s)
{ s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, std::ostream& s)
{ s << LA << dest_reg << " " << address << endl; }

static void emit_partial_load_address(char *dest_reg, std::ostream& s)
{ s << LA << dest_reg << " "; }

static void emit_load_bool(char *dest, const BoolConst& b, std::ostream& s)
{
  emit_partial_load_address(dest,s);
  b.code_ref(s);
  s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, std::ostream& s)
{
  emit_partial_load_address(dest,s);
  str->code_ref(s);
  s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, std::ostream& s)
{
  emit_partial_load_address(dest,s);
  i->code_ref(s);
  s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, std::ostream& s)
{ s << MOVE << dest_reg << " " << source_reg << endl; }

static void emit_neg(char *dest, char *src1, std::ostream& s)
{ s << NEG << dest << " " << src1 << endl; }

static void emit_add(char *dest, char *src1, char *src2, std::ostream& s)
{ s << ADD << dest << " " << src1 << " " << src2 << endl; }

static void emit_addu(char *dest, char *src1, char *src2, std::ostream& s)
{ s << ADDU << dest << " " << src1 << " " << src2 << endl; }

static void emit_addiu(char *dest, char *src1, int imm, std::ostream& s)
{ s << ADDIU << dest << " " << src1 << " " << imm << endl; }

static void emit_div(char *dest, char *src1, char *src2, std::ostream& s)
{ s << DIV << dest << " " << src1 << " " << src2 << endl; }

static void emit_mul(char *dest, char *src1, char *src2, std::ostream& s)
{ s << MUL << dest << " " << src1 << " " << src2 << endl; }

static void emit_sub(char *dest, char *src1, char *src2, std::ostream& s)
{ s << SUB << dest << " " << src1 << " " << src2 << endl; }

static void emit_sll(char *dest, char *src1, int num, std::ostream& s)
{ s << SLL << dest << " " << src1 << " " << num << endl; }

static void emit_jalr(char *dest, std::ostream& s)
{ s << JALR << "\t" << dest << endl; }

static void emit_jal(char *address, std::ostream &s)
{ s << JAL << address << endl; }

static void emit_return(std::ostream& s)
{ s << RET << endl; }

static void emit_gc_assign(std::ostream& s)
{ s << JAL << "_GenGC_Assign" << endl; }

static void emit_disptable_ref(Symbol sym, std::ostream& s)
{  s << sym << DISPTAB_SUFFIX; }

static void emit_init_ref(Symbol sym, std::ostream& s)
{ s << sym << CLASSINIT_SUFFIX; }

static void emit_label_ref(int l, std::ostream &s)
{ 
  s << "label" << l; 
}

static void emit_protobj_ref(Symbol sym, std::ostream& s)
{ s << sym << PROTOBJ_SUFFIX; }

static void emit_method_ref(Symbol classname, Symbol methodname, std::ostream& s)
{ s << classname << METHOD_SEP << methodname; }

static void emit_label_def(int l, std::ostream &s)
{
  emit_label_ref(l,s);
  s << ":" << endl;
}

static void emit_beqz(char *source, int label, std::ostream &s)
{
  s << BEQZ << source << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_beq(char *src1, char *src2, int label, std::ostream &s)
{
  s << BEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bne(char *src1, char *src2, int label, std::ostream &s)
{
  s << BNE << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bnez(char *src1, int label, std::ostream &s)
{
  s << BNEZ << src1 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, std::ostream &s)
{
  s << BLEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blt(char *src1, char *src2, int label, std::ostream &s)
{
  s << BLT << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blti(char *src1, int imm, int label, std::ostream &s)
{
  s << BLT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bgti(char *src1, int imm, int label, std::ostream &s)
{
  s << BGT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_branch(int _l, std::ostream& _str)
{
  _str << BRANCH;
  emit_label_ref(_l,_str);
  _str << endl;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char* _reg, std::ostream& _str)
{
  emit_store(_reg, 0, SP, _str);
  emit_addiu(SP, SP, -WORD_SIZE, _str);
}

static void emit_pop(int _words, ostream& _str)
{
  emit_addiu(SP, SP, _words*WORD_SIZE, _str); 
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char* _dest, char* _source, std::ostream& _str)
{ emit_load(_dest, DEFAULT_OBJFIELDS, _source, _str); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, std::ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }

static void emit_test_collector(std::ostream &s)
{
  emit_push(ACC, s);
  emit_move(ACC, SP, s); // stack end
  emit_move(A1, ZERO, s); // allocate nothing
  s << JAL << gc_collect_names[cgen_Memmgr] << endl;
  emit_pop(1, s);
  emit_load(ACC,0,SP,s);
}

static void emit_gc_check(char *source, std::ostream &s)
{
  if (source != (char*)A1) emit_move(A1, source, s);
  s << JAL << "_gc_check" << endl;
}

static void EmitEnterFunctionCall(std::ostream& _str)
{
  emit_addiu(SP, SP, -12, _str); // addiu $sp $sp -12 ($sp = $sp - 12) 
  //save caller frame pointer
  emit_store(FP, 3, SP, _str);   // sw $fp 12($sp)a   (*($sp +12) = $fp)
  //save caller this pointer
  emit_store(SELF, 2, SP, _str); // sw $s0 8($sp)     (*($sp + 8) = $s0)
  //save return address
  emit_store(RA, 1, SP, _str);   // sw $ra 4($sp)     (*($sp + 4) = $ra)
  //update callee frame pointer
  emit_addiu(FP, SP, 4, _str);   // addiu $fp $sp 4   (*($sp + 4) = $fp)
  //save the accumulator into $s0 registry (expected SELF pointer)
  emit_move(SELF, ACC, _str);    // move $s0 $a0      ($s0 <- $a0)
}

static void EmitLeaveFunctionCall(unsigned _arg_count, std::ostream& _str)
{
  //restore the stack pointer
  emit_load(FP, 3, SP, _str);     // lw $fp 12($sp) ($sp=$sp+12)
  //restore the this pointer
  emit_load(SELF, 2, SP, _str);   // lw $s0 8($sp) ($s0=$sp+8)
  //restore the return address
  emit_load(RA, 1, SP, _str);     // lw $ra 4($sp) ($ra=$sp+4)
  //free the stack pointer
  emit_pop(DEFAULT_OBJFIELDS +_arg_count, _str); // addiu $sp $sp 12 ($sp=$sp+12)
  //return
  _str<<RET<<std::endl;           // jr $ra
}

static void EmitDefaultValue(Symbol _type_decl, std::ostream& _str)
{
  if (_type_decl == Str) {
    stringtable.lookup_string("")->code_ref(_str);
  }
  else if (_type_decl == Int) {
    inttable.lookup_string("0")->code_ref(_str);
  }
  else if (_type_decl == Bool) {
    falsebool.code_ref(_str) ;
  }
  else {
    _str<<"0";
  }
}

namespace {
  class LabelGenerator 
  {
    unsigned label_generator_ = 0;
  public:
    unsigned NextLabel() {return label_generator_++;} 
  };
  LabelGenerator label_gen;

  void EvalOperands(Expression _e1, Expression _e2, 
                    std::ostream& _str, EmitInfo _emit_info, bool _create_new) {

    _e1->code(_str, _emit_info);
    emit_push(ACC, _str);
    _emit_info.GetMethodStack().emplace_back(No_type, false); //fake type to keep the stack size

    _e2->code(_str, _emit_info);

    if(_create_new) {
      emit_jal(OBJECT_COPY, _str);
    }

    emit_load(T1, 1, SP, _str);
    emit_pop(1, _str);
    emit_move(T2, ACC, _str);

    emit_fetch_int(T1, T1, _str);
    emit_fetch_int(T2, T2, _str);
  }

  bool IsBasicType (Symbol _type_decl) {
    return (_type_decl == Int || _type_decl == Str || _type_decl == Bool);
  }
} //namespace {

///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(std::ostream& _str)
{ _str << STRCONST_PREFIX << index; }

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(std::ostream& _str, int _stringclasstag)
{
  IntEntryP lensym = inttable.add_int(len);

  // Add -1 eye catcher
  _str << WORD << "-1" << endl;

  code_ref(_str);  _str  << LABEL                                     // label
    << WORD << _stringclasstag << endl                                // tag
    << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len+4)/4) << endl // size
    << WORD ;

  /***** Add dispatch information for class String ******/
  _str << Str << DISPTAB_SUFFIX << endl;              // dispatch table
  _str << WORD;lensym->code_ref(_str);  _str << endl; // string length
  emit_string_constant(_str,str);                     // ascii string
  _str << ALIGN;                                      // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(std::ostream& _str, int _stringclasstag)
{
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(_str,_stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(std::ostream& _str)
{
  _str << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(std::ostream& _str, int _intclasstag)
{
  // Add -1 eye catcher
  _str << WORD << "-1" << std::endl;

  code_ref(_str);  _str << LABEL                            // label
    << WORD << _intclasstag << std::endl                    // class tag
    << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << std::endl // object size
    << WORD; 

  /***** Add dispatch information for class Int ******/
  _str << Int << DISPTAB_SUFFIX << std::endl;// dispatch table
  _str << WORD << str << std::endl;          // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(std::ostream& _str, int _intclasstag)
{
  for (List<IntEntry> *l = tbl; l; l = l->tl()) {
    l->hd()->code_def(_str, _intclasstag);
  }
}

//
// Bools
//
BoolConst::BoolConst(int _i) : val(_i) { assert(_i == 0 || _i == 1); }

void BoolConst::code_ref(std::ostream& _str) const
{
  _str << BOOLCONST_PREFIX << val;
}
  
//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(std::ostream& _str, int _boolclasstag)
{
  // Add -1 eye catcher
  _str << WORD << "-1" << std::endl;

  code_ref(_str);  _str << LABEL                             // label
    << WORD << _boolclasstag << std::endl                    // class tag
    << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << std::endl // object size
    << WORD;

  /***** Add dispatch information for class Bool ******/

  _str << Bool << DISPTAB_SUFFIX << std::endl; // dispatch table
  _str << WORD << val << std::endl;            // value (0 or 1)
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data(ostream& _str)
{
  Symbol main    = idtable.lookup_string(MAINNAME);
  Symbol string  = idtable.lookup_string(STRINGNAME);
  Symbol integer = idtable.lookup_string(INTNAME);
  Symbol boolc   = idtable.lookup_string(BOOLNAME);

  _str << "\t.data\n" << ALIGN;
  //
  // The following global names must be defined first.
  //
  _str << GLOBAL << CLASSNAMETAB << endl;
  _str << GLOBAL; emit_protobj_ref(main, _str); _str << std::endl;
  _str << GLOBAL; emit_protobj_ref(integer, _str); _str << std::endl;
  _str << GLOBAL; emit_protobj_ref(string, _str);  _str << std::endl;
  _str << GLOBAL; falsebool.code_ref(_str);  _str << std::endl;
  _str << GLOBAL; truebool.code_ref(_str);   _str << std::endl;
  _str << GLOBAL << INTTAG << std::endl;
  _str << GLOBAL << BOOLTAG << std::endl;
  _str << GLOBAL << STRINGTAG << std::endl;

  //
  // We also need to know the tag of the Int, String, and Bool classes
  // during code generation.
  //
  _str << INTTAG << LABEL << WORD << intclasstag << std::endl;
  _str << BOOLTAG << LABEL << WORD << boolclasstag << std::endl;
  _str << STRINGTAG << LABEL << WORD << stringclasstag << std::endl;
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text(std::ostream& _str)
{
  _str << GLOBAL << HEAP_START << std::endl
      << HEAP_START << LABEL 
      << WORD << 0 << std::endl
      << "\t.text" << std::endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Main"), _str);
  _str << std::endl << GLOBAL;
  emit_init_ref(idtable.add_string("Int"), _str);
  _str << std::endl << GLOBAL;
  emit_init_ref(idtable.add_string("String"), _str);
  _str << std::endl << GLOBAL;
  emit_init_ref(idtable.add_string("Bool"), _str);
  _str << std::endl << GLOBAL;
  emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), _str);
  _str << std::endl;
}

void CgenClassTable::code_bools(std::ostream& _str, int _boolclasstag)
{
  falsebool.code_def(_str, _boolclasstag);
  truebool.code_def(_str, _boolclasstag);
}

void CgenClassTable::code_select_gc(std::ostream& _str)
{
  //
  // Generate GC choice constants (pointers to GC functions)
  //
  _str << GLOBAL << "_MemMgr_INITIALIZER" << std::endl;
  _str << "_MemMgr_INITIALIZER:" << endl;
  _str << WORD << gc_init_names[cgen_Memmgr] << std::endl;
  _str << GLOBAL << "_MemMgr_COLLECTOR" << std::endl;
  _str << "_MemMgr_COLLECTOR:" << std::endl;
  _str << WORD << gc_collect_names[cgen_Memmgr] << std::endl;
  _str << GLOBAL << "_MemMgr_TEST" << std::endl;
  _str << "_MemMgr_TEST:" << std::endl;
  _str << WORD << (cgen_Memmgr_Test == GC_TEST) << std::endl;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants(std::ostream& _str)
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(_str, stringclasstag);
  inttable.code_string_table(_str, intclasstag);
  code_bools(_str, boolclasstag);
}


CgenClassTable::CgenClassTable(Classes _classes, std::ostream& _str) : nds(nullptr)
{
//sleep(60);
  _classTagCounter = 0;

  enterscope();
  if (cgen_debug) cout << "Building CgenClassTable" << std::endl;
  install_basic_classes();
  install_classes(_classes);
  build_inheritance_tree();

  code(_str);
  exitscope();
}


unsigned CgenClassTable::nextTag()
{
  return _classTagCounter++;
}


void CgenClassTable::install_basic_classes()
{

// The tree package uses these globals to annotate the classes built below.
  //curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

//
// A few special class names are installed in the lookup table but not
// the class list.  Thus, these classes exist, but are not part of the
// inheritance hierarchy.
// No_class serves as the parent of Object and the other special classes.
// SELF_TYPE is the self class; it cannot be redefined or inherited.
// prim_slot is a class known to the code generator.
//
  CgenNodeP node = nullptr;
  node = new CgenNode(class_(No_class,No_class,nil_Features(),filename),
                      Basic, this);
  addid(No_class, node);

  node = new CgenNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
          Basic, this);
  addid(SELF_TYPE, node);

  node = new CgenNode(class_(prim_slot,No_class,nil_Features(),filename),
          Basic, this);
  addid(prim_slot, node);

// 
// The Object class has no parent class. Its methods are
//        cool_abort() : Object    aborts the program
//        type_name() : Str        returns a string representation of class name
//        copy() : SELF_TYPE       returns a copy of the object
//
// There is no need for method bodies in the basic classes---these
// are already built in to the runtime system.
//
  unsigned tag = 0;
  install_class(
   new CgenNode(
    class_(Object, 
     No_class,
     append_Features(
           append_Features(
           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
           single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
     filename),
    Basic, this), nextTag());

// 
// The IO class inherits from Object. Its methods are
//        out_string(Str) : SELF_TYPE          writes a string to the output
//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
//        in_string() : Str                    reads a string from the input
//        in_int() : Int                         "   an int     "  "     "
//
   install_class(
    new CgenNode(
     class_(IO, 
            Object,
            append_Features(
            append_Features(
            append_Features(
            single_Features(method(out_string, single_Formals(formal(arg, Str)),
                        SELF_TYPE, no_expr())),
            single_Features(method(out_int, single_Formals(formal(arg, Int)),
                        SELF_TYPE, no_expr()))),
            single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
            single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
     filename),
    Basic, this), nextTag());

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
   intclasstag = nextTag();
   install_class(
    new CgenNode(
     class_(Int, 
      Object,
            single_Features(attr(val, prim_slot, no_expr())),
      filename),
     Basic, this), intclasstag);

//
// Bool also has only the "val" slot.
//
   boolclasstag = nextTag();
    install_class(
     new CgenNode(
      class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename),
      Basic, this), boolclasstag);

//
// The class Str has a number of slots and operations:
//       val                                  ???
//       str_field                            the string itself
//       length() : Int                       length of the string
//       concat(arg: Str) : Str               string concatenation
//       substr(arg: Int, arg2: Int): Str     substring
//       
   stringclasstag = nextTag();
   install_class(
    new CgenNode(
      class_(Str, 
       Object,
             append_Features(
             append_Features(
             append_Features(
             append_Features(
             single_Features(attr(val, Int, no_expr())),
            single_Features(attr(str_field, prim_slot, no_expr()))),
            single_Features(method(length, nil_Formals(), Int, no_expr()))),
            single_Features(method(concat, 
           single_Formals(formal(arg, Str)),
           Str, 
           no_expr()))),
      single_Features(method(substr, 
           append_Formals(single_Formals(formal(arg, Int)), 
              single_Formals(formal(arg2, Int))),
           Str, 
           no_expr()))),
       filename),
        Basic, this), stringclasstag);
}

// CgenClassTable::install_class
// CgenClssTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_class(CgenNodeP _nd, unsigned _tag)
{
  Symbol name = _nd->get_name();

  if (probe(name)) {
    return;
  }

  // The class name is legal, so add it to the list of classes
  // and the symbol table.
  nds = new List<CgenNode>(_nd,nds);
  addid(name, _nd);
  //add the class name to tag map
  tag_name_map_[_tag] = name;
  name_tag_map_[name] = _tag;
}


void CgenClassTable::install_classes(Classes cs)
{
  for(int i = cs->first(); cs->more(i); i = cs->next(i)) {
    install_class(new CgenNode(cs->nth(i),NotBasic, this), nextTag());
  }
}


//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
  for(List<CgenNode> *l = nds; l; l = l->tl())
      set_relations(l->hd());
}


//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
  CgenNode *parent_node = probe(nd->get_parent());
  nd->set_parentnd(parent_node);
  parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
  children = new List<CgenNode>(n,children);
}

void CgenNode::set_parentnd(CgenNodeP p)
{
  assert(parentnd == NULL);
  assert(p != NULL);
  parentnd = p;
}


void CgenClassTable::code(ostream& str)
{
  if (cgen_debug) cout << "coding global data" << endl;
  code_global_data(str);

  if (cgen_debug) cout << "choosing gc" << endl;
  code_select_gc(str);

  if (cgen_debug) cout << "coding constants" << endl;
  code_constants(str);

//                 Add your code to emit
//                   - prototype objects
//                   - class_nameTab
//                   - dispatch tables
//

  EmitClassesNameTable(str);
  EmitClassesInitializer(str);
  EmitPrototypeObjects(str); //for the prototype object of class <class>
  EmitDispatchTables(str);

//dispatchTables();

  if (cgen_debug) cout << "coding global text" << endl;
  code_global_text(str);

//                 Add your code to emit
//                   - object initializer
//                   - the class methods
//                   - etc...

//initialize();
  EmitObjectInitializers(str);

  EmitClassMethods(str);
}


void CgenClassTable::EmitClassesNameTable(std::ostream& str)
{
  str << CLASSNAMETAB << LABEL;
  for(const std::pair<unsigned, Symbol>& tag_name_pair: tag_name_map_) {
    str<<WORD;
    stringtable.lookup_string(tag_name_pair.second->get_string())->code_ref(str);
    str<<std::endl;
  }
}


void CgenClassTable::EmitClassesInitializer(std::ostream& str)
{
  str<<CLASSOBJTAB<<LABEL;
  EmitClassesInitializer(root(), str);
}

void CgenClassTable::EmitClassesInitializer(CgenNode* node, std::ostream& str)
{
  for(const std::pair<unsigned, Symbol>& tag_name_pair: tag_name_map_) {
    CgenNodeP curr_node = probe(tag_name_pair.second);
    curr_node->EmitClassInitializer(str);
  }
}


void CgenClassTable::EmitDispatchTables(std::ostream& str)
{
  EmitDispatchTable(root(), str);
}


void CgenClassTable::EmitDispatchTable(CgenNode* node, std::ostream& str)
{
  CgenNode* head_node = node;
  head_node->EmitDispatchTable(str);
  List<CgenNode>* children = node->get_children();
  while(children) {
    head_node = children->hd();
    EmitDispatchTable(head_node, str);
    children = children->tl();
  }
}


void CgenClassTable::EmitPrototypeObjects(std::ostream& str)
{
  EmitPrototypeObjects(root(), str);
}


void CgenClassTable::EmitPrototypeObjects(CgenNode* node, std::ostream& str)
{
  CgenNode* head_node = node;
  head_node->EmitPrototypeObject(str);
  List<CgenNode>* children = node->get_children();
  while(children) {
    head_node = children->hd();
    EmitPrototypeObjects(head_node, str);
    children = children->tl();
  }
}


CgenNodeP CgenClassTable::root()
{
   return probe(Object);
}


void CgenClassTable::EmitObjectInitializers(std::ostream& str)
{
  EmitObjectInitializers(root(), str);
}


void CgenClassTable::EmitObjectInitializers(CgenNode* node, std::ostream& str)
{
  CgenNode* head_node = node;
  head_node->EmitObjectInitializer(str, this);
  List<CgenNode>* children = node->get_children();
  while(children) {
    head_node = children->hd();
    EmitObjectInitializers(head_node, str);
    children = children->tl();
  }
}


void CgenClassTable::EmitClassMethods(std::ostream& str)
{
  EmitClassMethods(root(), str);
}


void CgenClassTable::EmitClassMethods(CgenNode* node, std::ostream& _str)
{
  CgenNode* head_node = node;
  head_node->EmitClassMethods(_str, this);
  List<CgenNode>* children = node->get_children();
  while(children) {
    head_node = children->hd();
    EmitClassMethods(head_node, _str);
    children = children->tl();
  }
}

///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP _class_table) :
   class__class((const class__class &) *nd),
   parentnd(NULL),
   children(NULL),
   basic_status(bstatus),
   class_table_(_class_table)
{
   stringtable.add_string(name->get_string());          // Add class name to string table
}


//make the methods list to keep the same ordering of methods in base and derived classes.
std::vector<std::pair<CgenNodeP, method_class*>> CgenNode::GetMethods()
{
  std::vector<std::pair<CgenNodeP, method_class*>> methods;

  std::map<Symbol, size_t> registered_methods_idx;
  std::stack<CgenNodeP> hierarchy_stack;
  CgenNodeP curr_node = this;
  while (curr_node->get_name() != No_class) {
    hierarchy_stack.push(curr_node);
    curr_node = curr_node->get_parentnd();
  }
  while(false == hierarchy_stack.empty()) {
    curr_node = hierarchy_stack.top();
    for (int idx_feature = curr_node->features->first();
         curr_node->features->more(idx_feature);
         idx_feature = curr_node->features->next(idx_feature)) {

      Feature feature = curr_node->features->nth(idx_feature);
      if(!feature->IsAttribute()) {
        method_class* method = static_cast<method_class*>(feature);
        if(registered_methods_idx.find(method->GetName()) == std::cend(registered_methods_idx)) {
          registered_methods_idx[method->GetName()] = (size_t)methods.size();
          methods.emplace_back(curr_node, method);
        } else {
          methods[registered_methods_idx[method->GetName()]] = std::move(std::make_pair(curr_node, method));
        }
      }
    }
    hierarchy_stack.pop();
  }
  return methods;
}


std::vector<std::pair<CgenNodeP, method_class*>> CgenNode::GetClassMethods()
{
  std::vector<std::pair<CgenNodeP, method_class*>> methods;
  CgenNodeP curr_node = this;
  for (int idx_feature = curr_node->features->first();
       curr_node->features->more(idx_feature);
       idx_feature = curr_node->features->next(idx_feature)) {
    Feature feature = curr_node->features->nth(idx_feature);
    if(!feature->IsAttribute()) {
      methods.push_back(std::make_pair(curr_node, static_cast<method_class*>(feature)));
    }
  }
  return methods;
}


std::vector<attr_class*> CgenNode::GetAttributes()
{
  std::vector<attr_class*> attributes;
  CgenNodeP curr_node = this;
  while (curr_node->get_name() != No_class) {
    std::vector<attr_class*> class_attributes;
    for (int idx_feature = curr_node->features->first();
         curr_node->features->more(idx_feature);
         idx_feature = curr_node->features->next(idx_feature)) {
      Feature feature = curr_node->features->nth(idx_feature);
      if(feature->IsAttribute()) {
        class_attributes.push_back(static_cast<attr_class*>(feature));
      }
    }
    attributes.insert(std::begin(attributes), std::begin(class_attributes), std::end(class_attributes));
    curr_node = curr_node->get_parentnd();
  }
  return attributes;
}


std::vector<attr_class*> CgenNode::GetClassAttributes()
{
  std::vector<attr_class*> attributes;
  CgenNodeP curr_node = this;
  if (curr_node->get_name() != No_class) {
    for (int idx_feature = curr_node->features->first();
         curr_node->features->more(idx_feature);
         idx_feature = curr_node->features->next(idx_feature)) {
      Feature feature = curr_node->features->nth(idx_feature);
      if(feature->IsAttribute()) {
        attributes.push_back(static_cast<attr_class*>(feature));
      }
    }
  }

  return attributes;
}


void CgenNode::EmitClassMethods(std::ostream& _str, CgenClassTableP _class_table)
{
  //basic types are already generated
  if(!basic()) {
    std::vector<std::pair<CgenNodeP, method_class*>> methods = GetClassMethods();
    for(std::pair<CgenNodeP, method_class*> method : methods) {
      EmitInfo emit_info(method.first, _class_table);
      method.second->EmitCode(emit_info, _str);
    }
  }
}


void CgenNode::EmitPrototypeObject(std::ostream& str)
{
  std::vector<attr_class*> attributes = GetAttributes();
  str<<WORD<<-1<<std::endl;                                       // Off -4 |.word -1
  str<<get_name()->get_string()<<PROTOBJ_SUFFIX<<LABEL;           //        |<classname>_protObj:
  str<<WORD<<GetClassTable()->GetClassTag(get_name())<<std::endl; // Off  0 |.word class_tag
  str<<WORD<< DEFAULT_OBJFIELDS + attributes.size() <<std::endl;  // Off  4 |.word object_size
  str<<WORD<<get_name()->get_string()<<DISPTAB_SUFFIX<<std::endl; //Off 8: Dispatch pointer
  //Off 12. . . Attributes with default values
  for(attr_class* attribute : attributes) {
    str<<WORD; 
    EmitDefaultValue(attribute->GetTypeDecl(), str);
    str<<std::endl;
  }
}


void CgenNode::EmitClassInitializer(std::ostream& str)
{
  str<<WORD<<get_name()->get_string()<<PROTOBJ_SUFFIX<<std::endl;
  str<<WORD<<get_name()->get_string()<<CLASSINIT_SUFFIX<<std::endl;
}


void CgenNode::EmitDispatchTable(std::ostream& str)
{
  str<<get_name()->get_string()<<DISPTAB_SUFFIX<<LABEL;

  std::vector<std::pair<CgenNodeP, method_class*>> all_vtable = GetMethods();

  for(auto class_method_pair : all_vtable) {
    str<<WORD <<class_method_pair.first->get_name()->get_string()<<METHOD_SEP<<class_method_pair.second->GetName()->get_string()<<std::endl;
  }
}


void CgenNode::EmitObjectInitializer(std::ostream& _str, CgenClassTableP _class_table)
{
  _str<<get_name()->get_string()<<CLASSINIT_SUFFIX<<LABEL; //<className>_init:

  std::vector<attr_class*> attributes = GetAttributes();
  std::vector<attr_class*> local_attributes = GetClassAttributes();

  size_t attributes_count = attributes.size();
  size_t local_attributes_count = local_attributes.size();
  assert(attributes_count >= local_attributes_count);
  size_t offset = (attributes_count - local_attributes_count);

  EmitEnterFunctionCall(_str);

  if (get_name() != Object) {
    std::stringstream ss_parent;
    ss_parent<<get_parentnd()->get_name()->get_string()<<CLASSINIT_SUFFIX;
    emit_jal(const_cast<char*>(ss_parent.str().c_str()), _str); //jal <parentClass>_init
  }

  //init attributes
  Expression init;
  for(attr_class* attribute : local_attributes) {
    init = attribute->GetInit();
    if(!init->CheckNoExpression()) {
      EmitInfo emit_info(this, _class_table);
      init->code(_str, emit_info);
      emit_store(ACC, offset + DEFAULT_OBJFIELDS, SELF, _str);
      if (cgen_Memmgr == GC_GENGC) {
        emit_addiu(A1, SELF, WORD_SIZE * (offset + DEFAULT_OBJFIELDS), _str);
        emit_gc_assign(_str);
      }
    } else {
      if(IsBasicType(attribute->GetTypeDecl())) {
        //load into $a0 the address of default value
        emit_partial_load_address(ACC, _str); //la  $a0 ...
        EmitDefaultValue(attribute->GetTypeDecl(), _str);
        _str<<std::endl;
        //store the address stored in $a0 into the attribute at the right offset
        emit_store(ACC, DEFAULT_OBJFIELDS + offset, SELF, _str);
      }
    }
    ++offset;
  }

  //restore the SELF pointer into accumulator (result)
  emit_move(ACC, SELF, _str);      //move $a0 $s0 ($a0=$s0)

  EmitLeaveFunctionCall(0, _str);
}


//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************
/*virtual*/
void method_class::EmitCode(EmitInfo _emit_info, std::ostream& _str)
{
  emit_method_ref(_emit_info.GetCurrentClass()->get_name(), name, _str);
  _str<<LABEL;
  std::stack<Symbol> local_stack;
  for (int idx = formals->first(); formals->more(idx); idx = formals->next(idx)) {
    local_stack.push(formals->nth(idx)->GetName());
  }
  while(!local_stack.empty()) {
    _emit_info.GetMethodStack().emplace_back(local_stack.top(), false);
    local_stack.pop();
  }
  EmitEnterFunctionCall(_str);
  expr->code(_str, _emit_info);
  EmitLeaveFunctionCall(formals->len(),_str);
  assert((size_t)formals->len() == _emit_info.GetMethodStack().size());
  for (int idx = formals->first(); formals->more(idx); idx = formals->next(idx)) {
    _emit_info.GetMethodStack().pop_back();
  }
}


void assign_class::code(std::ostream &_str, EmitInfo _emit_info) 
{
  expr->code(_str, _emit_info); //righthand side of the assignment expression eval and result in ACC

  //make a method common with object_class
  size_t offset = _emit_info.GetMethodStack().size();
  bool is_on_stack = false;
  while(offset > 0) {
    --offset;
    if(_emit_info.GetMethodStack()[offset].GetName() == name) {
      is_on_stack = true;
      break;
    }
  }

  if(is_on_stack) {
    if(_emit_info.GetMethodStack()[offset].IsLocal()) {
      emit_store(ACC, _emit_info.GetMethodStack().size() - offset, SP, _str);

      if (cgen_Memmgr == GC_GENGC) {
        emit_addiu(A1, SP, WORD_SIZE * (_emit_info.GetMethodStack().size() - offset), _str);
        emit_gc_assign(_str);
      }
    } else {
      emit_store(ACC, offset  + 3, FP, _str);
      if (cgen_Memmgr == GC_GENGC) {
        emit_addiu(A1, FP, WORD_SIZE * (offset  + 3), _str);
        emit_gc_assign(_str);
      }
    }
  } else {

      bool is_member_variable = false;
      std::vector<attr_class*> attributes = _emit_info.GetCurrentClass()->GetAttributes();
      offset = 0;

      while(offset < attributes.size()) {
        if(attributes[offset]->GetName() == name) {
          is_member_variable = true;
          break;
        }
        ++offset;
      }
      assert(is_member_variable); 
      emit_store(ACC, offset + DEFAULT_OBJFIELDS, SELF, _str);
      if (cgen_Memmgr == GC_GENGC) {
        emit_addiu(A1, SELF, WORD_SIZE * (offset  + DEFAULT_OBJFIELDS), _str);
        emit_gc_assign(_str);
      }
  }
}


void static_dispatch_class::code(std::ostream &_str, EmitInfo _emit_info) 
{
  //put arguments on the stack
  int idx_param;
  for (idx_param = actual->first(); actual->more(idx_param); idx_param = actual->next(idx_param)) {
    //evaluate param into accumulator
    actual->nth(idx_param)->code(_str, _emit_info);
    //push the accumulator into the stack
    emit_push(ACC, _str); // sw $a0 0($sp)     ($a0 <- $sp); $sp<-$sp-4
    _emit_info.GetMethodStack().emplace_back(No_type, false); //fake type to keep the stack size
  }
  expr->code(_str, _emit_info); //Puts in ACC the dispatch table address evaluated by expr

  //check the $a0 to be not null
  unsigned label_id = label_gen.NextLabel();
  emit_bnez(ACC, label_id, _str); //bnez $a0 labelnum (branch to target if $a0 != $zero

  //report error
  //load filename into ACC 
  emit_partial_load_address(ACC, _str);
  stringtable.lookup_string(_emit_info.GetCurrentClass()->get_filename()->get_string())->code_ref(_str);
  _str<<std::endl;
  emit_load_imm(T1, curr_lineno, _str);
  emit_jal(DISPATCH_ABORT, _str); //defined in trap.handler

  emit_label_def(label_id, _str);


  //get the dispatch table
  std::string dispatch_table = type_name->get_string() + std::string(DISPTAB_SUFFIX);
  emit_load_address(T1, (char *)dispatch_table.c_str(), _str);
  //emit_load(T1, DISPTABLE_OFFSET, ACC, _str); // $t1 <- 2($a0)

  CgenNodeP current_class = _emit_info.GetClassTable()->lookup(type_name);

  std::vector<std::pair<CgenNodeP, method_class*>> all_methods = current_class->GetMethods();
    size_t method_offset = all_methods.size();
    while(method_offset > 0) {
      if(all_methods[--method_offset].second->GetName() == name) {
        break;
      }
    }

  emit_load(T1, method_offset, T1, _str); // lw $t1 method_offset($t1) ($t1=MEM($t1+method_offset))
  emit_jalr(T1, _str); //jump to method

}


void dispatch_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  //put arguments on the stack
  int idx_param;
  for (idx_param = actual->first(); actual->more(idx_param); idx_param = actual->next(idx_param)) {
    //evaluate param into accumulator
    actual->nth(idx_param)->code(_str, _emit_info);
    //push the accumulator into the stack
    emit_push(ACC, _str); // sw $a0 0($sp)     ($a0 <- $sp); $sp<-$sp-4
    _emit_info.GetMethodStack().emplace_back(No_type, false); //fake type to keep the stack size
  }
  expr->code(_str, _emit_info); //Puts in ACC the dispatch table address evaluated by expr

  //check the $a0 to be not null
  unsigned label_id = label_gen.NextLabel();
  emit_bnez(ACC, label_id, _str); //bnez $a0 labelnum (branch to target if $a0 != $zero

  //report error
  //load filename into ACC 
  emit_partial_load_address(ACC, _str);
  stringtable.lookup_string(_emit_info.GetCurrentClass()->get_filename()->get_string())->code_ref(_str);
  _str<<std::endl;
  emit_load_imm(T1, curr_lineno, _str);
  emit_jal(DISPATCH_ABORT, _str); //defined in trap.handler

  emit_label_def(label_id, _str);

  //get the dispatch table
  emit_load(T1, DISPTABLE_OFFSET, ACC, _str); // $t1 <- 2($a0)

  CgenNodeP current_class = _emit_info.GetCurrentClass();
  if (expr->get_type() != SELF_TYPE) {
    current_class = _emit_info.GetClassTable()->lookup(expr->get_type());
  }

  std::vector<std::pair<CgenNodeP, method_class*>> all_methods = current_class->GetMethods();
    size_t method_offset = all_methods.size();
    while(method_offset > 0) {
      if(all_methods[--method_offset].second->GetName() == name) {
        break;
      }
    }

  emit_load(T1, method_offset, T1, _str); // lw $t1 method_offset($t1) ($t1=MEM($t1+method_offset))
  emit_jalr(T1, _str); //jump to method
}


void cond_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  pred->code(_str, _emit_info); //evaluate predicate in $a0

  int label_false = label_gen.NextLabel();
  int label_end = label_gen.NextLabel();

  emit_fetch_int(T1, ACC, _str); //load the actual int member of bool class into T1
  emit_beqz(T1, label_false, _str); //if(predicate != 0) jump to false
  then_exp->code(_str, _emit_info); //eval then
  emit_branch(label_end, _str); //jump to end of else statement
  emit_label_def(label_false, _str); //create label
  else_exp->code(_str, _emit_info);
  emit_label_def(label_end, _str); //end of if statement
}


void loop_class::code(std::ostream& _str, EmitInfo _emit_info) {
  int start_loop_label = label_gen.NextLabel();
  int end_loop_label = label_gen.NextLabel();

  emit_label_def(start_loop_label, _str);
  pred->code(_str, _emit_info);
  emit_fetch_int(T1, ACC, _str);
  emit_beqz(T1, end_loop_label, _str);
  body->code(_str, _emit_info);
  emit_branch(start_loop_label, _str);

  emit_label_def(end_loop_label, _str);
  emit_move(ACC, ZERO, _str);
}


void typcase_class::code(std::ostream& _str, EmitInfo _emit_info) {

  expr->code(_str, _emit_info);

  int not_void_case_label = label_gen.NextLabel();
  emit_bnez(ACC, not_void_case_label, _str); //if ACC is not void
  emit_partial_load_address(ACC, _str);
  stringtable.lookup_string(_emit_info.GetCurrentClass()->get_filename()->get_string())->code_ref(_str);
  _str<<std::endl;
  emit_load_imm(T1, curr_lineno, _str);
  emit_jal(CASE_VOID, _str);

  emit_label_def(not_void_case_label, _str);

  emit_load(T2, 0, ACC, _str); //T2 holds the expression type class tag

  std::map<int, int> declared_case_tag_labels;
  std::vector<int> declared_case_labels;
  for (int idx_case = cases->first(); cases->more(idx_case); idx_case = cases->next(idx_case)) {
    branch_class* branch = (branch_class *)cases->nth(idx_case);
    int label_case = label_gen.NextLabel();
    declared_case_tag_labels[_emit_info.GetClassTable()->GetClassTag(branch->type_decl)] = label_case;
    declared_case_labels.emplace_back(label_case);
  }

  std::vector<std::pair<int, int>> all_case_tab_labels;
  for(auto class_tag_label : declared_case_tag_labels) {
    std::stack<int> stack_class_tags;
    stack_class_tags.emplace(class_tag_label.first);
    all_case_tab_labels.emplace_back(class_tag_label.first, class_tag_label.second);

    while(!stack_class_tags.empty()) {
      int class_tag = stack_class_tags.top();
      stack_class_tags.pop();
      if(class_tag == class_tag_label.first ||
         declared_case_tag_labels.find(class_tag) == std::end(declared_case_tag_labels)) {
        all_case_tab_labels.emplace_back(class_tag, class_tag_label.second);

        //get all children class tags and add them to the stack 
        Symbol class_name = _emit_info.GetClassTable()->GetClassName(class_tag);
        CgenNodeP curr_node = _emit_info.GetClassTable()->probe(class_name);
        //Get children
        List<CgenNode>* children = curr_node->get_children();

        while(children) {
          CgenNodeP child_node = children->hd();
          stack_class_tags.emplace(_emit_info.GetClassTable()->GetClassTag(child_node->get_name()));
          children = children->tl();
        }
      }
    }
  }

  for(auto class_tag_label: all_case_tab_labels) {
    emit_load_imm(T1, class_tag_label.first, _str);
    emit_beq(T2, T1, class_tag_label.second, _str);
  }

  int case_abort = label_gen.NextLabel();
  emit_branch(case_abort, _str);

  int end_case = label_gen.NextLabel();

  for (int idx_case = cases->first(); cases->more(idx_case); idx_case = cases->next(idx_case)) {
    branch_class* branch = (branch_class *)cases->nth(idx_case);
    emit_label_def(declared_case_labels[idx_case], _str);
    _emit_info.GetMethodStack().emplace_back(branch->name, true);
    emit_push(ACC, _str);
    branch->expr->code(_str, _emit_info);
    emit_pop(1, _str);
    _emit_info.GetMethodStack().pop_back();
    emit_branch(end_case, _str);
  }

  emit_label_def(case_abort, _str);
  emit_jal(CASE_ABORT, _str);

  emit_label_def(end_case, _str);
}


void block_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  int idx_param;
  for (idx_param = body->first(); body->more(idx_param); idx_param = body->next(idx_param)) {
    body->nth(idx_param)->code(_str, _emit_info);
  }
}


void let_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  init->code(_str, _emit_info);
  if (init->CheckNoExpression()) {
    if(IsBasicType(type_decl)) {
      //perform default initialization
      emit_partial_load_address(ACC, _str);
      EmitDefaultValue(type_decl, _str);
      _str<<std::endl;
    }
  } 

  emit_push(ACC, _str);
  _emit_info.GetMethodStack().emplace_back(identifier, true);

  body->code(_str, _emit_info);

  emit_pop(1, _str);
}


void plus_class::code(std::ostream &_str, EmitInfo _emit_info) 
{
  EvalOperands(e1, e2, _str, _emit_info, true);
  emit_add(T1, T1, T2, _str);
  emit_store_int(T1, ACC, _str);
}


void sub_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  EvalOperands(e1, e2, _str, _emit_info, true);
  emit_sub(T1, T1, T2, _str);
  emit_store_int(T1, ACC, _str);
}


void mul_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  EvalOperands(e1, e2, _str, _emit_info, true);
  emit_mul(T1, T1, T2, _str);
  emit_store_int(T1, ACC, _str);
}


void divide_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  EvalOperands(e1, e2, _str, _emit_info, true);
  emit_div(T1, T1, T2, _str);
  emit_store_int(T1, ACC, _str);
}


void neg_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  e1->code(_str, _emit_info);
  emit_jal(OBJECT_COPY, _str);
  emit_fetch_int(T1, ACC, _str);
  emit_neg(T1, T1, _str);
  emit_store_int(T1, ACC, _str);
}


void lt_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  EvalOperands(e1, e2, _str, _emit_info, false);

  emit_load_bool(ACC, truebool, _str);
  int label_lt = label_gen.NextLabel();
  emit_blt(T1, T2, label_lt, _str);
  emit_load_bool(ACC, falsebool, _str);
  emit_label_def(label_lt, _str);
}


void eq_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  //Put in $t1 and $t2 objects to be compared
  e1->code(_str, _emit_info);

  emit_push(ACC, _str);
  _emit_info.GetMethodStack().emplace_back(No_type, false); //fake type to keep the stack size

  e2->code(_str, _emit_info);

  emit_pop(1, _str); //increase SP, do not increase the stack size
  emit_load(T1, 0, SP, _str);
  emit_move(T2, ACC, _str);

  emit_load_bool(ACC, truebool, _str);//true result if objects are equal
  if(IsBasicType(e1->type)) {
    emit_load_bool(A1, falsebool, _str);//false result if objects are equal
    emit_jal(EQUALITY_TEST, _str);
  } else {
    //compare adresses of objects for non-basic types
    int label_equal = label_gen.NextLabel();
    emit_beq(T1, T2, label_equal, _str);
    emit_load_bool(ACC, falsebool, _str);
    emit_label_def(label_equal, _str);
  }
}


void leq_class::code(std::ostream& _str, EmitInfo _emit_info)
{
  EvalOperands(e1, e2, _str, _emit_info, false);

  emit_load_bool(ACC, truebool, _str);
  int label_leq = label_gen.NextLabel();
  emit_bleq(T1, T2, label_leq, _str);
  emit_load_bool(ACC, falsebool, _str);
  emit_label_def(label_leq, _str);
}


void comp_class::code(std::ostream& _str, EmitInfo _emit_info)
{
  e1->code(_str, _emit_info);

  int end_label = label_gen.NextLabel();
  emit_fetch_int(T1, ACC, _str);
  emit_load_bool(ACC, truebool, _str);
  emit_beqz(T1, end_label, _str);
  emit_load_bool(ACC, falsebool, _str);
  emit_label_def(end_label, _str);
}


void int_const_class::code(std::ostream& _str, EmitInfo _emit_info)
{
  //
  // Need to be sure we have an IntEntry *, not an arbitrary Symbol
  //
  emit_load_int(ACC,inttable.lookup_string(token->get_string()), _str);
}


void string_const_class::code(std::ostream& _str, EmitInfo _emit_info)
{
  emit_load_string(ACC,stringtable.lookup_string(token->get_string()), _str);
}


void bool_const_class::code(std::ostream& _str, EmitInfo _emit_info)
{
  emit_load_bool(ACC, BoolConst(val), _str);
}

void new__class::code(std::ostream &_str, EmitInfo _emit_info) 
{
  std::string prot_object = std::string(type_name->get_string()) + std::string(PROTOBJ_SUFFIX); //_protObj
  std::string object_init = type_name->get_string() + std::string(CLASSINIT_SUFFIX); //_init
  if (type_name != SELF_TYPE) {
    emit_load_address(ACC, &*prot_object.begin(), _str); //$a0 <- <type_name>_protobj
    emit_jal(OBJECT_COPY, _str); //create new instance
    emit_jal(&*object_init.begin(), _str); //call the initializer
  }
  else {
    emit_load_address(T1, CLASSOBJTAB, _str); //class_objTab
    emit_load(T2, 0, SELF, _str); //$t2 <- $s0 (SELF contains the class tag (idx in class_objTab)
    emit_sll(T2, T2, 3, _str); //$t2 <- $t2 << 3 (*8 = 4*2 word size * 2 (protobj+init))

    emit_addu(T1, T1, T2, _str);  //$t1 <- <type_name>_protobj
    emit_move(S1, T1, _str); //save $t1 on $s1 (must be preserved across calls)
    emit_load(ACC, 0, T1, _str); //$a0 <- <type_name>_protobj
    emit_jal(OBJECT_COPY, _str);
    emit_load(T1, 1, S1, _str); //$t1 <- <type_name>_init
    emit_jalr(T1, _str); //execute init
  }
}


void isvoid_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  e1->code(_str, _emit_info); //evaluate predicate in $a0
  emit_move(T1, ACC, _str);

  int label_false = label_gen.NextLabel();
  int label_end = label_gen.NextLabel();

  emit_beqz(T1, label_false, _str); //if(predicate != 0) jump to false
  emit_load_bool(ACC, falsebool, _str);
  emit_branch(label_end, _str); //jump to end of else statement
  emit_label_def(label_false, _str); //create label
  emit_load_bool(ACC, truebool, _str);
  emit_label_def(label_end, _str); //end of if statement
}

void no_expr_class::code(std::ostream &s, EmitInfo _emit_info) 
{
  emit_move(ACC, ZERO, s);
}

void object_class::code(std::ostream& _str, EmitInfo _emit_info) 
{
  if(name == self) {
    emit_move(ACC, SELF, _str);
  } else {
    size_t offset = _emit_info.GetMethodStack().size();
    bool is_on_stack = false;
    while(offset > 0) {
      --offset;
      if(_emit_info.GetMethodStack()[offset].GetName() == name) {
        is_on_stack = true;
        break;
      }
    }

    if(is_on_stack) {
      if(_emit_info.GetMethodStack()[offset].IsLocal()) {
        emit_load(ACC, _emit_info.GetMethodStack().size() - offset, SP, _str);
        if (cgen_Memmgr == GC_GENGC) {
          emit_addiu(A1, SP, WORD_SIZE * (_emit_info.GetMethodStack().size() - offset), _str);
          emit_gc_assign(_str);
        }
      } else {
        emit_load(ACC, offset + 3, FP, _str);
        if (cgen_Memmgr == GC_GENGC) {
          emit_addiu(A1, FP, WORD_SIZE * (offset + 3), _str);
          emit_gc_assign(_str);
        }
      }
    } else {
      bool is_member_variable = false;
      std::vector<attr_class*> attributes = _emit_info.GetCurrentClass()->GetAttributes();
      offset = 0;

      while(offset < attributes.size()) {
        if(attributes[offset]->GetName() == name) {
          is_member_variable = true;
          break;
        }
        ++offset;
      }
      assert(is_member_variable);
      if(is_member_variable) {
        emit_load(ACC, offset + DEFAULT_OBJFIELDS, SELF, _str);
        if (cgen_Memmgr == GC_GENGC) {
          emit_addiu(A1, SELF, WORD_SIZE * (offset + DEFAULT_OBJFIELDS), _str);
          emit_gc_assign(_str);
        }
      } 
    }
  }
}
