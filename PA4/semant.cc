

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "utilities.h"
#include "semant.h"
#include <set>


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol
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



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {
  /* Fill this in */
  mapClasses.enterscope(); //enter global scope
 //sleep(60);
  install_basic_classes();
  for(int i = classes->first(); classes->more(i); i = classes->next(i)) {
    Class_ classObject = classes->nth(i);
    Symbol name = classObject->GetSymbol();
    //check if the class is already defined
    if(mapClasses.lookup(name) == nullptr) {
      mapClasses.addid(name, classObject);
    } else {
      semant_error(classObject) << "Illegal redefinition of the class " <<
        name->get_string() << ".\n";
    }
  }

  bool foundStartingPoint = false;
  Class_ classMain = dynamic_cast<Class_>(mapClasses.lookup(Main));
  if(classMain != nullptr) {
    method_class* mainMethod = GetMemberFunction(classMain, main_meth);
    Features features = classMain->GetFeatures();
    if(features != nullptr) {
      for(int i = features->first(); features->more(i); i = features->next(i)) {
        method_class* method = dynamic_cast<method_class*>(features->nth(i));
        if(method != nullptr && method->GetFeatureName() == main_meth) {
          mainMethod = method;
        }
      }
    }

    if(mainMethod != nullptr) {
      int idxFormal = mainMethod->GetFormals()->first();
      if(idxFormal == 0) {
        foundStartingPoint = true;
      } else {
        semant_error()<< "Missing main method with no parameters.\n";
      }
    } else {
      semant_error()<< "Missing main method.\n";
    }
    } else {
      semant_error() << "Class Main is not defined.\n";
    }

    for(int i = classes->first(); classes->more(i); i = classes->next(i)) {
      Class_ classObject = classes->nth(i);
      Symbol name = classObject->GetSymbol();

      //check if the parent is defined
      Symbol nameParent = classObject->GetParentSymbol();
      if (nameParent == Int || nameParent == Bool || nameParent == Str || name == SELF_TYPE) {
        semant_error(classObject) << " illegaly inherits from " << nameParent->get_string() << ".\n";
      } else {
        if(mapClasses.lookup(nameParent) == nullptr) {
          semant_error(classObject) << "Illegal inheritance of class " <<
            name->get_string() << " trying to derive from "<< nameParent->get_string() <<" .\n";
        }
      }
    }
  }

bool ClassTable::CheckCompatibleTypes(Symbol derivedType, Symbol baseType, Class_ cls) {
  bool areTypesCompatible = true;
  if( (baseType != derivedType)  && baseType != Object && derivedType != No_type) {
    areTypesCompatible = false;
    if(derivedType == SELF_TYPE) {
      derivedType = cls->GetSymbol();
    }
    if(derivedType != Object) {
      Class_ currClass = (Class_)GetSymbolTable().lookup(derivedType);
      Symbol firstParent = currClass->GetSymbol();
      while(firstParent != Object && firstParent != baseType) {
        currClass = (Class_)GetSymbolTable().lookup(firstParent);
        firstParent = currClass->GetParentSymbol();
      }
      if(firstParent == baseType) {
        areTypesCompatible = true;
      }
    }
  }
  return areTypesCompatible;
}

method_class* ClassTable::GetMemberFunction(Class_ cls, Symbol name) {
  method_class* method = nullptr;
  Class_ searchClass = cls;
  while(searchClass != nullptr && method == nullptr) {
    method = dynamic_cast<method_class*>(searchClass->GetMemberFunctions().lookup(name));
    if(method == nullptr) {
      searchClass = dynamic_cast<Class_>(GetSymbolTable().lookup(searchClass->GetParentSymbol()));
    } else {
      searchClass = nullptr;
    }
  }
  return method;
}

Entry* ClassTable::GetMemberAttribute(Class_ cls, Symbol name) {
  Entry* attribute = nullptr;
  Class_ searchClass = cls;
  while(searchClass != nullptr && attribute == nullptr) {
    attribute = searchClass->GetObjects().lookup(name);
    if(attribute == nullptr) {
      searchClass = dynamic_cast<Class_>(GetSymbolTable().lookup(searchClass->GetParentSymbol()));
    } else {
      searchClass = nullptr;
    }
  }
  return attribute;
}

bool ClassTable::CheckFunctionCall(method_class* method, Expressions actuals, Class_ cls) {
  int idxParams =  method->GetFormals()->first();
  int idxArgs =  actuals->first();
  bool match = true;
  while(match && actuals->more(idxArgs) && method->GetFormals()->more(idxParams)) {
    Expression arg = actuals->nth(idxArgs);
    Symbol arg_type = arg->get_type();
    if(arg->get_type() == SELF_TYPE) {
      arg_type = cls->GetSymbol();
    }
    Formal param = method->GetFormals()->nth(idxParams);
    if(!CheckCompatibleTypes(arg_type, param->GetFormalType(), cls)) {
      match = false;
      semant_error(cls)<<"Argument of type: "<< arg->get_type()->get_string()
        <<" doesn't match function parameter "<<param->GetFormalType()->get_string()<<" .\n";
    } else {
      idxArgs = actuals->next(idxArgs);
      idxParams = method->GetFormals()->next(idxParams);
    }
  }
  if(match && (actuals->more(idxArgs) || method->GetFormals()->more(idxParams))) {
      semant_error(cls) << "Number of arguments in function call does not match in function "
        <<method->GetFeatureName()->get_string()<<" .\n";
      match = false;
  }
  return match;
}

bool ClassTable::CheckInheritedMethod(method_class* derived, method_class* base, Class_ cls)
{
  int idxDerived =  derived->GetFormals()->first();
  int idxBase =  base->GetFormals()->first();

  bool match = true;
  while(match 
        && derived->GetFormals()->more(idxDerived)
        && base->GetFormals()->more(idxBase)) {
    Formal paramDerived = derived->GetFormals()->nth(idxDerived);
    Formal paramBase = base->GetFormals()->nth(idxBase);
    if(paramDerived->GetFormalType() != paramBase->GetFormalType()) {
      match = false;
      semant_error(cls) <<"Argument of type: "<<paramDerived->GetFormalType()->get_string()
        << " doesn't match inherited function parameter type"
        << paramBase->GetFormalType()->get_string() <<" .\n";
    } else {
      idxDerived = base->GetFormals()->next(idxDerived);
      idxBase = base->GetFormals()->next(idxBase);
    }
  }
  if(match &&
      (base->GetFormals()->more(idxBase) || derived->GetFormals()->more(idxDerived))) {
    semant_error(cls) << "Number of arguments in inherited function does not match: "
        <<derived->GetFeatureName()->get_string()<<".\n";
    match = false;
  }
  return match;
}

ClassTable::~ClassTable() {
  mapClasses.exitscope(); //leave global scope
}

void ClassTable::install_basic_classes() {

  // The tree package uses these globals to annotate the classes built below.
  // curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

  // The following demonstrates how to create dummy parse trees to
  // refer to basic Cool classes.  There's no need for method
  // bodies -- these are already built into the runtime system.

  // IMPORTANT: The results of the following expressions are
  // stored in local variables.  You will want to do something
  // with those variables at the end of this method to make this
  // code meaningful.

  //
  // The Object class has no parent class. Its methods are
  //        abort() : Object    aborts the program
  //        type_name() : Str   returns a string representation of class name
  //        copy() : SELF_TYPE  returns a copy of the object
  //
  // There is no need for method bodies in the basic classes---these
  // are already built in to the runtime system.

  Class_ Object_class =
    class_(Object,
           No_class,
           append_Features(
                           append_Features(
                                           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                                           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                           single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
           filename);
  mapClasses.addid(Object, Object_class);
  Object_class->RegisterClass(this);
  //Object_class->SetTypes(this);
  //
  // The IO class inherits from Object. Its methods are
  //        out_string(Str) : SELF_TYPE       writes a string to the output
  //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
  //        in_string() : Str                 reads a string from the input
  //        in_int() : Int                      "   an int     "  "     "
  //
  Class_ IO_class =
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
           filename);
  mapClasses.addid(IO, IO_class);
  IO_class->RegisterClass(this);
  //IO_class->SetTypes(this);
  //
  // The Int class has no methods and only a single attribute, the
  // "val" for the integer.
  //
  Class_ Int_class =
    class_(Int,
           Object,
           single_Features(attr(val, prim_slot, no_expr())),
           filename);

  mapClasses.addid(Int, Int_class);
  Int_class->RegisterClass(this);
  //Int_class->SetTypes(this);
  //
  // Bool also has only the "val" slot.
  //
  Class_ Bool_class =
    class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
  mapClasses.addid(Bool, Bool_class);
  //mapClasses[Bool] = Bool_class;
  //
  // The class Str has a number of slots and operations:
  //       val                                  the length of the string
  //       str_field                            the string itself
  //       length() : Int                       returns length of the string
  //       concat(arg: Str) : Str               performs string concatenation
  //       substr(arg: Int, arg2: Int): Str     substring selection
  //
  Class_ Str_class =
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
           filename);
  mapClasses.addid(Str, Str_class);
  Str_class->RegisterClass(this);
  //Str_class->SetTypes(this);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{
    return semant_error(c->get_filename(),c);
}

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()
{
    semant_errors++;
    return error_stream;
}

std::vector<Symbol> ClassTable::GetInheritancePath(Symbol sym) {
  std::vector<Symbol> pathFromRoot;
  Symbol iterSym = sym;
  while (iterSym != No_class) {
    pathFromRoot.insert(pathFromRoot.begin(), iterSym);
    Class_ cls = dynamic_cast<Class_>(GetSymbolTable().lookup(iterSym));
    iterSym == nullptr;
    if(cls != nullptr) {
      iterSym = cls->GetParentSymbol();
    }
  }
  return pathFromRoot;
}

Symbol ClassTable::GetCommonAncestor(Symbol sym1, Symbol sym2, Class_ cls) {
  Symbol commonAncestor = nullptr;
  if(sym1 == SELF_TYPE || sym2 == SELF_TYPE) {
    if(sym1 == sym2) {
      commonAncestor = SELF_TYPE;
    } else {
      if(sym1 == SELF_TYPE) {
        sym1 = cls->GetSymbol();
      } else {
        sym2 = cls->GetSymbol();
      }
    }
  }

  if (commonAncestor == nullptr) {
    if(sym1 == No_type) {
      commonAncestor = sym2;
    } else if(sym2 == No_type) {
      commonAncestor = sym1;
    } else {
      std::vector<Symbol> path1 = GetInheritancePath(sym1);
      std::vector<Symbol> path2 = GetInheritancePath(sym2);
      commonAncestor = nullptr;
      auto iterPath1 = path1.begin();
      auto iterPath2 = path2.begin();
      for(;
          iterPath1 != path1.end() && iterPath2 != path2.cend();
          iterPath1++, iterPath2++) {
        if(*iterPath1 == *iterPath2) {
          commonAncestor = *iterPath1;
        }
      }
    }
  }
  return commonAncestor;
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */

void program_class::semant()
{
    initialize_constants();

    ClassTable classTable(classes);
    SetTypes(&classTable);

    /* ClassTable constructor may do some semantic analysis */
    /* some semantic analysis code may go here */

    if (classTable.errors()) {
      cerr << "Compilation halted due to static semantic errors." << endl;
      exit(1);
    }

}

void program_class::SetTypes(ClassTable* classTable) {
  classTable->GetSymbolTable().enterscope();
  //set types for basic classes

  for(int i = classes->first(); classes->more(i); i = classes->next(i)) {
    classes->nth(i)->RegisterClass(classTable);
  }
  for(int i = classes->first(); classes->more(i); i = classes->next(i)) {
    classes->nth(i)->SetTypes(classTable);
  }
  classTable->GetSymbolTable().exitscope();
}

class__class::class__class(Symbol a1, Symbol a2, Features a3, Symbol a4) {
  name = a1;
  parent = a2;
  features = a3;
  filename = a4;
  GetObjects().enterscope();
  GetMemberFunctions().enterscope();
}
class__class::~class__class() {
  GetMemberFunctions().exitscope();
  GetObjects().exitscope();
}

void class__class::RegisterClass(ClassTable* classTable) {
  classTable->GetSymbolTable().addid(self, this);
  for(int i = features->first(); features->more(i); i = features->next(i)) {
    features->nth(i)->RegisterMember(classTable, this);
  }
}

void class__class::SetTypes(ClassTable* classTable) {
  for(int i = features->first(); features->more(i); i = features->next(i)) {
    features->nth(i)->SetType(classTable, this);
  }
}


void method_class::SetType(ClassTable* classTable, Class_ cls) {
  cls->GetObjects().enterscope();
  for(int i = formals->first(); formals->more(i); i = formals->next(i)) {
    formals->nth(i)->RegisterFormal(classTable, cls);
  }
  expr->SetExpressionType(classTable, cls);
  if(return_type == SELF_TYPE || classTable->GetSymbolTable().lookup(return_type) != nullptr) {
    if(!classTable->CheckCompatibleTypes(expr->get_type(), return_type, cls)) {
      classTable->semant_error(cls->get_filename(), this)
        << "Cannot infer expression " << expr->get_type()->get_string()
        << " to declared type "<<return_type->get_string()  <<"\n";
    }
  } else {
    classTable->semant_error(cls->get_filename(), this)
      << "Return type " << return_type->get_string()
      << " is undefined.\n";
  }
  cls->GetObjects().exitscope();
}

void method_class::RegisterMember(ClassTable* classTable, Class_ cls) {
    method_class* baseMethod = classTable->GetMemberFunction(cls, name);
    if(baseMethod == nullptr || true == classTable->CheckInheritedMethod(this, baseMethod, cls)) {
      cls->GetMemberFunctions().addid(name, this);
    }
}

void attr_class::SetType(ClassTable* classTable, Class_ cls) {
  init->SetExpressionType(classTable, cls);
  if(name == self) {
    classTable->semant_error(cls->get_filename(), this)
      << "'self' cannot be an attribute name.\n";
  }
}

void attr_class::RegisterMember(ClassTable* classTable, Class_ cls) {
  if(classTable->GetMemberAttribute(cls, name) == nullptr) {
    cls->GetObjects().addid(name, type_decl);
  } else {
    classTable->semant_error(cls->get_filename(), this) 
      << " Attempt to override an existing member from parent: "
      << name->get_string()<<".\n";
  }
}

void Expression_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  type = nullptr;
}
int_const_class::int_const_class(Symbol a1) {
  token = a1;
}

void int_const_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  type = Int;
}

bool_const_class::bool_const_class(Boolean a1) {
  val = a1;
}

void bool_const_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  type = Bool;
}

string_const_class::string_const_class(Symbol a1) {
  token = a1;
}

void string_const_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  type = Str;
}

new__class::new__class(Symbol a1) {
  type_name = a1;
}

void new__class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  type = type_name;
}

assign_class::assign_class(Symbol a1, Expression a2) {
  name = a1;
  expr = a2;
}

void assign_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  expr->SetExpressionType(classTable, cls);
  type = expr->get_type();
  Symbol typeId = classTable->GetMemberAttribute(cls, name);
  if(typeId == nullptr) {
    classTable->semant_error(cls->get_filename() ,this)<<"Undeclared identifier "
      <<name->get_string()<<" cannot be assigned.\n";;
  } else {
    if(!classTable->CheckCompatibleTypes(type, typeId, cls)) {
      classTable->semant_error(cls->get_filename() ,this)
        <<"Wrong assignment of expression of type "<<type->get_string()
        <<" to a variable of type "<<typeId->get_string()<<".\n";
    }
  }
}

Feature_class::Feature_class() {
  line_number = curr_lineno;
}


void isvoid_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  e1->SetExpressionType(classTable, cls);
  type = Bool;
}

void block_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  cls->GetObjects().enterscope();

  for(int i = body->first(); body->more(i); i = body->next(i)) {
    body->nth(i)->SetExpressionType(classTable, cls);
    type = body->nth(i)->get_type();
  }
  cls->GetObjects().exitscope();
}

void no_expr_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  type = No_type;
}

void object_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  if(name == self) {
    type = SELF_TYPE;
  } else {
    Symbol typeId = classTable->GetMemberAttribute(cls, name);
    if(typeId == nullptr) {
      classTable->semant_error(cls->get_filename() ,this)<<"Undeclared identifier "
        <<name->get_string()<<".\n";;
      type = Object;
    } else {
      type = typeId;
    }
  }
}

void formal_class::RegisterFormal(ClassTable* classTable, Class_ cls) {
  if(name == self || type_decl == SELF_TYPE) {
    classTable->semant_error(cls->get_filename() ,this)
      <<"Illegal definition of parameter " << name->get_string()<<" of type "
      << type_decl->get_string()<<".\n";
  } else {

    if(cls->GetObjects().probe(name) == nullptr) {
      cls->GetObjects().addid(name, type_decl);
    } else {
      classTable->semant_error(cls->get_filename() ,this)
        <<"Duplication of " << name->get_string()<<" parameter not allowed.\n";
    }
  }
}


void plus_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  e1->SetExpressionType(classTable, cls);
  e2->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  Symbol type_e2 = e2->get_type();

  if(type_e1 == Int && type_e2 == Int) {
    type = Int;
  } else {
    classTable->semant_error(cls->get_filename() ,this)
      <<" Only adding integer types allowed, while trying to add "
        << "e1 of type "<< type_e1->get_string() << " with "
        << "e2 of type "<< type_e2->get_string() << ".\n";
    type = Object;
  }
}

void eq_class::SetExpressionType(ClassTable* classTable, Class_ cls) {

  type = Bool;
  e1->SetExpressionType(classTable, cls);
  e2->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  Symbol type_e2 = e2->get_type();
  if(type_e1 != type_e2) {
    if(type_e1 == Int || type_e1 == Bool || type_e1 == Str
       || type_e2 == Int || type_e2 == Bool || type_e2 == Str) {
      classTable->semant_error(cls->get_filename() ,this) 
        << "Illegal basic type comparison "
        << "e1 of type "<< type_e1->get_string() << " with "
        << "e2 of type "<< type_e2->get_string() << ".\n";
      type = Object;
    }
  }
}

void leq_class::SetExpressionType(ClassTable* classTable, Class_ cls) {

  e1->SetExpressionType(classTable, cls);
  e2->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  Symbol type_e2 = e2->get_type();
  if(type_e1 == Int && type_e2 == Int) {
    type = Bool;
  } else {
      classTable->semant_error(cls->get_filename() ,this) 
        << "Illegal <= comparison of non-Ints"
        << "e1 of type "<< type_e1->get_string() << " with "
        << "e2 of type "<< type_e2->get_string() << ".\n";
      type = Object;
  }
}

void lt_class::SetExpressionType(ClassTable* classTable, Class_ cls) {

  e1->SetExpressionType(classTable, cls);
  e2->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  Symbol type_e2 = e2->get_type();
  if(type_e1 == Int && type_e2 == Int) {
    type = Bool;
  } else {
      classTable->semant_error(cls->get_filename() ,this) 
        << "Illegal < comparison of non-Ints"
        << "e1 of type "<< type_e1->get_string() << " with "
        << "e2 of type "<< type_e2->get_string() << ".\n";
      type = Object;
  }
}

void neg_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  e1->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  if(type_e1 == Int) {
    type = Int;
  } else {
      classTable->semant_error(cls->get_filename() ,this) 
        << "Illegal negation of non-Ints"
        << "e1 of type "<< type_e1->get_string() << ".\n";
      type = Object;
  }
}


void comp_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  e1->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  if(type_e1 == Bool) {
    type = Bool;
  } else {
      classTable->semant_error(cls->get_filename() ,this) 
        << "Illegal negation of non-Bools"
        << "e1 of type "<< type_e1->get_string() << ".\n";
      type = Object;
  }
}

void sub_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  e1->SetExpressionType(classTable, cls);
  e2->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  Symbol type_e2 = e2->get_type();

  if(type_e1 == Int && type_e2 == Int) {
    type = Int;
  } else {
    classTable->semant_error(cls->get_filename() ,this)
      <<" Only substracting integer types allowed, while trying to substract "
        << "e2 of type "<< type_e2->get_string() << " from "
        << "e1 of type "<< type_e1->get_string() << ".\n";
    type = Object;
  }
}

void mul_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  e1->SetExpressionType(classTable, cls);
  e2->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  Symbol type_e2 = e2->get_type();

  if(type_e1 == Int && type_e2 == Int) {
    type = Int;
  } else {
    classTable->semant_error(cls->get_filename() ,this)
      <<" Only integer multiplication types allowed, while trying to multiply "
        << "e1 of type "<< type_e1->get_string() << " with "
        << "e2 of type "<< type_e2->get_string() << ".\n";
    type = Object;
  }
}

void divide_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  e1->SetExpressionType(classTable, cls);
  e2->SetExpressionType(classTable, cls);

  Symbol type_e1 = e1->get_type();
  Symbol type_e2 = e2->get_type();

  if(type_e1 == Int && type_e2 == Int) {
    type = Int;
  } else {
    classTable->semant_error(cls->get_filename() ,this)
      <<" Only integer division types allowed, while trying to divide "
        << "e1 of type "<< type_e1->get_string() << " to "
        << "e2 of type "<< type_e2->get_string() << ".\n";
    type = Object;
  }
}

void dispatch_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  expr->SetExpressionType(classTable, cls);
  for(int i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->SetExpressionType(classTable, cls);
  }
  Symbol exprType = expr->get_type();
  Class_ localCls = nullptr;
  if (expr->get_type() == SELF_TYPE) {
    localCls = cls;
  } else {
    localCls = dynamic_cast<Class_>(classTable->GetSymbolTable().lookup(expr->get_type()));
  }
  if(localCls == nullptr) {
    classTable->semant_error(cls->get_filename() ,this)
      <<" Use of undefined class " << expr->get_type()<<".\n";
    type = Object;
  } else {
    method_class* method = classTable->GetMemberFunction(localCls, name);
    if(method != nullptr) {
      if(classTable->CheckFunctionCall(method, actual, cls)) {
        type = method->GetReturnType();
        if (type == SELF_TYPE) {
          type = exprType;
        }
      } else {
        classTable->semant_error(cls->get_filename() ,this)
        <<"Parameter list does not match in "<<name->get_string()<<"function call.\n";
      }
    } else {
      classTable->semant_error(cls->get_filename() ,this)
        <<"Undefined member function: "<<name->get_string()<<".\n";
      type = Object;
    }
  }
}

void static_dispatch_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  expr->SetExpressionType(classTable, cls);
  for(int i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->SetExpressionType(classTable, cls);
  }

  Class_ localCls = nullptr;
  if (expr->get_type() == SELF_TYPE) {
    localCls = cls;
  } else {
    localCls = dynamic_cast<Class_>(classTable->GetSymbolTable().lookup(expr->get_type()));
  }
  Class_ targetCls = dynamic_cast<Class_>(classTable->GetSymbolTable().lookup(type_name));
  if(targetCls == nullptr) {
    classTable->semant_error(cls->get_filename() ,this)
      <<" Use of undefined class " << type_name->get_string()<<"as type in static_dispatch.\n";
    type = Object;
  } else {
    if(false == classTable->CheckCompatibleTypes(localCls->GetSymbol(), type_name, cls)) {
      classTable->semant_error(cls->get_filename() ,this)
        <<" Incompatible static_dispatch of  type " << type_name->get_string()
        << " which must be a base of "<< localCls->GetSymbol()->get_string()<<".\n";
      type = Object;
    } else {
      method_class* method = classTable->GetMemberFunction(targetCls, name);
      if(method != nullptr) {
        type = method->GetReturnType();
      } else {
        classTable->semant_error(cls->get_filename() ,this)
          <<"Undefined member function: "<<name->get_string()<<" in static_dispatch with type " 
          <<type_name->get_string()<<".\n";
        type = Object;
      }
    }
  }
}

void let_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  if(identifier == self) {
    classTable->semant_error(cls->get_filename() ,this)
      <<"Illegal to bind self into 'let' expressions.\n";
    type = Object;
  } else {
    if(type_decl != SELF_TYPE
       && classTable->GetSymbolTable().lookup(type_decl) == nullptr) {
      classTable->semant_error(cls->get_filename() ,this)
        <<"Undefined class: "<<type_decl->get_string()<<".\n";
      type = Object;
    } else {
      cls->GetObjects().enterscope();
      cls->GetObjects().addid(identifier, type_decl);
      init->SetExpressionType(classTable, cls);
      body->SetExpressionType(classTable, cls);
      type = body->get_type();
      Symbol init_type = init->get_type();
      if(!classTable->CheckCompatibleTypes(init_type, type_decl, cls)) {
        classTable->semant_error(cls->get_filename() ,this)
          <<"Wrong assignment of expression of type "<<init_type->get_string()
          <<" to a variable of type "<<type_decl->get_string()<<".\n";
      }
      cls->GetObjects().exitscope();
    }
  }
}

void branch_class::SetTypes(ClassTable* classTable, Class_ cls) {
  cls->GetObjects().enterscope();
  cls->GetObjects().addid(name, type_decl);
  expr->SetExpressionType(classTable, cls);
  cls->GetObjects().exitscope();
}

void typcase_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  std::set<Symbol> types;
  expr->SetExpressionType(classTable, cls);
  type = No_type;
  for(int i = cases->first(); cases->more(i); i = cases->next(i)) {
    Case_class* branch = cases->nth(i);
    if(types.find(branch->GetTypeDecl()) == types.end()) {
      types.insert(branch->GetTypeDecl());
      branch->SetTypes(classTable, cls);
      type = classTable->GetCommonAncestor(type, branch->GetExprType(), cls);
    } else {
        classTable->semant_error(cls->get_filename() ,this)
          <<"Branch of type "<<branch->GetTypeDecl()->get_string()
          <<" cannot be duplicated.\n";
    }
  }
}

void loop_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  pred->SetExpressionType(classTable, cls);
  if(pred->get_type() != Bool) {
        classTable->semant_error(cls->get_filename() ,this)
          <<"Predicate of loop statement must be of type Bool.\n";
  }
  body->SetExpressionType(classTable, cls);
  type = Object;
}

void cond_class::SetExpressionType(ClassTable* classTable, Class_ cls) {
  pred->SetExpressionType(classTable, cls);
  if(pred->get_type() != Bool) {
    classTable->semant_error(cls->get_filename() ,this)
      <<"Predicate of loop statement must be of type Bool.\n";
  }
  then_exp->SetExpressionType(classTable, cls);
  else_exp->SetExpressionType(classTable, cls);
  Symbol then_type = then_exp->get_type();
  Symbol else_type = else_exp->get_type();
  type = classTable->GetCommonAncestor(then_type, else_type, cls);
}
