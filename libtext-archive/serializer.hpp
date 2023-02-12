#ifndef  LIBTEXT_ARCHIVE_SERIALIZER_HPP
# define LIBTEXT_ARCHIVE_SERIALIZER_HPP

# include "archive.hpp"

class OArchive : public Archive
{
  template<typename T, bool archivable = std::is_base_of<Archive, T>::value, bool pointer = std::is_pointer<T>::value>
  struct Serializer
  {
    static OArchive& serialize(OArchive& archive, const T& value)
    {
      archive.str += archive.typecode<T>();
      archive.serialize(value);
      return archive;
    }
  };

  template<typename T, bool archivable>
  struct Serializer<T, archivable, true>
  {
    static OArchive& serialize(OArchive& archive, const T& value)
    {
      char flag = value == nullptr ? Archive::null_typecode : Archive::pointer_typecode;

      archive.str += flag;
      if (flag == Archive::pointer_typecode)
        return Serializer<typename std::pointer_traits<T>::element_type>::serialize(archive, *value);
      return archive;
    }
  };

  template<typename T>
  struct Serializer<T, true, false>
  {
    static OArchive& serialize(OArchive& archive, const T& value)
    {
      value.serialize(archive);
      return archive;
    }
  };

  template<typename T, bool archivable = std::is_base_of<Archive, T>::value>
  struct ArraySerializer
  {
    static OArchive& serialize(OArchive& archive, const T& list)
    {
      archive.str += Archive::array_typecode;
      archive.str += archive.typecode<T>();
      archive.serialize<unsigned long>(list.size());
      for (auto it = list.begin() ; it != list.end() ; ++it)
        archive.serialize<T>(*it);
      return archive;
    }
  };

  template<typename T>
  struct ArraySerializer<T, true>
  {
    static OArchive& serialize(OArchive& archive, const T& list)
    {
      archive.str += Archive::array_typecode;
      for (auto it = list.begin() ; it != list.end() ; ++it)
        it->serialize(archive);
      return archive;
    }
  };

public:
  template<typename T>
  OArchive& operator&(const T& value)
  {
    return Serializer<T>::serialize(*this, value);
  }

  template<typename T>
  OArchive& operator&(const std::vector<T>& value)
  {
    return ArraySerializer<T>::serialize(*this, value);
  }

  template<typename T>
  OArchive& operator&(const std::list<T>& value)
  {
    return ArraySerializer<T>::serialize(*this, value);
  }

  template<typename T>
  OArchive& operator&(const std::set<T>& value)
  {
    return ArraySerializer<T>::serialize(*this, value);
  }

  template<typename KEY, typename VALUE>
  OArchive& operator&(const std::map<KEY, VALUE>& map)
  {
    str += Archive::map_typecode;
    serialize<unsigned long>(map.size());
    for (auto it = map.begin() ; it != map.end() ; ++it)
      (*this) & it->first & it->second;
    return *this;
  }

  template<typename T>
  void serialize(const T&)
  {
    throw ArchiveUnimplementedSerializer();
  }
};

#endif
