#include <assert.h>
#include <stdio.h>
#include "emit.h"
#include "cool-tree.h"
#include <map>
#include <vector>

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   List<CgenNode> *nds;
   //ostream& str;
   int stringclasstag;
   int intclasstag;
   int boolclasstag;
   unsigned _classTagCounter;

   std::map<unsigned, Symbol> tag_name_map_;
   std::map<Symbol, unsigned> name_tag_map_;

// The following methods emit code for
// constants and global declarations.

   void code_global_data(std::ostream&);
   void code_global_text(std::ostream&);
   void code_bools(std::ostream&, int);
   void code_select_gc(std::ostream&);
   void code_constants(std::ostream&);

   void EmitPrototypeObjects(CgenNode* node, std::ostream& str);
   void EmitPrototypeObjects(std::ostream& str);

   void EmitClassesNameTable(std::ostream& str);

   void EmitClassesInitializer(std::ostream& str);
   void EmitClassesInitializer(CgenNode* node, std::ostream& str);

   void EmitDispatchTables(std::ostream& str);
   void EmitDispatchTable(CgenNode* node, std::ostream& str);

   void EmitObjectInitializers(std::ostream& str);
   void EmitObjectInitializers(CgenNode* node, std::ostream& str);

   void EmitClassMethods(std::ostream& str);
   void EmitClassMethods(CgenNode* node, std::ostream& str);
// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

   void install_basic_classes();
   void install_class(CgenNodeP nd, unsigned tag);
   void install_classes(Classes cs);
   void build_inheritance_tree();
   void set_relations(CgenNodeP nd);
   unsigned nextTag();
public:
   CgenClassTable(Classes, std::ostream& str);
   void code(ostream& str);
   CgenNodeP root();
   unsigned GetClassTag(Symbol _name) {return name_tag_map_[_name];} 
   Symbol GetClassName(unsigned _class_tag) {return tag_name_map_[_class_tag];} 
};


class CgenNode : public class__class {
private: 
   CgenNodeP parentnd;                        // Parent of class
   List<CgenNode> *children;                  // Children of class
   Basicness basic_status;                    // `Basic' if class is basic
                                              // `NotBasic' otherwise
   CgenClassTableP class_table_;
   const CgenClassTableP& GetClassTable(){return class_table_;}
public:
   CgenNode(Class_ c,
            Basicness bstatus,
            CgenClassTableP _class_table);

   void add_child(CgenNodeP child);
   List<CgenNode> *get_children() { return children; }
   void set_parentnd(CgenNodeP p);
   CgenNodeP get_parentnd() { return parentnd; }
   bool basic() { return (basic_status == Basic); }
   std::vector<std::pair<CgenNodeP, method_class*>> GetMethods();
   std::vector<std::pair<CgenNodeP, method_class*>> GetClassMethods();
   std::vector<attr_class*> GetAttributes();
   std::vector<attr_class*> GetClassAttributes();

   void EmitClassInitializer(std::ostream& str);
   void EmitPrototypeObject(std::ostream& str);
   void EmitDispatchTable(std::ostream& str);
   void EmitObjectInitializer(std::ostream&, CgenClassTableP);
   void EmitClassMethods(std::ostream&, CgenClassTableP);
};

class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};

