// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: header.proto

#ifndef PROTOBUF_header_2eproto__INCLUDED
#define PROTOBUF_header_2eproto__INCLUDED

#include <string>

#include "google/protobuf/stubs/common.h"

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/message.h"
#include "google/protobuf/repeated_field.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/generated_enum_reflection.h"
#include "google/protobuf/unknown_field_set.h"
// @@protoc_insertion_point(includes)

namespace header {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_header_2eproto();
void protobuf_AssignDesc_header_2eproto();
void protobuf_ShutdownFile_header_2eproto();

class signal_header;

enum signal_group_type {
  SIGNALE_GROUP_TYPE_SYS = 0,
  SIGNALE_GROUP_TYPE_MESSAGE = 1,
  SIGNALE_GROUP_TYPE_INVITE = 3,
  SIGNALE_GROUP_TYPE_RPC = 4,
  SIGNALE_GROUP_TYPE_ADDRESS_BOOK = 5,
  SIGNALE_GROUP_TYPE_REMOTE_CONTROL = 6
};
bool signal_group_type_IsValid(int value);
const signal_group_type signal_group_type_MIN = SIGNALE_GROUP_TYPE_SYS;
const signal_group_type signal_group_type_MAX = SIGNALE_GROUP_TYPE_REMOTE_CONTROL;
const int signal_group_type_ARRAYSIZE = signal_group_type_MAX + 1;

const ::google::protobuf::EnumDescriptor* signal_group_type_descriptor();
inline const ::std::string& signal_group_type_Name(signal_group_type value) {
  return ::google::protobuf::internal::NameOfEnum(
    signal_group_type_descriptor(), value);
}
inline bool signal_group_type_Parse(
    const ::std::string& name, signal_group_type* value) {
  return ::google::protobuf::internal::ParseNamedEnum<signal_group_type>(
    signal_group_type_descriptor(), name, value);
}
enum signal_cmd_type {
  SIGNALE_CMD_TYPE_INVITE = 0,
  SIGNALE_CMD_TYPE_INVITE_CANCEL = 1,
  SIGNALE_CMD_TYPE_INVITE_RINGING = 2,
  SIGNALE_CMD_TYPE_INVITE_OK = 3,
  SIGNALE_CMD_TYPE_INVITE_REJECT = 4,
  SIGNALE_CMD_TYPE_INVITE_TIMEOUT = 5,
  SIGNALE_CMD_TYPE_INVITE_BYE = 6,
  SIGNALE_CMD_TYPE_RC_MOUSE_EVENTS = 32,
  SIGNALE_CMD_TYPE_RC_KEYBOARD_EVENTS = 33,
  SIGNALE_CMD_TYPE_MESSAGE_DATA = 48,
  SIGNALE_CMD_TYPE_MESSAGE_RECEIVED = 49,
  SIGNALE_CMD_TYPE_MESSAGE_READ = 50,
  SIGNALE_CMD_TYPE_MESSAGE_RECALLED = 51,
  SIGNALE_CMD_TYPE_RPC_CALL_REQUEST = 64,
  SIGNALE_CMD_TYPE_RPC_CALL_RESPONSE = 65,
  SIGNALE_CMD_TYPE_RPC_NOTIFICATION = 66
};
bool signal_cmd_type_IsValid(int value);
const signal_cmd_type signal_cmd_type_MIN = SIGNALE_CMD_TYPE_INVITE;
const signal_cmd_type signal_cmd_type_MAX = SIGNALE_CMD_TYPE_RPC_NOTIFICATION;
const int signal_cmd_type_ARRAYSIZE = signal_cmd_type_MAX + 1;

const ::google::protobuf::EnumDescriptor* signal_cmd_type_descriptor();
inline const ::std::string& signal_cmd_type_Name(signal_cmd_type value) {
  return ::google::protobuf::internal::NameOfEnum(
    signal_cmd_type_descriptor(), value);
}
inline bool signal_cmd_type_Parse(
    const ::std::string& name, signal_cmd_type* value) {
  return ::google::protobuf::internal::ParseNamedEnum<signal_cmd_type>(
    signal_cmd_type_descriptor(), name, value);
}
// ===================================================================

class signal_header : public ::google::protobuf::Message {
 public:
  signal_header();
  virtual ~signal_header();

  signal_header(const signal_header& from);

  inline signal_header& operator=(const signal_header& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const signal_header& default_instance();

  void Swap(signal_header* other);

  // implements Message ----------------------------------------------

  signal_header* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const signal_header& from);
  void MergeFrom(const signal_header& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string version = 1;
  inline bool has_version() const;
  inline void clear_version();
  static const int kVersionFieldNumber = 1;
  inline const ::std::string& version() const;
  inline void set_version(const ::std::string& value);
  inline void set_version(const char* value);
  inline void set_version(const char* value, size_t size);
  inline ::std::string* mutable_version();
  inline ::std::string* release_version();
  inline void set_allocated_version(::std::string* version);

  // required .header.signal_group_type group_type = 2;
  inline bool has_group_type() const;
  inline void clear_group_type();
  static const int kGroupTypeFieldNumber = 2;
  inline ::header::signal_group_type group_type() const;
  inline void set_group_type(::header::signal_group_type value);

  // required .header.signal_cmd_type cmd_type = 3;
  inline bool has_cmd_type() const;
  inline void clear_cmd_type();
  static const int kCmdTypeFieldNumber = 3;
  inline ::header::signal_cmd_type cmd_type() const;
  inline void set_cmd_type(::header::signal_cmd_type value);

  // required uint32 timestamp = 4;
  inline bool has_timestamp() const;
  inline void clear_timestamp();
  static const int kTimestampFieldNumber = 4;
  inline ::google::protobuf::uint32 timestamp() const;
  inline void set_timestamp(::google::protobuf::uint32 value);

  // required string from = 5;
  inline bool has_from() const;
  inline void clear_from();
  static const int kFromFieldNumber = 5;
  inline const ::std::string& from() const;
  inline void set_from(const ::std::string& value);
  inline void set_from(const char* value);
  inline void set_from(const char* value, size_t size);
  inline ::std::string* mutable_from();
  inline ::std::string* release_from();
  inline void set_allocated_from(::std::string* from);

  // required string to = 6;
  inline bool has_to() const;
  inline void clear_to();
  static const int kToFieldNumber = 6;
  inline const ::std::string& to() const;
  inline void set_to(const ::std::string& value);
  inline void set_to(const char* value);
  inline void set_to(const char* value, size_t size);
  inline ::std::string* mutable_to();
  inline ::std::string* release_to();
  inline void set_allocated_to(::std::string* to);

  // required bytes content = 7;
  inline bool has_content() const;
  inline void clear_content();
  static const int kContentFieldNumber = 7;
  inline const ::std::string& content() const;
  inline void set_content(const ::std::string& value);
  inline void set_content(const char* value);
  inline void set_content(const void* value, size_t size);
  inline ::std::string* mutable_content();
  inline ::std::string* release_content();
  inline void set_allocated_content(::std::string* content);

  // @@protoc_insertion_point(class_scope:header.signal_header)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_group_type();
  inline void clear_has_group_type();
  inline void set_has_cmd_type();
  inline void clear_has_cmd_type();
  inline void set_has_timestamp();
  inline void clear_has_timestamp();
  inline void set_has_from();
  inline void clear_has_from();
  inline void set_has_to();
  inline void clear_has_to();
  inline void set_has_content();
  inline void clear_has_content();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* version_;
  int group_type_;
  int cmd_type_;
  ::std::string* from_;
  ::std::string* to_;
  ::std::string* content_;
  ::google::protobuf::uint32 timestamp_;
  friend void  protobuf_AddDesc_header_2eproto();
  friend void protobuf_AssignDesc_header_2eproto();
  friend void protobuf_ShutdownFile_header_2eproto();

  void InitAsDefaultInstance();
  static signal_header* default_instance_;
};
// ===================================================================


// ===================================================================

// signal_header

// required string version = 1;
inline bool signal_header::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void signal_header::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void signal_header::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void signal_header::clear_version() {
  if (version_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_->clear();
  }
  clear_has_version();
}
inline const ::std::string& signal_header::version() const {
  // @@protoc_insertion_point(field_get:header.signal_header.version)
  return *version_;
}
inline void signal_header::set_version(const ::std::string& value) {
  set_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_ = new ::std::string;
  }
  version_->assign(value);
  // @@protoc_insertion_point(field_set:header.signal_header.version)
}
inline void signal_header::set_version(const char* value) {
  set_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_ = new ::std::string;
  }
  version_->assign(value);
  // @@protoc_insertion_point(field_set_char:header.signal_header.version)
}
inline void signal_header::set_version(const char* value, size_t size) {
  set_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_ = new ::std::string;
  }
  version_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:header.signal_header.version)
}
inline ::std::string* signal_header::mutable_version() {
  set_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:header.signal_header.version)
  return version_;
}
inline ::std::string* signal_header::release_version() {
  clear_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = version_;
    version_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void signal_header::set_allocated_version(::std::string* version) {
  if (version_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete version_;
  }
  if (version) {
    set_has_version();
    version_ = version;
  } else {
    clear_has_version();
    version_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:header.signal_header.version)
}

// required .header.signal_group_type group_type = 2;
inline bool signal_header::has_group_type() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void signal_header::set_has_group_type() {
  _has_bits_[0] |= 0x00000002u;
}
inline void signal_header::clear_has_group_type() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void signal_header::clear_group_type() {
  group_type_ = 0;
  clear_has_group_type();
}
inline ::header::signal_group_type signal_header::group_type() const {
  // @@protoc_insertion_point(field_get:header.signal_header.group_type)
  return static_cast< ::header::signal_group_type >(group_type_);
}
inline void signal_header::set_group_type(::header::signal_group_type value) {
  assert(::header::signal_group_type_IsValid(value));
  set_has_group_type();
  group_type_ = value;
  // @@protoc_insertion_point(field_set:header.signal_header.group_type)
}

// required .header.signal_cmd_type cmd_type = 3;
inline bool signal_header::has_cmd_type() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void signal_header::set_has_cmd_type() {
  _has_bits_[0] |= 0x00000004u;
}
inline void signal_header::clear_has_cmd_type() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void signal_header::clear_cmd_type() {
  cmd_type_ = 0;
  clear_has_cmd_type();
}
inline ::header::signal_cmd_type signal_header::cmd_type() const {
  // @@protoc_insertion_point(field_get:header.signal_header.cmd_type)
  return static_cast< ::header::signal_cmd_type >(cmd_type_);
}
inline void signal_header::set_cmd_type(::header::signal_cmd_type value) {
  assert(::header::signal_cmd_type_IsValid(value));
  set_has_cmd_type();
  cmd_type_ = value;
  // @@protoc_insertion_point(field_set:header.signal_header.cmd_type)
}

// required uint32 timestamp = 4;
inline bool signal_header::has_timestamp() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void signal_header::set_has_timestamp() {
  _has_bits_[0] |= 0x00000008u;
}
inline void signal_header::clear_has_timestamp() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void signal_header::clear_timestamp() {
  timestamp_ = 0u;
  clear_has_timestamp();
}
inline ::google::protobuf::uint32 signal_header::timestamp() const {
  // @@protoc_insertion_point(field_get:header.signal_header.timestamp)
  return timestamp_;
}
inline void signal_header::set_timestamp(::google::protobuf::uint32 value) {
  set_has_timestamp();
  timestamp_ = value;
  // @@protoc_insertion_point(field_set:header.signal_header.timestamp)
}

// required string from = 5;
inline bool signal_header::has_from() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void signal_header::set_has_from() {
  _has_bits_[0] |= 0x00000010u;
}
inline void signal_header::clear_has_from() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void signal_header::clear_from() {
  if (from_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    from_->clear();
  }
  clear_has_from();
}
inline const ::std::string& signal_header::from() const {
  // @@protoc_insertion_point(field_get:header.signal_header.from)
  return *from_;
}
inline void signal_header::set_from(const ::std::string& value) {
  set_has_from();
  if (from_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    from_ = new ::std::string;
  }
  from_->assign(value);
  // @@protoc_insertion_point(field_set:header.signal_header.from)
}
inline void signal_header::set_from(const char* value) {
  set_has_from();
  if (from_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    from_ = new ::std::string;
  }
  from_->assign(value);
  // @@protoc_insertion_point(field_set_char:header.signal_header.from)
}
inline void signal_header::set_from(const char* value, size_t size) {
  set_has_from();
  if (from_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    from_ = new ::std::string;
  }
  from_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:header.signal_header.from)
}
inline ::std::string* signal_header::mutable_from() {
  set_has_from();
  if (from_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    from_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:header.signal_header.from)
  return from_;
}
inline ::std::string* signal_header::release_from() {
  clear_has_from();
  if (from_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = from_;
    from_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void signal_header::set_allocated_from(::std::string* from) {
  if (from_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete from_;
  }
  if (from) {
    set_has_from();
    from_ = from;
  } else {
    clear_has_from();
    from_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:header.signal_header.from)
}

// required string to = 6;
inline bool signal_header::has_to() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void signal_header::set_has_to() {
  _has_bits_[0] |= 0x00000020u;
}
inline void signal_header::clear_has_to() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void signal_header::clear_to() {
  if (to_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    to_->clear();
  }
  clear_has_to();
}
inline const ::std::string& signal_header::to() const {
  // @@protoc_insertion_point(field_get:header.signal_header.to)
  return *to_;
}
inline void signal_header::set_to(const ::std::string& value) {
  set_has_to();
  if (to_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    to_ = new ::std::string;
  }
  to_->assign(value);
  // @@protoc_insertion_point(field_set:header.signal_header.to)
}
inline void signal_header::set_to(const char* value) {
  set_has_to();
  if (to_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    to_ = new ::std::string;
  }
  to_->assign(value);
  // @@protoc_insertion_point(field_set_char:header.signal_header.to)
}
inline void signal_header::set_to(const char* value, size_t size) {
  set_has_to();
  if (to_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    to_ = new ::std::string;
  }
  to_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:header.signal_header.to)
}
inline ::std::string* signal_header::mutable_to() {
  set_has_to();
  if (to_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    to_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:header.signal_header.to)
  return to_;
}
inline ::std::string* signal_header::release_to() {
  clear_has_to();
  if (to_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = to_;
    to_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void signal_header::set_allocated_to(::std::string* to) {
  if (to_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete to_;
  }
  if (to) {
    set_has_to();
    to_ = to;
  } else {
    clear_has_to();
    to_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:header.signal_header.to)
}

// required bytes content = 7;
inline bool signal_header::has_content() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void signal_header::set_has_content() {
  _has_bits_[0] |= 0x00000040u;
}
inline void signal_header::clear_has_content() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void signal_header::clear_content() {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_->clear();
  }
  clear_has_content();
}
inline const ::std::string& signal_header::content() const {
  // @@protoc_insertion_point(field_get:header.signal_header.content)
  return *content_;
}
inline void signal_header::set_content(const ::std::string& value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set:header.signal_header.content)
}
inline void signal_header::set_content(const char* value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set_char:header.signal_header.content)
}
inline void signal_header::set_content(const void* value, size_t size) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:header.signal_header.content)
}
inline ::std::string* signal_header::mutable_content() {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:header.signal_header.content)
  return content_;
}
inline ::std::string* signal_header::release_content() {
  clear_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = content_;
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void signal_header::set_allocated_content(::std::string* content) {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete content_;
  }
  if (content) {
    set_has_content();
    content_ = content;
  } else {
    clear_has_content();
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:header.signal_header.content)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace header

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::header::signal_group_type> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::header::signal_group_type>() {
  return ::header::signal_group_type_descriptor();
}
template <> struct is_proto_enum< ::header::signal_cmd_type> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::header::signal_cmd_type>() {
  return ::header::signal_cmd_type_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_header_2eproto__INCLUDED
