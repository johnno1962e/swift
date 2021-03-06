//===--- TypeRef.cpp - Swift Type References for Reflection ---------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2016 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
// Implements the structures of type references for property and enum
// case reflection.
//
//===----------------------------------------------------------------------===//

#include "swift/Basic/Demangle.h"
#include "swift/Reflection/TypeRef.h"
#include "swift/Reflection/TypeRefBuilder.h"

using namespace swift;
using namespace reflection;

class PrintTypeRef : public TypeRefVisitor<PrintTypeRef, void> {
  std::ostream &OS;
  unsigned Indent;

  std::ostream &indent(unsigned Amount) {
    for (unsigned i = 0; i < Amount; ++i)
      OS << ' ';
    return OS;
  }

  std::ostream &printHeader(std::string Name) {
    indent(Indent) << '(' << Name;
    return OS;
  }

  template<typename T>
  std::ostream &printField(std::string name, const T &value) {
    if (!name.empty())
      OS << " " << name << "=" << value;
    else
      OS << " " << value;
    return OS;
  }

  void printRec(const TypeRef *typeRef) {
    OS << "\n";

    Indent += 2;
    visit(typeRef);
    Indent -= 2;
  }

public:
  PrintTypeRef(std::ostream &OS, unsigned Indent)
    : OS(OS), Indent(Indent) {}

  void visitBuiltinTypeRef(const BuiltinTypeRef *B) {
    printHeader("builtin");
    auto demangled = Demangle::demangleTypeAsString(B->getMangledName());
    printField("", demangled);
    OS << ')';
  }

  void visitNominalTypeRef(const NominalTypeRef *N) {
    if (N->isStruct())
      printHeader("struct");
    else if (N->isEnum())
      printHeader("enum");
    else if (N->isClass())
      printHeader("class");
    else
      printHeader("nominal");
    auto demangled = Demangle::demangleTypeAsString(N->getMangledName());
    printField("", demangled);
    if (auto parent = N->getParent())
      printRec(parent);
    OS << ')';
  }

  void visitBoundGenericTypeRef(const BoundGenericTypeRef *BG) {
    if (BG->isStruct())
      printHeader("bound_generic_struct");
    else if (BG->isEnum())
      printHeader("bound_generic_enum");
    else if (BG->isClass())
      printHeader("bound_generic_class");
    else
      printHeader("bound_generic");

    auto demangled = Demangle::demangleTypeAsString(BG->getMangledName());
    printField("", demangled);
    for (auto param : BG->getGenericParams())
      printRec(param);
    if (auto parent = BG->getParent())
      printRec(parent);
    OS << ')';
  }

  void visitTupleTypeRef(const TupleTypeRef *T) {
    printHeader("tuple");
    for (auto element : T->getElements())
      printRec(element);
    OS << ')';
  }

  void visitFunctionTypeRef(const FunctionTypeRef *F) {
    printHeader("function");
    for (auto Arg : F->getArguments())
      printRec(Arg);
    printRec(F->getResult());
    OS << ')';
  }

  void visitProtocolTypeRef(const ProtocolTypeRef *P) {
    printHeader("protocol");
    printField("module", P->getModuleName());
    printField("name", P->getName());
    OS << ')';
  }

  void visitProtocolCompositionTypeRef(const ProtocolCompositionTypeRef *PC) {
    printHeader("protocol_composition");
    for (auto protocol : PC->getProtocols())
      printRec(protocol);
    OS << ')';
  }

  void visitMetatypeTypeRef(const MetatypeTypeRef *M) {
    printHeader("metatype");
    printRec(M->getInstanceType());
    OS << ')';
  }

  void visitExistentialMetatypeTypeRef(const ExistentialMetatypeTypeRef *EM) {
    printHeader("existential_metatype");
    printRec(EM->getInstanceType());
    OS << ')';
  }

  void visitGenericTypeParameterTypeRef(const GenericTypeParameterTypeRef *GTP){
    printHeader("generic_type_parameter");
    printField("depth", GTP->getDepth());
    printField("index", GTP->getIndex());
    OS << ')';
  }

  void visitDependentMemberTypeRef(const DependentMemberTypeRef *DM) {
    printHeader("dependent_member");
    printRec(DM->getProtocol());
    printRec(DM->getBase());
    printField("member", DM->getMember());
    OS << ')';
  }

  void visitForeignClassTypeRef(const ForeignClassTypeRef *F) {
    printHeader("foreign");
    if (!F->getName().empty())
      printField("name", F->getName());
    OS << ')';
  }

  void visitObjCClassTypeRef(const ObjCClassTypeRef *OC) {
    printHeader("objective_c_class");
    if (!OC->getName().empty())
      printField("name", OC->getName());
    OS << ')';
  }

  void visitUnownedStorageTypeRef(const UnownedStorageTypeRef *US) {
    printHeader("unowned_storage");
    printRec(US->getType());
    OS << ')';
  }

  void visitWeakStorageTypeRef(const WeakStorageTypeRef *WS) {
    printHeader("weak_storage");
    printRec(WS->getType());
    OS << ')';
  }

  void visitUnmanagedStorageTypeRef(const UnmanagedStorageTypeRef *US) {
    printHeader("unmanaged_storage");
    printRec(US->getType());
    OS << ')';
  }

  void visitOpaqueTypeRef(const OpaqueTypeRef *O) {
    printHeader("opaque");
    OS << ')';
  }
};

struct TypeRefIsConcrete
  : public TypeRefVisitor<TypeRefIsConcrete, bool> {
  bool visitBuiltinTypeRef(const BuiltinTypeRef *B) {
    return true;
  }

  bool visitNominalTypeRef(const NominalTypeRef *N) {
    return true;
  }

  bool visitBoundGenericTypeRef(const BoundGenericTypeRef *BG) {
    std::vector<TypeRef *> GenericParams;
    for (auto Param : BG->getGenericParams())
      if (!visit(Param))
        return false;
    return true;
  }

  bool visitTupleTypeRef(const TupleTypeRef *T) {
    for (auto Element : T->getElements()) {
      if (!visit(Element))
        return false;
    }
    return true;
  }

  bool visitFunctionTypeRef(const FunctionTypeRef *F) {
    std::vector<TypeRef *> SubstitutedArguments;
    for (auto Argument : F->getArguments())
      if (!visit(Argument))
        return false;
    return visit(F->getResult());
  }

  bool visitProtocolTypeRef(const ProtocolTypeRef *P) {
    return true;
  }

  bool
  visitProtocolCompositionTypeRef(const ProtocolCompositionTypeRef *PC) {
    for (auto Protocol : PC->getProtocols())
      if (!visit(Protocol))
        return false;
    return true;
  }

  bool visitMetatypeTypeRef(const MetatypeTypeRef *M) {
    return visit(M->getInstanceType());
  }

  bool
  visitExistentialMetatypeTypeRef(const ExistentialMetatypeTypeRef *EM) {
    return visit(EM->getInstanceType());
  }

  bool
  visitGenericTypeParameterTypeRef(const GenericTypeParameterTypeRef *GTP){
    return false;
  }

  bool
  visitDependentMemberTypeRef(const DependentMemberTypeRef *DM) {
    return visit(DM->getBase());
  }

  bool visitForeignClassTypeRef(const ForeignClassTypeRef *F) {
    return true;
  }

  bool visitObjCClassTypeRef(const ObjCClassTypeRef *OC) {
    return true;
  }
  
  bool visitOpaqueTypeRef(const OpaqueTypeRef *Op) {
    return true;
  }

  bool visitUnownedStorageTypeRef(const UnownedStorageTypeRef *US) {
    return visit(US->getType());
  }

  bool visitWeakStorageTypeRef(const WeakStorageTypeRef *WS) {
    return visit(WS->getType());
  }

  bool visitUnmanagedStorageTypeRef(const UnmanagedStorageTypeRef *US) {
    return visit(US->getType());
  }
};

const ForeignClassTypeRef *
ForeignClassTypeRef::UnnamedSingleton = new ForeignClassTypeRef("");

const ForeignClassTypeRef *ForeignClassTypeRef::getUnnamed() {
  return UnnamedSingleton;
}

const ObjCClassTypeRef *
ObjCClassTypeRef::UnnamedSingleton = new ObjCClassTypeRef("");

const ObjCClassTypeRef *ObjCClassTypeRef::getUnnamed() {
  return UnnamedSingleton;
}

const OpaqueTypeRef *
OpaqueTypeRef::Singleton = new OpaqueTypeRef();

const OpaqueTypeRef *OpaqueTypeRef::get() {
  return Singleton;
}

void TypeRef::dump() const {
  dump(std::cerr);
}

void TypeRef::dump(std::ostream &OS, unsigned Indent) const {
  PrintTypeRef(OS, Indent).visit(this);
  OS << std::endl;
}

bool TypeRef::isConcrete() const {
  return TypeRefIsConcrete().visit(this);
}

unsigned NominalTypeTrait::getDepth() const {
  if (auto P = Parent) {
    switch (P->getKind()) {
    case TypeRefKind::Nominal:
      return 1 + cast<NominalTypeRef>(P)->getDepth();
    case TypeRefKind::BoundGeneric:
      return 1 + cast<BoundGenericTypeRef>(P)->getDepth();
    default:
      assert(false && "Asked for depth on non-nominal typeref");
    }
  }

  return 0;
}

GenericArgumentMap TypeRef::getSubstMap() const {
  GenericArgumentMap Substitutions;
  switch (getKind()) {
    case TypeRefKind::Nominal: {
      auto Nom = cast<NominalTypeRef>(this);
      if (auto Parent = Nom->getParent())
        return Parent->getSubstMap();
      return GenericArgumentMap();
    }
    case TypeRefKind::BoundGeneric: {
      auto BG = cast<BoundGenericTypeRef>(this);
      auto Depth = BG->getDepth();
      unsigned Index = 0;
      for (auto Param : BG->getGenericParams())
        Substitutions.insert({{Depth, Index++}, Param});
      if (auto Parent = BG->getParent()) {
        auto ParentSubs = Parent->getSubstMap();
        Substitutions.insert(ParentSubs.begin(), ParentSubs.end());
      }
      break;
    }
    default:
      break;
  }
  return Substitutions;
}

namespace {
bool isStruct(Demangle::NodePointer Node) {
  switch (Node->getKind()) {
    case Demangle::Node::Kind::Type:
      return isStruct(Node->getChild(0));
    case Demangle::Node::Kind::Structure:
    case Demangle::Node::Kind::BoundGenericStructure:
      return true;
    default:
      return false;
  }
}
bool isEnum(Demangle::NodePointer Node) {
  switch (Node->getKind()) {
    case Demangle::Node::Kind::Type:
      return isEnum(Node->getChild(0));
    case Demangle::Node::Kind::Enum:
    case Demangle::Node::Kind::BoundGenericEnum:
      return true;
    default:
      return false;
  }
}
bool isClass(Demangle::NodePointer Node) {
  switch (Node->getKind()) {
    case Demangle::Node::Kind::Type:
      return isClass(Node->getChild(0));
    case Demangle::Node::Kind::Class:
    case Demangle::Node::Kind::BoundGenericClass:
      return true;
    default:
      return false;
  }
}
}

bool NominalTypeTrait::isStruct() const {
  auto Demangled = Demangle::demangleTypeAsNode(MangledName);
  return ::isStruct(Demangled);
}


bool NominalTypeTrait::isEnum() const {
  auto Demangled = Demangle::demangleTypeAsNode(MangledName);
  return ::isEnum(Demangled);
}


bool NominalTypeTrait::isClass() const {
  auto Demangled = Demangle::demangleTypeAsNode(MangledName);
  return ::isClass(Demangled);
}

class TypeRefSubstitution
  : public TypeRefVisitor<TypeRefSubstitution, const TypeRef *> {
  TypeRefBuilder &Builder;
  GenericArgumentMap Substitutions;
public:
  using TypeRefVisitor<TypeRefSubstitution, const TypeRef *>::visit;

  TypeRefSubstitution(TypeRefBuilder &Builder, GenericArgumentMap Substitutions)
    : Builder(Builder), Substitutions(Substitutions) {}

  const TypeRef *visitBuiltinTypeRef(const BuiltinTypeRef *B) {
    return B;
  }

  const TypeRef *visitNominalTypeRef(const NominalTypeRef *N) {
    return N;
  }

  const TypeRef *visitBoundGenericTypeRef(const BoundGenericTypeRef *BG) {
    std::vector<const TypeRef *> GenericParams;
    for (auto Param : BG->getGenericParams())
      GenericParams.push_back(visit(Param));
    return BoundGenericTypeRef::create(Builder, BG->getMangledName(),
                                       GenericParams);
  }

  const TypeRef *visitTupleTypeRef(const TupleTypeRef *T) {
    std::vector<const TypeRef *> Elements;
    for (auto Element : T->getElements()) {
      Elements.push_back(visit(Element));
    }
    return TupleTypeRef::create(Builder, Elements);
  }

  const TypeRef *visitFunctionTypeRef(const FunctionTypeRef *F) {
    std::vector<const TypeRef *> SubstitutedArguments;
    for (auto Argument : F->getArguments())
      SubstitutedArguments.push_back(visit(Argument));

    auto SubstitutedResult = visit(F->getResult());

    return FunctionTypeRef::create(Builder, SubstitutedArguments,
                                   SubstitutedResult);
  }

  const TypeRef *visitProtocolTypeRef(const ProtocolTypeRef *P) {
    return P;
  }

  const TypeRef *
  visitProtocolCompositionTypeRef(const ProtocolCompositionTypeRef *PC) {
    return PC;
  }

  const TypeRef *visitMetatypeTypeRef(const MetatypeTypeRef *M) {
    return MetatypeTypeRef::create(Builder, visit(M->getInstanceType()));
  }

  const TypeRef *
  visitExistentialMetatypeTypeRef(const ExistentialMetatypeTypeRef *EM) {
    assert(EM->getInstanceType()->isConcrete());
    return EM;
  }

  const TypeRef *
  visitGenericTypeParameterTypeRef(const GenericTypeParameterTypeRef *GTP) {
    auto found = Substitutions.find({GTP->getDepth(), GTP->getIndex()});
    assert(found != Substitutions.end());
    assert(found->second->isConcrete());
    return found->second;
  }

  const TypeRef *visitDependentMemberTypeRef(const DependentMemberTypeRef *DM) {
    auto SubstBase = visit(DM->getBase());

    const TypeRef *TypeWitness;

    switch (SubstBase->getKind()) {
    case TypeRefKind::Nominal: {
      auto Nominal = cast<NominalTypeRef>(SubstBase);
      TypeWitness = Builder.getDependentMemberTypeRef(Nominal->getMangledName(), DM);
      break;
    }
    case TypeRefKind::BoundGeneric: {
      auto BG = cast<BoundGenericTypeRef>(SubstBase);
      TypeWitness = Builder.getDependentMemberTypeRef(BG->getMangledName(), DM);
      break;
    }
    default:
      assert(false && "Unknown base type");
    }

    assert(TypeWitness);
    return TypeWitness->subst(Builder, SubstBase->getSubstMap());
  }

  const TypeRef *visitForeignClassTypeRef(const ForeignClassTypeRef *F) {
    return F;
  }

  const TypeRef *visitObjCClassTypeRef(const ObjCClassTypeRef *OC) {
    return OC;
  }

  const TypeRef *visitUnownedStorageTypeRef(const UnownedStorageTypeRef *US) {
    return UnownedStorageTypeRef::create(Builder, visit(US->getType()));
  }

  const TypeRef *visitWeakStorageTypeRef(const WeakStorageTypeRef *WS) {
    return WeakStorageTypeRef::create(Builder, visit(WS->getType()));
  }

  const TypeRef *
  visitUnmanagedStorageTypeRef(const UnmanagedStorageTypeRef *US) {
    return UnmanagedStorageTypeRef::create(Builder, visit(US->getType()));
  }

  const TypeRef *visitOpaqueTypeRef(const OpaqueTypeRef *Op) {
    return Op;
  }
};

const TypeRef *
TypeRef::subst(TypeRefBuilder &Builder, GenericArgumentMap Subs) const {
  const TypeRef *Result = TypeRefSubstitution(Builder, Subs).visit(this);
  assert(Result->isConcrete());
  return Result;
}

