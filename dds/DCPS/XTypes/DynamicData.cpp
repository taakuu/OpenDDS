/*
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "DynamicData.h"

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL

namespace OpenDDS {
namespace XTypes {

DDS::ReturnCode_t DynamicData::get_descriptor(MemberDescriptor& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_descriptor(MemberId id, const MemberDescriptor& value)
{
}

bool DynamicData::equals(const DynamicData& other) const
{
}

MemberId DynamicData::get_member_id_by_name(String name) const
{
}

MemberId DynamicData::get_member_id_at_index(ACE_CDR::ULong) const
{
}

ACE_CDR::ULong DynamicData::get_item_count() const
{
}

DDS::ReturnCode_t DynamicData::clear_all_values()
{
}

DDS::ReturnCode_t DynamicData::clear_nonkey_values()
{
}

DDS::ReturnCode_t DynamicData::clear_value(MemberId id)
{
}

DynamicData DynamicData::loan_value(MemberId id)
{
}

DDS::ReturnCode_t DynamicData::return_loaned_value(const DynamicData& value)
{
}

DynamicData DynamicData::clone() const
{
}

DDS::ReturnCode_t DynamicData::get_int32_value(ACE_CDR::Long& value, MemberId id) const
{
  const OPENDDS_MAP<MemberId, size_t>::const_iterator it = offset_lookup_table_.find(id);
  if (it != offset_lookup_table_.cend()) {
    size_t offset = it->second;
    std::ostringstream err_msg;
    err_msg << "Failed to skip to the member with ID " << id;
    if (!skip("get_int32_value", err_msg.str().c_str(), offset)) {
      return DDS::RETCODE_ERROR;
    }

    if (!(strm_ >> value)) {
      if (DCPS_debug_level >= 10) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                   ACE_TEXT(" Failed to read an int32 member with ID %d\n"), id));
      }
      return DDS::RETCODE_ERROR;
    }

    reset_rpos();
    return DDS::RETCODE_OK;
  }

  DynamicTypeMember_rch member;
  DDS::ReturnCode_t retcode = type_->get_member(member, id);
  if (retcode != DDS::RETCODE_OK) {
    if (DCPS_debug_level >= 10) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                 ACE_TEXT(" Failed to get DynamicTypeMember with ID %d\n"), id));
    }
    return retcode;
  }

  MemberDescriptor member_desc;
  member->get_descriptor(member_desc);

  if (member_desc.type->get_kind() != TK_INT32) {
    if (DCPS_debug_level >= 10) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                 ACE_TEXT(" Member with ID %d is not an int32\n"), id));
    }
    return DDS::RETCODE_ERROR;
  }

  ExtensibilityKind ek = descriptor_.extensibility_kind;
  const Encoding& encoding = ser_.encoding();
  if (encoding.xcdr_version() == Encoding::XCDR_VERSION_2) {
    if (ek == FINAL || ek == APPENDABLE) {
      if (ek == APPENDABLE) {
        size_t dheader = 0;
        if (!strm_.read_delimiter(dheader)) {
          if (DCPS_debug_level >= 10) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                       ACE_TEXT(" Failed to read DHEADER for member ID %d\n"), id));
          }
          return DDS::RETCODE_ERROR;
        }
      }

      // Skip preceding members until reach the requested member.
      ACE_CDR::ULong i = 0;
      while (i < member_desc.index) {
        if (!skip_member_by_index(i)) {
          if (DCPS_debug_level >= 10) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                       ACE_TEXT(" Failed to skip member at index %d\n"), i));
          }
          return DDS::RETCODE_ERROR;
        }
        ++i;
      }
    } else {
      size_t dheader = 0;
      if (!strm_.read_delimiter(dheader)) {
        if (DCPS_debug_level >= 10) {
          ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                     ACE_TEXT(" Failed to read DHEADER for member ID %d\n"), id));
        }
        return DDS::RETCODE_ERROR;
      }

      const size_t end_of_sample = strm_.rpos() + dheader;
      ACE_CDR::ULong member_id;
      size_t member_size;
      while (true) {
        if (strm_.rpos() >= end_of_sample) {
          if (DCPS_debug_level >= 10) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                       ACE_TEXT(" Could not find an int32 member with ID %d\n"), id));
          }
          return DDS::RETCODE_ERROR;
        }

        bool must_understand = false;
        if (!strm_.read_parameter_id(member_id, member_size, must_understand)) {
          if (DCPS_debug_level >= 10) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                       ACE_TEXT(" Failed to read EMHEADER while finding member ID %d\n"), id));
          }
          return DDS::RETCODE_ERROR;
        }

        if (member_id == id) {
          break;
        } else {
          std::ostringstream err_msg;
          err_msg << "Failed to skip a member with ID " << member_id;
          if (!skip("get_int32_value", err_msg.str().c_str(), member_size)) {
            return DDS::RETCODE_ERROR;
          }
        }
      }
    }

    offset_lookup_table_[id] = strm_.rpos() - start_rpos_;
    if (!(strm_ >> value)) {
      if (DCPS_debug_level >= 10) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::get_int32_value -")
                   ACE_TEXT(" Failed to read the int32 member with ID %d\n"), id));
      }
      return DDS::RETCODE_ERROR;
    }
  } else {
    // TODO: Support encodings other than XCDR2?
    return DDS::RETCODE_UNSUPPORTED;
  }

  reset_rpos();
  return DDS::RETCODE_OK;
}

DDS::ReturnCode_t DynamicData::set_int32_value(MemberId id, ACE_CDR::Long value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_uint32_value(ACE_CDR::ULong& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_uint32_value(MemberId id, ACE_CDR::ULong value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_int8_value(ACE_CDR::Int8& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_int8_value(MemberId id, ACE_CDR::Int8 value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_uint8_value(ACE_CDR::UInt8& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_uint8_value(MemberId id, ACE_CDR::UInt8 value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_int16_value(ACE_CDR::Short& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_int16_value(MemberId id, ACE_CDR::Short value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_uint16_value(ACE_CDR::UShort& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_uint16_value(MemberId id, ACE_CDR::UShort value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_int64_value(ACE_CDR::LongLong& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_int64_value(MemberId id, ACE_CDR::LongLong value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_uint64_value(ACE_CDR::ULongLong& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_uint64_value(MemberId id, ACE_CDR::ULongLong value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_float32_value(ACE_CDR::Float& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_float32_value(MemberId id, ACE_CDR::Float value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_float64_value(ACE_CDR::Double& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_float64_value(MemberId id, ACE_CDR::Double value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_float128_value(ACE_CDR::LongDouble& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_float128_value(MemberId id, ACE_CDR::LongDouble value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_char8_value(ACE_CDR::Char& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_char8_value(MemberId id, ACE_CDR::Char value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_char16_value(ACE_CDR::WChar& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_char16_value(MemberId id, ACE_CDR::WChar value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_byte_value(ACE_CDR::Octet& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_byte_value(MemberId id, ACE_CDR::Octet value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_boolean_value(ACE_CDR::Boolean& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_boolean_value(MemberId id, ACE_CDR::Boolean value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_string_value(DCPS::String& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_string_value(MemberId id, DCPS::String value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_wstring_value(DCPS::WString& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_wstring_value(MemberId id, DCPS::WString value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_complex_value(DynamicData& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_complex_value(MemberId id, DynamicData value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_int32_values(Int32Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_int32_values(MemberId id, const Int32Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_uint32_values(UInt32Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_uint32_values(MemberId id, const UInt32Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_int8_values(Int8Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_int8_values(MemberId id, const Int8Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_uint8_values(UInt8Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_uint8_values(MemberId id, const UInt8Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_int16_values(Int16Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_int16_values(MemberId id, const Int16Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_uint16_values(UInt16Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_uint16_values(MemberId id, const UInt16Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_int64_values(Int64Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_int64_values(MemberId id, const Int64Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_uint64_values(UInt64Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_uint64_values(MemberId id, const UInt64Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_float32_values(Float32Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_float32_values(MemberId id, const Float32Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_float64_values(Float64Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_float64_values(MemberId id, const Float64Seq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_float128_values(Float128Seq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_float128_values(MemberId id, const Float128Seq& value);
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_char8_values(CharSeq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_char8_values(MemberId id, const CharSeq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_char16_values(WCharSeq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_char16_values(MemberId id, const WCharSeq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_byte_values(ByteSeq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_byte_values(MemberId id, const ByteSeq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_boolean_values(BooleanSeq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_boolean_values(MemberId id, const BooleanSeq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_string_values(StringSeq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_string_values(MemberId id, const StringSeq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

DDS::ReturnCode_t DynamicData::get_wstring_values(WStringSeq& value, MemberId id) const
{
}

DDS::ReturnCode_t DynamicData::set_wstring_values(MemberId id, const WStringSeq& value)
{
  // TODO: Implement this.
  return DDS::RETCODE_UNSUPPORTED;
}

void DynamicData::reset_rpos()
{
  // TODO(sonndinh): Move the read pointer to the beginning of the stream.
}

bool DynamicData::skip_member_by_index(ACE_CDR::ULong index)
{
  DynamicTypeMember_rch member;
  if (type_.get_member_by_index(dtm, member) != DDS::RETCODE_OK) {
    if (DCPS_debug_level >= 10) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_member_by_index -")
                 ACE_TEXT(" Failed to get DynamicTypeMember for member index %d\n"), index));
    }
    return DDS::RETCODE_ERROR;
  }

  MemberDescriptor member_descriptor;
  member->get_descriptor(member_descriptor);
  DynamicType_rch member_type = member_descriptor.type;

  return skip_member(member_type);
}

bool DynamicData::skip_member(DynamicType_rch member_type)
{
  TypeKind member_kind = member_type->get_kind();
  if (member_kind == TK_ALIAS) {
    member_type = get_base_type(member_type);
    member_kind = member_type->get_kind();
  }

  switch (member_kind) {
  case TK_BOOLEAN:
  case TK_BYTE:
  case TK_INT8:
  case TK_UINT8:
  case TK_CHAR8:
    if (!skip("skip_member", "Failed to skip a member of size 1 byte", 1, 1)) {
      return false;
    }
    break;
  case TK_INT16:
  case TK_UINT16:
  case TK_CHAR16:
    if (!skip("skip_member", "Failed to skip a member of size 2 bytes", 1, 2)) {
      return false;
    }
    break;
  case TK_INT32:
  case TK_UINT32:
  case TK_FLOAT32:
    if (!skip("skip_member", "Failed to skip a member of size 4 bytes", 1, 4)) {
      return false;
    }
    break;
  case TK_INT64:
  case TK_UINT64:
  case TK_FLOAT64:
    if (!skip("skip_member", "Failed to skip a member of size 8 bytes", 1, 8)) {
      return false;
    }
    break;
  case TK_FLOAT128:
    if (!skip("skip_member", "Failed to skip a member of size 16 bytes", 1, 16)) {
      return false;
    }
    break;
  case TK_STRING8:
  case TK_STRING16:
    {
      const char* str_kind = member_kind == TK_STRING8 ? "string" : "wstring";
      ACE_CDR::ULong length;
      if (!(strm_ >> length)) {
        if (DCPS_debug_level >= 10) {
          ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_member -")
                     ACE_TEXT(" Failed to read length of a %C member\n"), str_kind));
        }
        return false;
      }

      std::ostringstream err_msg;
      err_msg << "Failed to skip a " << str_kind << " member";
      if (!skip("skip_member", err_msg.str().c_str(), length)) {
        return false;
      }
      break;
    }
  case TK_ENUM:
  case TK_BITMASK:
    {
      TypeDescriptor member_td;
      member_type->get_descriptor(member_td);
      const ACE_CDR::ULong bit_bound = member_td.bound[0];
      const char* err_msg = member_kind == TK_ENUM ?
        "Failed to skip an enum member" : "Failed to skip a bitmask member";

      if (bit_bound >= 1 && bit_bound <= 8) {
        if (!skip("skip_member", err_msg, 1, 1)) {
          return false;
        }
      } else if (bit_bound >= 9 && bit_bound <= 16) {
        if (!skip("skip_member", err_msg, 1, 2)) {
          return false;
        }
      } else if (bit_bound >= 17 && bit_bound <= 32) {
        if (!skip("skip_member", err_msg, 1, 4)) {
          return false;
        }
      } else if (bit_bound >= 33 && bit_bound <= 64 && member_kind == TK_BITMASK) {
        if (!skip("skip_member", err_msg, 1, 8)) {
          return false;
        }
      } else {
        if (DCPS_debug_level >= 10) {
          ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_member - Found a%C")
                     ACE_TEXT(" member with bit bound %d\n"),
                     member_kind == TK_ENUM ? "n enum" : " bitmask", bit_bound));
        }
        return false;
      }
      break;
    }
  case TK_STRUCTURE:
    return skip_struct_member(member_type);
  case TK_UNION:
    return skip_union_member(member_type);
  case TK_SEQUENCE:
    return skip_sequence_member(member_type);
  case TK_ARRAY:
    return skip_array_member(member_type);
  case TK_MAP:
    return skip_map_member(member_type);
  default:
    {
      const char* kind = 0;
      switch (member_kind) {
      case TK_ANNOTATION:
        // Annotation can't be a member of another type.
        kind = "TK_ANNOTATION";
        break;
      case TK_BITSET:
        // The spec doesn't give enough information on bitset.
        kind = "TK_BITSET";
        break;
      case TK_NONE:
        kind = "TK_NONE";
        break;
      default:
        kind = "Unknown";
        break;
      }

      if (DCPS_debug_level >= 10) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_member -")
                   ACE_TEXT(" Found a member with kind %C\n"), kind));
      }
      return false;
    }
  }

  return true;
}

bool DynamicData::skip_sequence_member(DynamicType_rch seq_type)
{
  TypeDescriptor descriptor;
  seq_type->get_descriptor(descriptor);
  DynamicType_rch elem_type = descriptor.element_type;
  if (elem_type->get_kind() == TK_ALIAS) {
    elem_type = get_base_type(elem_type);
  }

  ACE_CDR::ULong primitive_size = 0;
  if (is_primitive(elem_type, primitive_size)) {
    ACE_CDR::ULong length;
    if (!(strm_ >> length)) {
      if (DCPS_debug_level >= 10) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_sequence_member -")
                   ACE_TEXT(" Failed to deserialize a primitive sequence member\n")));
      }
      return false;
    }

    return skip("skip_sequence_member", "Failed to skip a primitive sequence member",
                length, primitive_size)
  } else {
    return skip_collection_member(TK_SEQUENCE);
  }
}

bool DynamicData::skip_array_member(DynamicType_rch array_type)
{
  TypeDescriptor descriptor;
  array_type->get_descriptor(descriptor);
  DynamicType_rch elem_type = descriptor.element_type;
  if (elem_type->get_kind() == TK_ALIAS) {
    elem_type = get_base_type(elem_type);
  }

  ACE_CDR::ULong primitive_size = 0;
  if (is_primitive(elem_type, primitive_size)) {
    const LBoundSeq& bounds = descriptor.bound;
    ACE_CDR::ULong num_elems = 1;
    for (unsigned i = 0; i < bounds.size(); ++i) {
      num_elems *= bounds[i];
    }

    return skip("skip_array_member", "Failed to skip a primitive array member",
                num_elems, primitive_size)
  } else {
    return skip_collection_member(TK_ARRAY);
  }
}

bool DynamicData::skip_map_member(DynamicType_rch map_type)
{
  TypeDescriptor descriptor;
  map_type->get_descriptor(descriptor);

  DynamcType_rch elem_type = descriptor.element_type;
  if (elem_type->get_kind() == TK_ALIAS) {
    elem_type = get_base_type(elem_type);
  }

  DynamicType_rch key_type = descriptor.key_element_type;
  if (key_type->get_kind() == TK_ALIAS) {
    key_type = get_base_type(key_type);
  }

  ACE_CDR::ULong key_primitive_size = 0, elem_primitive_size = 0;
  if (is_primitive(key_type, key_primitive_size) &&
      is_primitive(elem_type, elem_primitive_size)) {
    ACE_CDR::ULong length;
    if (!(strm_ >> length)) {
      if (DCPS_debug_level >= 10) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_map_member -")
                   ACE_TEXT(" Failed to deserialize length of a primitive map member\n")));
      }
      return false;
    }

    for (unsigned i = 0; i < length; ++i) {
      if (!skip("skip_map_member", "Failed to skip a key of a primitive map member",
                1, key_primitive_size)) {
        return false;
      }
      if (!skip("skip_map_member", "Failed to skip an element of a primitive map member",
                1, elem_primitive_size)) {
        return false;
      }
    }
    return true;
  } else {
    return skip_collection_member(TK_MAP);
  }
}

bool DynamicData::skip_collection_member(TypeKind kind)
{
  const char* kind_str = 0;
  if (kind == TK_SEQUENCE) {
    kind_str = "sequence";
  } else if (kind == TK_ARRAY) {
    kind_str = "array";
  } else if (kind == TK_MAP) {
    kind_str = "map";
  } else {
    return false;
  }

  ACE_CDR::ULong dheader;
  if (!(strm_ >> dheader)) {
    if (DCPS_debug_level >= 10) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_collection_member -")
                 ACE_TEXT(" Failed to deserialize DHEADER of a non-primitive %C member\n"),
                 kind_str));
    }
    return false;
  }

  std::ostringstream err_msg;
  err_msg << "Failed to skip a non-primitive " << kind_str <<  " member";
  return skip("skip_collection_member", err_msg.str().c_str(), dheader);
}

bool DynamicData::skip_struct_member(DynamicType_rch struct_type)
{
  // TODO(sonndinh): The internal data buffer must be shared between the current DynamicData
  // object and the struct_data DynamicData object.
  DynamicData struct_data(strm_, struct_type);
  return struct_data.skip_all();
}

bool DynamicData::skip_union_member(DynamicType_rch union_type)
{
  // TODO(sonndinh)
  DynamicData union_data(strm_, union_type);
  return union_data.skip_all();
}

bool DynamicData::skip_all()
{
  ExtensibilityKind extensibility = descriptor_.extensibility_kind;

  if (extensibility == APPENDABLE || extensibility == MUTABLE) {
    ACE_CdR::ULong dheader;
    if (!(strm_ >> dheader)) {
      if (DCPS_debug_level >= 10) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_all - Failed to read")
                   ACE_TEXT(" DHEADER of the sample\n")));
      }
      return false;
    }
    return skip("skip_all", "Failed to skip the whole sample\n", dheader);

  } else {
    const TypeKind tk = type_->get_kind();
    if (tk == TK_STRUCTURE) {
      const ACE_CDR::ULong member_count = type_->get_member_count();
      for (ACE_CDR::ULong i = 0; i < member_count; ++i) {
        if (!skip_member_by_index(i)) {
          return false;
        }
      }
      return true;
    } else if (tk == TK_UNION) {
      // Read the discriminator.
      DynamicType_rch disc_type = descriptor_.discriminator_type;
      if (disc_type->get_kind() == TK_ALIAS) {
        disc_type = get_base_type(disc_type);
      }
      ACE_CDR::Long label;

      switch (disc_type->get_kind()) {
      case TK_BOOLEAN:
        {
          ACE_CDR::Boolean value;
          strm_ >> ACE_InputCDR::to_boolean(value);
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_BYTE:
        {
          ACE_CDR::Octet value;
          strm_ >> ACE_InputCDR::to_octet(value);
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_CHAR8:
        {
          ACE_CDR::Char value;
          strm_ >> ACE_InputCDR::to_char(value);
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_CHAR16:
        {
          ACE_CDR::WChar value;
          strm_ >> ACE_InputCDR::to_wchar(value);
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_INT8:
        {
          ACE_CDR::Int8 value;
          strm_ >> ACE_InputCDR::to_int8(value);
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_UINT8:
        {
          ACE_CDR::UInt8 value;
          strm_ >> ACE_InputCDR::to_uint8(value);
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_INT16:
        {
          ACE_CDR::Short value;
          strm_ >> value;
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_UINT16:
        {
          ACE_CDR::UShort value;
          strm_ >> value;
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_INT32:
        {
          strm_ >> label;
          break;
        }
      case TK_UINT32:
        {
          ACE_CDR::ULong value;
          strm_ >> value;
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_INT64:
        {
          ACE_CDR::LongLong value;
          strm_ >> value;
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_UINT64:
        {
          ACE_CDR::ULongLong value;
          strm_ >> value;
          label = std::static_cast<ACE_CDR::Long>(value);
          break;
        }
      case TK_ENUM:
        {
          TypeDescriptor disc_td;
          disc_type->get_descriptor(disc_td);
          const ACE_CDR::ULong bit_bound = disc_td.bound[0];

          if (bit_bound >= 1 && bit_bound <= 8) {
            ACE_CDR::Int8 value;
            strm_ >> ACE_InputCDR::to_int8(value);
            label = std::static_cast<ACE_CDR::Long>(value);
          } else if (bit_bound >= 9 && bit_bound <= 16) {
            ACE_CDR::Short value;
            strm_ >> value;
            label = std::static_cast<ACE_CDR::Long>(value);
          } else {
            strm_ >> label;
          }
          break;
        }
      default:
        {
          if (DCPS_debug_level >= 10) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_all - Union type has")
                       ACE_TEXT(" unsupported discriminator type\n")));
          }
          return false;
        }
      }

      // Then, skip the selected member.
      DynamicTypeMembersById members;
      type_->get_all_members(members);

      DynamicTypeMembersById::const_iterator it = members.begin();
      for (; it != members.end(); ++it) {
        MemberDescriptor md;
        it->second->get_descriptor(md);
        const UnionCaseLabelSeq& labels = md.label;
        for (ACE_CDR::ULong i = 0; i < labels.size(); ++i) {
          if (label == labels[i]) {
            return skip_member(md.type)
          }
        }
      }
      if (DCPS_debug_level >= 10) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::skip_all - Not found a member")
                   ACE_TEXT(" of the union type with label %d\n"), label));
      }
      return false;
    }
  }
}

bool DynamicData::skip(const char* func_name, const char* description, size_t n, int size)
{
  if (!strm_.skip(n, size)) {
    if (DCPS_debug_level >= 10) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) DynamicData::%C - %C\n"), func_name, description));
    }
    return false;
  }
  return true;
}

DynamicType_rch DynamicData::get_base_type(DynamicType_rch type) const
{
  if (type->get_kind() != TK_ALIAS) {
    return type;
  }

  TypeDescriptor descriptor;
  type->get_descriptor(descriptor);
  return get_base_type(descriptor.base_type);
}

bool DynamicData::is_primitive(DynamicType_rch type, ACE_CDR::ULong& size) const
{
  size = 0;

  switch (type->get_kind()) {
  case TK_BOOLEAN:
  case TK_BYTE:
  case TK_INT8:
  case TK_UINT8:
  case TK_CHAR8:
    size = 1;
    break;
  case TK_INT16:
  case TK_UINT16:
  case TK_CHAR16:
    size = 2;
    break;
  case TK_INT32:
  case TK_UINT32:
  case TK_FLOAT32:
    size = 4;
    break;
  case TK_INT64:
  case TK_UINT64:
  case TK_FLOAT64:
    size = 8;
    break;
  case TK_FLOAT128:
    size = 16;
    break;
  default:
    return false;
  }
  return true;
}

} // namespace XTypes
} // namespace OpenDDS

OPENDDS_END_VERSIONED_NAMESPACE_DECL
