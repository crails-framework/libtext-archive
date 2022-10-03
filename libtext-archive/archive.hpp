#ifndef  LIBTEXT_ARCHIVE_HPP
# define LIBTEXT_ARCHIVE_HPP

# include <memory>
# include <string>
# include <vector>
# include <list>
# include <set>
# include <map>
# include <ios>
# include <sstream>
# include "errors.hpp"
# include "archivable.hpp"

class Archive
{
public:
  static const std::string mimetype;

  const std::string& as_string() const { return str; }

  template<typename T>
  static char typecode()
  {
    return '*';
  }

  static constexpr char array_typecode = 'a';
  static constexpr char map_typecode = '2';
  static constexpr char null_typecode = '0';
  static constexpr char pointer_typecode = '1';

  std::string description(unsigned int offset = 0) const { std::stringstream stream; description(stream, offset); return stream.str(); }
  void description(std::ostream&, unsigned int offset = 0) const;

protected:
  std::string str;
};

template<> char Archive::typecode<bool>();
template<> char Archive::typecode<char>();
template<> char Archive::typecode<int>();
template<> char Archive::typecode<long>();
template<> char Archive::typecode<short>();
template<> char Archive::typecode<unsigned char>();
template<> char Archive::typecode<unsigned long>();
template<> char Archive::typecode<unsigned int>();
template<> char Archive::typecode<unsigned short>();
template<> char Archive::typecode<std::string>();
template<> char Archive::typecode<double>();
template<> char Archive::typecode<long double>();
template<> char Archive::typecode<float>();
template<> char Archive::typecode<long long>();
template<> char Archive::typecode<unsigned long long>();

# include "serializer.hpp"
# include "unserializer.hpp"

template<> void OArchive::serialize<bool>(const bool& value);
template<> void OArchive::serialize<char>(const char& value);
template<> void OArchive::serialize<int>(const int& value);
template<> void OArchive::serialize<long>(const long& value);
template<> void OArchive::serialize<short>(const short& value);
template<> void OArchive::serialize<unsigned char>(const unsigned char& value);
template<> void OArchive::serialize<unsigned long>(const unsigned long& value);
template<> void OArchive::serialize<unsigned int>(const unsigned int& value);
template<> void OArchive::serialize<unsigned short>(const unsigned short& value);
template<> void OArchive::serialize<std::string>(const std::string& value);

template<> void IArchive::unserialize<bool>(bool& value);
template<> void IArchive::unserialize<char>(char& value);
template<> void IArchive::unserialize<int>(int& value);
template<> void IArchive::unserialize<long>(long& value);
template<> void IArchive::unserialize<short>(short& value);
template<> void IArchive::unserialize<unsigned char>(unsigned char& value);
template<> void IArchive::unserialize<unsigned int>(unsigned int& value);
template<> void IArchive::unserialize<unsigned long>(unsigned long& value);
template<> void IArchive::unserialize<unsigned short>(unsigned short& value);
template<> void IArchive::unserialize<std::string>(std::string& value);

#endif
