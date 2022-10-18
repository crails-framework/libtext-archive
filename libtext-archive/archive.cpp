#include "archive.hpp"

using namespace std;

const std::string Archive::mimetype = "crails/archive";

template<> char Archive::typecode<bool>()               { return 'b'; }
template<> char Archive::typecode<int>()                { return 'c'; }
template<> char Archive::typecode<long>()               { return 'd'; }
template<> char Archive::typecode<unsigned int>()       { return 'e'; }
template<> char Archive::typecode<unsigned long>()      { return 'f'; }
template<> char Archive::typecode<long long>()          { return 'h'; }
template<> char Archive::typecode<unsigned long long>() { return 'i'; }
template<> char Archive::typecode<short>()              { return 'j'; }
template<> char Archive::typecode<unsigned short>()     { return 'k'; }
template<> char Archive::typecode<float>()              { return 'l'; }
template<> char Archive::typecode<double>()             { return 'm'; }
template<> char Archive::typecode<long double>()        { return 'n'; }
template<> char Archive::typecode<char>()               { return 'o'; }
template<> char Archive::typecode<unsigned char>()      { return 'p'; }
template<> char Archive::typecode<std::string>()        { return 'q'; }

#define type_to_typename(name) {Archive::typecode<name>(), #name}
static const std::map<char, std::string> typecode_to_typename{
  type_to_typename(bool),
  type_to_typename(int),
  type_to_typename(long),
  type_to_typename(unsigned int),
  type_to_typename(unsigned long),
  type_to_typename(long long),
  type_to_typename(unsigned long long),
  type_to_typename(short),
  type_to_typename(unsigned short),
  type_to_typename(float),
  type_to_typename(double),
  type_to_typename(long double),
  type_to_typename(char),
  type_to_typename(unsigned char),
  type_to_typename(std::string),
  {'a', "list"}, {'2', "map"}, {'0', "nullptr"}, {'1', "non-null"}
};

template<typename NUMBER>
std::string number_to_hex_string(const NUMBER value) { std::ostringstream stream; stream << std::hex << value; return stream.str(); }

template<> void OArchive::serialize<bool>(const bool& value)                     { str += (value ? '1' : '0'); }
template<> void OArchive::serialize<int>(const int& value)                       { str += number_to_hex_string(value) + ';'; }
template<> void OArchive::serialize<long>(const long& value)                     { str += number_to_hex_string(value) + ';'; }
template<> void OArchive::serialize<short>(const short& value)                   { str += number_to_hex_string(static_cast<int>(value)) + ';'; }
template<> void OArchive::serialize<unsigned long>(const unsigned long& value)   { str += number_to_hex_string(value) + ';'; }
template<> void OArchive::serialize<unsigned int>(const unsigned int& value)     { str += number_to_hex_string(value) + ';'; }
template<> void OArchive::serialize<unsigned short>(const unsigned short& value) { str += number_to_hex_string(value) + ';'; }
template<> void OArchive::serialize<float>(const float& value)                   { str += std::to_string(value); }
template<> void OArchive::serialize<double>(const double& value)                 { str += std::to_string(value); }
template<> void OArchive::serialize<long double>(const long double& value)       { str += std::to_string(value); }
template<> void OArchive::serialize<char>(const char& value)                     { str += value; }
template<> void OArchive::serialize<unsigned char>(const unsigned char& value)   { str += value; }

template<> void OArchive::serialize<std::string>(const std::string& value)
{
  str += number_to_hex_string<int>(value.length()) + ';';
  str += value;
}

template<> void IArchive::unserialize<bool>(bool& value)                     { value = str[offset++] == '1'; }
template<> void IArchive::unserialize<int>(int& value)                       { unserialize_number<int>(value); }
template<> void IArchive::unserialize<long>(long& value)                     { unserialize_number<long>(value); }
template<> void IArchive::unserialize<short>(short& value)                   { unserialize_number<short>(value); }
template<> void IArchive::unserialize<unsigned int>(unsigned int& value)     { unserialize_number<unsigned int>(value); }
template<> void IArchive::unserialize<unsigned long>(unsigned long& value)   { unserialize_number<unsigned long>(value); }
template<> void IArchive::unserialize<unsigned short>(unsigned short& value) { unserialize_number<unsigned short>(value); }
template<> void IArchive::unserialize<float>(float& value)                   { unserialize_number<float>(value); }
template<> void IArchive::unserialize<double>(double& value)                 { unserialize_number<double>(value); }
template<> void IArchive::unserialize<long double>(long double& value)       { unserialize_number<long double>(value); }
template<> void IArchive::unserialize<char>(char& value)                     { unserialize_char<char>(value); }
template<> void IArchive::unserialize<unsigned char>(unsigned char& value)   { unserialize_char<unsigned char>(value); }

template<> void IArchive::unserialize<std::string>(std::string& value)
{
  std::size_t length;

  unserialize_number<std::size_t>(length);
#if defined(__CHEERP_CLIENT__) || defined(__COMET_CLIENT__) // wide characters in std::string do not behave as expected with Cheerp
  int end = offset + length;
  value = "";
  while (offset < end)
  {
    unsigned int char_value = (unsigned int)str[offset];

    if (char_value > 255)
      end--;
    value = value + str[offset];
    offset++;
  }
#else
  value   = str.substr(offset, length);
  offset += length;
#endif
}

void IArchive::output_next_value(ostream& stream)
{
  switch (str[offset])
  {
    case 'b':
      { bool val; *this & val; stream << (val ? "true" : "false"); } break ;
    case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k':
      { long long val; offset++; unserialize_number<long long>(val); stream << val; } break ;
    case 'o':
      { char val; offset++; unserialize_char<char>(val); stream << val; } break ;
    case 'p':
      { unsigned char val; offset++; unserialize_char<unsigned char>(val); stream << val; } break ;
    case 'q':
      {
        std::string val;
        *this & val;
        stream << '"';
        for (std::size_t i = 0 ; i < val.length() ; ++i)
        {
          if (val[i] == '\n') stream << "\\n";
          else if (val[i] == '"') stream << "\\\"";
          else if (val[i] == '\\') stream << "\\\\";
          else stream << val[i];
        }
        stream << '"';
        break ;
      }
  }
}

string Archive::description(unsigned int offset) const
{
  stringstream stream;

  description(stream, offset);
  return stream.str();
}

void Archive::description(ostream& stream, unsigned int offset) const
{
  IArchive iarchive;

  iarchive.set_data(str);
  while (iarchive.offset < iarchive.str.length())
  {
    auto type_name = typecode_to_typename.find(iarchive.str[iarchive.offset]);

    if (iarchive.offset >= offset)
    {
      stream << "** parsing error happened around here (offset " << offset << ") **\n";
      offset = 0;
    }
    if (type_name != typecode_to_typename.end())
    {
      stream << '[' << type_name->second << "] -> ";
      switch (type_name->first)
      {
        case 'a':
        case '2':
        {
          iarchive.offset++;
          unsigned long array_size; iarchive.unserialize_number(array_size);
          stream << array_size << " elements\n";
          break ;
        }
        case '0':
        case '1':
          iarchive.offset++;
          break ;
        default:
          iarchive.output_next_value(stream);
          stream << '\n';
          break ;
      }
    }
    else
    {
      stream << "** unknown typecode `" << iarchive.str[iarchive.offset] << "` **\n";
      break ;
    }
  }
}
