/*
TODO Untested Types:
Map
Annotation
Bitmask
Bitset
*/

#include "CompleteToDynamicTypeTypeSupportImpl.h"

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/XTypes/TypeLookupService.h>
#include <dds/DCPS/XTypes/TypeObject.h>
#include <dds/DCPS/XTypes/TypeDescriptor.h>
#include <dds/DCPS/XTypes/MemberDescriptor.h>
#include <dds/DCPS/XTypes/DynamicType.h>
#include <dds/DCPS/XTypes/DynamicTypeMember.h>

#include <gtest/gtest.h>

using namespace OpenDDS;

XTypes::TypeLookupService_rch tls = DCPS::make_rch<XTypes::TypeLookupService>();

template<typename T>
void test_conversion(const XTypes::DynamicType_rch& expected_dynamic_type)
{
  const XTypes::TypeIdentifier& com_ti = DCPS::getCompleteTypeIdentifier<T>();
  const XTypes::TypeMap& com_map = DCPS::getCompleteTypeMap<T>();
  XTypes::TypeMap::const_iterator pos = com_map.find(com_ti);
  EXPECT_TRUE(pos != com_map.end());
  const XTypes::TypeObject& com_to = pos->second;
  XTypes::DynamicType_rch converted_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeLookupService::DynamicTypeMap dtm;
  converted_dt = tls->complete_to_dynamic(com_to.complete, dtm);
  EXPECT_EQ(*expected_dynamic_type, *converted_dt);
}

TEST(CompleteToDynamicType, MyInnerStruct)
{
  XTypes::DynamicType_rch expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor expected_td;
  expected_td.kind = XTypes::TK_STRUCTURE;
  expected_td.name = "::MyMod::MyInnerStruct";
  expected_td.bound.length(0);
  expected_td.extensibility_kind = XTypes::MUTABLE;
  expected_td.is_nested = false;
  XTypes::DynamicTypeMember_rch expected_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor expected_md;
  expected_dtm->set_parent(expected_dt);
  expected_md.name = "l";
  expected_md.id = false;
  expected_md.index = false;
  expected_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicType_rch long_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor long_td;
  long_td.kind = XTypes::TK_INT32;
  long_td.bound.length(0);
  long_td.name = "Int32";
  expected_md.type = long_expected_dt;
  expected_dt->set_descriptor(expected_td);
  expected_dtm->set_descriptor(expected_md);
  long_expected_dt->set_descriptor(long_td);
  expected_dt->insert_dynamic_member(expected_dtm);
  test_conversion<DCPS::MyMod_MyInnerStruct_xtag>(expected_dt);
}

TEST(CompleteToDynamicType, MyOuterStruct)
{
  XTypes::DynamicType_rch expected_outer_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor outer_td;
  outer_td.kind = XTypes::TK_STRUCTURE;
  outer_td.name = "::MyMod::MyOuterStruct";
  outer_td.bound.length(0);
  outer_td.extensibility_kind = XTypes::APPENDABLE;
  outer_td.is_nested = 0;
  XTypes::DynamicTypeMember_rch expected_outer_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor expected_outer_md;
  expected_outer_dtm->set_parent(expected_outer_dt);
  expected_outer_md.name = "ms";
  expected_outer_md.id = 0;
  expected_outer_md.index = 0;
  expected_outer_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicType_rch expected_inner_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor inner_td;
  inner_td.kind = XTypes::TK_STRUCTURE;
  inner_td.name = "::MyMod::MyInnerStruct";
  inner_td.bound.length(0);
  inner_td.extensibility_kind = XTypes::MUTABLE;
  inner_td.is_nested = 0;
  XTypes::DynamicTypeMember_rch expected_inner_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor expected_inner_md;
  expected_inner_dtm->set_parent(expected_inner_dt);
  expected_inner_md.name = "l";
  expected_inner_md.id = 0;
  expected_inner_md.index = 0;
  expected_inner_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicType_rch long_expected_inner_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor long_td;
  long_td.kind = XTypes::TK_INT32;
  long_td.bound.length(0);
  long_td.name = "Int32";
  expected_inner_md.type = long_expected_inner_dt;
  expected_outer_md.type = expected_inner_dt;
  expected_outer_dt->set_descriptor(outer_td);
  expected_outer_dtm->set_descriptor(expected_outer_md);
  expected_inner_dt->set_descriptor(inner_td);
  expected_inner_dtm->set_descriptor(expected_inner_md);
  long_expected_inner_dt->set_descriptor(long_td);
  expected_inner_dt->insert_dynamic_member(expected_inner_dtm);
  expected_outer_dt->insert_dynamic_member(expected_outer_dtm);
  test_conversion<DCPS::MyMod_MyOuterStruct_xtag>(expected_outer_dt);
}

TEST(CompleteToDynamicType, MyAliasStruct)
{
  XTypes::DynamicType_rch expected_alias_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor alias_td;
  alias_td.kind = XTypes::TK_ALIAS;
  alias_td.name = "::MyMod::MyAliasStruct";
  alias_td.bound.length(0);
  alias_td.extensibility_kind = XTypes::FINAL;
  alias_td.is_nested = 0;
  XTypes::DynamicType_rch expected_outer_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor outer_td;
  outer_td.kind = XTypes::TK_STRUCTURE;
  outer_td.name = "::MyMod::MyOuterStruct";
  outer_td.bound.length(0);
  outer_td.extensibility_kind = XTypes::APPENDABLE;
  outer_td.is_nested = 0;
  XTypes::DynamicTypeMember_rch expected_outer_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor expected_outer_md;
  expected_outer_dtm->set_parent(expected_outer_dt);
  expected_outer_md.name = "ms";
  expected_outer_md.id = 0;
  expected_outer_md.index = 0;
  expected_outer_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicType_rch expected_inner_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor inner_td;
  inner_td.kind = XTypes::TK_STRUCTURE;
  inner_td.name = "::MyMod::MyInnerStruct";
  inner_td.bound.length(0);
  inner_td.extensibility_kind = XTypes::MUTABLE;
  inner_td.is_nested = 0;
  XTypes::DynamicTypeMember_rch expected_inner_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor expected_inner_md;
  expected_inner_dtm->set_parent(expected_inner_dt);
  expected_inner_md.name = "l";
  expected_inner_md.id = 0;
  expected_inner_md.index = 0;
  expected_inner_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicType_rch long_expected_inner_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor long_td;
  long_td.kind = XTypes::TK_INT32;
  long_td.bound.length(0);
  long_td.name = "Int32";
  expected_inner_md.type = long_expected_inner_dt;
  expected_outer_md.type = expected_inner_dt;
  alias_td.base_type = expected_outer_dt;
  expected_alias_dt->set_descriptor(alias_td);
  expected_outer_dt->set_descriptor(outer_td);
  expected_outer_dtm->set_descriptor(expected_outer_md);
  expected_inner_dt->set_descriptor(inner_td);
  expected_inner_dtm->set_descriptor(expected_inner_md);
  long_expected_inner_dt->set_descriptor(long_td);
  expected_inner_dt->insert_dynamic_member(expected_inner_dtm);
  expected_outer_dt->insert_dynamic_member(expected_outer_dtm);
  test_conversion<DCPS::MyMod_MyAliasStruct_xtag>(expected_alias_dt);
}

TEST(CompleteToDynamicType, PrimitiveKind)
{
  XTypes::DynamicType_rch expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor td;
  td.kind = XTypes::TK_ENUM;
  td.name = "::MyMod::PrimitiveKind";
  td.bound.length(0);
  td.extensibility_kind = XTypes::FINAL;
  td.is_nested = 0;
  XTypes::DynamicTypeMember_rch first_expected_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor first_expected_md;
  first_expected_dtm->set_parent(expected_dt);
  first_expected_md.name = "TK_INT32";
  first_expected_md.id = UINT32_MAX;
  first_expected_md.index = 0;
  first_expected_md.is_default_label = 1;
  first_expected_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicTypeMember_rch second_expected_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor second_expected_md;
  second_expected_dtm->set_parent(expected_dt);
  second_expected_md.name = "TK_CHAR8";
  second_expected_md.id = UINT32_MAX;
  second_expected_md.index = 1;
  second_expected_md.try_construct_kind = XTypes::DISCARD;
  first_expected_md.type = expected_dt;
  second_expected_md.type = expected_dt;
  expected_dt->set_descriptor(td);
  first_expected_dtm->set_descriptor(first_expected_md);
  second_expected_dtm->set_descriptor(second_expected_md);
  expected_dt->insert_dynamic_member(first_expected_dtm);
  expected_dt->insert_dynamic_member(second_expected_dtm);
  test_conversion<DCPS::MyMod_PrimitiveKind_xtag>(expected_dt);
}

TEST(CompleteToDynamicType, MyUnion)
{
  XTypes::DynamicType_rch expected_union_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor td;
  td.kind = XTypes::TK_UNION;
  td.name = "::MyMod::MyUnion";
  td.bound.length(0);
  td.extensibility_kind = XTypes::APPENDABLE;
  td.is_nested = 0;
  XTypes::DynamicTypeMember_rch long_expected_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor long_expected_md;
  long_expected_dtm->set_parent(expected_union_dt);
  long_expected_md.name = "l";
  long_expected_md.id = 0;
  long_expected_md.index = 0;
  long_expected_md.try_construct_kind = XTypes::DISCARD;
  long_expected_md.label.length(1);
  long_expected_md.label[0] = 0;
  long_expected_md.is_default_label = 0;
  XTypes::DynamicTypeMember_rch char_expected_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor char_expected_md;
  char_expected_dtm->set_parent(expected_union_dt);
  char_expected_md.name = "c";
  char_expected_md.id = 1;
  char_expected_md.index = 1;
  char_expected_md.try_construct_kind = XTypes::DISCARD;
  char_expected_md.label.length(1);
  char_expected_md.label[0] = 1;
  char_expected_md.is_default_label = 0;
  XTypes::DynamicTypeMember_rch short_expected_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor short_expected_md;
  short_expected_dtm->set_parent(expected_union_dt);
  short_expected_md.name = "s";
  short_expected_md.id = 2;
  short_expected_md.index = 2;
  short_expected_md.try_construct_kind = XTypes::DISCARD;
  short_expected_md.is_default_label = 1;
  XTypes::DynamicType_rch long_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor long_td;
  long_td.kind = XTypes::TK_INT32;
  long_td.bound.length(0);
  long_td.extensibility_kind = XTypes::FINAL;
  long_td.name = "Int32";
  long_expected_md.type = long_expected_dt;
  XTypes::DynamicType_rch char_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor char_td;
  char_td.kind = XTypes::TK_CHAR8;
  char_td.bound.length(0);
  char_td.extensibility_kind = XTypes::FINAL;
  char_td.name = "Char8";
  char_expected_md.type = char_expected_dt;
  XTypes::DynamicType_rch short_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor short_td;
  short_td.kind = XTypes::TK_INT16;
  short_td.bound.length(0);
  short_td.extensibility_kind = XTypes::FINAL;
  short_td.name = "Int16";
  short_expected_md.type = short_expected_dt;
  XTypes::DynamicType_rch expected_enum_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor enum_td;
  enum_td.kind = XTypes::TK_ENUM;
  enum_td.name = "::MyMod::PrimitiveKind";
  enum_td.bound.length(0);
  enum_td.extensibility_kind = XTypes::FINAL;
  enum_td.is_nested = 0;
  XTypes::DynamicTypeMember_rch first_expected_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor first_expected_md;
  first_expected_dtm->set_parent(expected_enum_dt);
  first_expected_md.name = "TK_INT32";
  first_expected_md.id = UINT32_MAX;
  first_expected_md.index = 0;
  first_expected_md.is_default_label = 1;
  first_expected_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicTypeMember_rch second_expected_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor second_expected_md;
  second_expected_dtm->set_parent(expected_enum_dt);
  second_expected_md.name = "TK_CHAR8";
  second_expected_md.id = UINT32_MAX;
  second_expected_md.index = 1;
  second_expected_md.try_construct_kind = XTypes::DISCARD;
  first_expected_md.type = expected_enum_dt;
  second_expected_md.type = expected_enum_dt;
  long_expected_md.type = long_expected_dt;
  char_expected_md.type = char_expected_dt;
  short_expected_md.type = short_expected_dt;
  td.discriminator_type = expected_enum_dt;
  expected_union_dt->set_descriptor(td);
  long_expected_dtm->set_descriptor(long_expected_md);
  char_expected_dtm->set_descriptor(char_expected_md);
  short_expected_dtm->set_descriptor(short_expected_md);
  long_expected_dt->set_descriptor(long_td);
  char_expected_dt->set_descriptor(char_td);
  short_expected_dt->set_descriptor(short_td);
  expected_enum_dt->set_descriptor(enum_td);
  first_expected_dtm->set_descriptor(first_expected_md);
  second_expected_dtm->set_descriptor(second_expected_md);
  expected_enum_dt->insert_dynamic_member(first_expected_dtm);
  expected_enum_dt->insert_dynamic_member(second_expected_dtm);
  expected_union_dt->insert_dynamic_member(long_expected_dtm);
  expected_union_dt->insert_dynamic_member(char_expected_dtm);
  expected_union_dt->insert_dynamic_member(short_expected_dtm);
  test_conversion<DCPS::MyMod_MyUnion_xtag>(expected_union_dt);
}

TEST(CompleteToDynamicType, MyInnerArray)
{
  XTypes::DynamicType_rch alias_inner_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor alias_inner_td;
  alias_inner_td.kind = XTypes::TK_ALIAS;
  alias_inner_td.name = "::MyMod::MyInnerArray";
  alias_inner_td.bound.length(0);
  alias_inner_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch inner_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor inner_td;
  inner_td.kind = XTypes::TK_ARRAY;
  inner_td.name = "ArraySmall";
  inner_td.bound.length(1);
  inner_td.bound[0] = 2;
  inner_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch long_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor long_td;
  long_td.kind = XTypes::TK_INT32;
  long_td.bound.length(0);
  long_td.name = "Int32";
  long_td.extensibility_kind = XTypes::FINAL;
  inner_td.element_type = long_expected_dt;
  alias_inner_td.base_type = inner_expected_dt;
  alias_inner_expected_dt->set_descriptor(alias_inner_td);
  inner_expected_dt->set_descriptor(inner_td);
  long_expected_dt->set_descriptor(long_td);
  test_conversion<DCPS::MyMod_MyInnerArray_xtag>(alias_inner_expected_dt);
}

TEST(CompleteToDynamicType, MyOuterArray)
{
  XTypes::DynamicType_rch alias_outer_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor alias_outer_td;
  alias_outer_td.kind = XTypes::TK_ALIAS;
  alias_outer_td.name = "::MyMod::MyOuterArray";
  alias_outer_td.bound.length(0);
  alias_outer_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch outer_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor outer_td;
  outer_td.kind = XTypes::TK_ARRAY;
  outer_td.name = "ArraySmall";
  outer_td.bound.length(2);
  outer_td.bound[0] = 3;
  outer_td.bound[1] = 2;
  outer_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch alias_inner_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor alias_inner_td;
  alias_inner_td.kind = XTypes::TK_ALIAS;
  alias_inner_td.name = "::MyMod::MyInnerArray";
  alias_inner_td.bound.length(0);
  alias_inner_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch inner_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor inner_td;
  inner_td.kind = XTypes::TK_ARRAY;
  inner_td.name = "ArraySmall";
  inner_td.bound.length(1);
  inner_td.bound[0] = 2;
  inner_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch long_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor long_td;
  long_td.kind = XTypes::TK_INT32;
  long_td.bound.length(0);
  long_td.name = "Int32";
  long_td.extensibility_kind = XTypes::FINAL;
  outer_td.element_type = alias_inner_expected_dt;
  alias_outer_td.base_type = outer_expected_dt;
  inner_td.element_type = long_expected_dt;
  alias_inner_td.base_type = inner_expected_dt;
  alias_outer_expected_dt->set_descriptor(alias_outer_td);
  outer_expected_dt->set_descriptor(outer_td);
  alias_inner_expected_dt->set_descriptor(alias_inner_td);
  inner_expected_dt->set_descriptor(inner_td);
  long_expected_dt->set_descriptor(long_td);
  test_conversion<DCPS::MyMod_MyOuterArray_xtag>(alias_outer_expected_dt);
}

TEST(CompleteToDynamicType, MySeq)
{
  XTypes::DynamicType_rch alias_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor alias_td;
  alias_td.kind = XTypes::TK_ALIAS;
  alias_td.name = "::MyMod::MySeq";
  alias_td.bound.length(0);
  alias_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor td;
  td.kind = XTypes::TK_SEQUENCE;
  td.name = "SequenceLarge";
  td.bound.length(1);
  td.bound[0] = UINT32_MAX;
  td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch long_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor long_td;
  long_td.kind = XTypes::TK_INT32;
  long_td.bound.length(0);
  long_td.name = "Int32";
  long_td.extensibility_kind = XTypes::FINAL;
  td.element_type = long_expected_dt;
  alias_td.base_type = expected_dt;
  alias_expected_dt->set_descriptor(alias_td);
  expected_dt->set_descriptor(td);
  long_expected_dt->set_descriptor(long_td);
  test_conversion<DCPS::MyMod_MySeq_xtag>(alias_expected_dt);
}

TEST(CompleteToDynamicType, MyAnonStruct)
{
  XTypes::DynamicType_rch expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor td;
  td.kind = XTypes::TK_STRUCTURE;
  td.name = "::MyMod::MyAnonStruct";
  td.bound.length(0);
  td.extensibility_kind = XTypes::APPENDABLE;
  td.is_nested = 0;
  XTypes::DynamicTypeMember_rch sequence_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor sequence_md;
  sequence_dtm->set_parent(expected_dt);
  sequence_md.name = "lseq";
  sequence_md.id = 0;
  sequence_md.index = 0;
  sequence_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicTypeMember_rch array_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor array_md;
  array_dtm->set_parent(expected_dt);
  array_md.name = "larr";
  array_md.id = 1;
  array_md.index = 1;
  array_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicType_rch sequence_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor sequence_td;
  sequence_td.kind = XTypes::TK_SEQUENCE;
  sequence_td.name = "SequenceSmall";
  sequence_td.bound.length(1);
  sequence_td.bound[0] = 5;
  sequence_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch array_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor array_td;
  array_td.kind = XTypes::TK_ARRAY;
  array_td.name = "ArraySmall";
  array_td.bound.length(1);
  array_td.bound[0] = 3;
  array_td.extensibility_kind = XTypes::FINAL;
  XTypes::DynamicType_rch long_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor long_td;
  long_td.kind = XTypes::TK_INT32;
  long_td.bound.length(0);
  long_td.name = "Int32";
  long_td.extensibility_kind = XTypes::FINAL;
  sequence_td.element_type = long_expected_dt;
  array_td.element_type = long_expected_dt;
  sequence_md.type = sequence_expected_dt;
  array_md.type = array_expected_dt;
  expected_dt->set_descriptor(td);
  sequence_dtm->set_descriptor(sequence_md);
  array_dtm->set_descriptor(array_md);
  sequence_expected_dt->set_descriptor(sequence_td);
  array_expected_dt->set_descriptor(array_td);
  long_expected_dt->set_descriptor(long_td);
  expected_dt->insert_dynamic_member(sequence_dtm);
  expected_dt->insert_dynamic_member(array_dtm);
  test_conversion<DCPS::MyMod_MyAnonStruct_xtag>(expected_dt);
}

TEST(CompleteToDynamicType, CircularStruct)
{
  XTypes::DynamicType_rch struct_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor struct_td;
  struct_td.kind = XTypes::TK_STRUCTURE;
  struct_td.name = "::MyMod::CircularStruct";
  struct_td.bound.length(0);
  struct_td.extensibility_kind = XTypes::MUTABLE;
  struct_td.is_nested = 0;
  XTypes::DynamicTypeMember_rch struct_seq_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor struct_seq_md;
  struct_seq_dtm->set_parent(struct_expected_dt);
  struct_seq_md.name = "circular_struct2_seq";
  struct_seq_md.id = 0;
  struct_seq_md.index = 0;
  struct_seq_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicType_rch struct_seq_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor struct_seq_td;
  struct_seq_td.kind = XTypes::TK_SEQUENCE;
  struct_seq_td.bound.length(1);
  struct_seq_td.bound[0] = UINT32_MAX;
  struct_seq_td.name = "";
  XTypes::DynamicType_rch struct2_expected_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor struct2_td;
  struct2_td.kind = XTypes::TK_STRUCTURE;
  struct2_td.name = "::MyMod::CircularStruct2";
  struct2_td.bound.length(0);
  struct2_td.extensibility_kind = XTypes::MUTABLE;
  struct2_td.is_nested = 0;
  XTypes::DynamicTypeMember_rch struct2_seq_dtm = DCPS::make_rch<XTypes::DynamicTypeMember>();
  XTypes::MemberDescriptor struct2_seq_md;
  struct2_seq_dtm->set_parent(struct2_expected_dt);
  struct2_seq_md.name = "circular_struct_seq";
  struct2_seq_md.id = 0;
  struct2_seq_md.index = 0;
  struct2_seq_md.try_construct_kind = XTypes::DISCARD;
  XTypes::DynamicType_rch struct2_seq_dt = DCPS::make_rch<XTypes::DynamicType>();
  XTypes::TypeDescriptor struct2_seq_td;
  struct2_seq_td.kind = XTypes::TK_SEQUENCE;
  struct2_seq_td.bound.length(1);
  struct2_seq_td.bound[0] = UINT32_MAX;
  struct2_seq_td.name = "";

  struct2_seq_md.type = struct2_seq_dt;
  struct2_seq_td.element_type = struct_expected_dt;

  struct_seq_md.type = struct_seq_dt;
  struct_seq_td.element_type = struct2_expected_dt;
  struct_expected_dt->set_descriptor(struct_td);
  struct_seq_dtm->set_descriptor(struct_seq_md);
  struct_seq_dt->set_descriptor(struct_seq_td);
  struct2_expected_dt->set_descriptor(struct2_td);
  struct2_seq_dtm->set_descriptor(struct2_seq_md);
  struct2_seq_dt->set_descriptor(struct2_seq_td);
  struct_expected_dt->insert_dynamic_member(struct_seq_dtm);
  struct2_expected_dt->insert_dynamic_member(struct2_seq_dtm);
  test_conversion<DCPS::MyMod_CircularStruct_xtag>(struct_expected_dt);
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  XTypes::TypeIdentifierPairSeq tid_pairs;

  XTypes::TypeIdentifierPair my_inner_struct_tids;
  my_inner_struct_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_MyInnerStruct_xtag>();
  my_inner_struct_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_MyInnerStruct_xtag>();
  tid_pairs.append(my_inner_struct_tids);

  XTypes::TypeIdentifierPair my_outer_struct_tids;
  my_outer_struct_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_MyOuterStruct_xtag>();
  my_outer_struct_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_MyOuterStruct_xtag>();
  tid_pairs.append(my_outer_struct_tids);

  XTypes::TypeIdentifierPair my_alias_struct_tids;
  my_alias_struct_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_MyAliasStruct_xtag>();
  my_alias_struct_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_MyAliasStruct_xtag>();
  tid_pairs.append(my_alias_struct_tids);

  XTypes::TypeIdentifierPair enum_tids;
  enum_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_PrimitiveKind_xtag>();
  enum_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_PrimitiveKind_xtag>();
  tid_pairs.append(enum_tids);

  XTypes::TypeIdentifierPair union_tids;
  union_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_MyUnion_xtag>();
  union_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_MyUnion_xtag>();
  tid_pairs.append(union_tids);

  XTypes::TypeIdentifierPair inner_array_tids;
  inner_array_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_MyInnerArray_xtag>();
  inner_array_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_MyInnerArray_xtag>();
  tid_pairs.append(inner_array_tids);

  XTypes::TypeIdentifierPair outer_array_tids;
  outer_array_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_MyOuterArray_xtag>();
  outer_array_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_MyOuterArray_xtag>();
  tid_pairs.append(outer_array_tids);

  XTypes::TypeIdentifierPair sequence_tids;
  sequence_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_MySeq_xtag>();
  sequence_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_MySeq_xtag>();
  tid_pairs.append(sequence_tids);

  XTypes::TypeIdentifierPair anon_struct_tids;
  anon_struct_tids.type_identifier1 = DCPS::getCompleteTypeIdentifier<DCPS::MyMod_MyAnonStruct_xtag>();
  anon_struct_tids.type_identifier2 = DCPS::getMinimalTypeIdentifier<DCPS::MyMod_MyAnonStruct_xtag>();
  tid_pairs.append(anon_struct_tids);

  tls->update_type_identifier_map(tid_pairs);

  MyMod::MyInnerStructTypeSupportImpl inner_typesupport;
  MyMod::MyOuterStructTypeSupportImpl outer_typesupport;
  MyMod::MyUnionTypeSupportImpl union_typesupport;
  MyMod::MyAnonStructTypeSupportImpl anon_typesupport;
  inner_typesupport.add_types(tls);
  outer_typesupport.add_types(tls);
  union_typesupport.add_types(tls);
  anon_typesupport.add_types(tls);

  return RUN_ALL_TESTS();
}
