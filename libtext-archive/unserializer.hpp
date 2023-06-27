#ifndef  LIBTEXT_ARCHIVE_UNSERIALIZER_HPP
# define LIBTEXT_ARCHIVE_UNSERIALIZER_HPP

# include "archive.hpp"

class IArchive : public Archive
{
  friend class Archive;

  template<typename ARRAY, bool reservable>
  struct ArrayReserve { static void reserve(ARRAY&, std::size_t) {} };
  template<typename ARRAY>
  struct ArrayReserve<ARRAY, true> { static void reserve(ARRAY& array, std::size_t length) { array.reserve(length); } };

  template<typename T, bool archivable = std::is_base_of<Archivable, T>::value, bool pointer = std::is_pointer<T>::value>
  struct Serializer
  {
    static IArchive& unserialize(IArchive& archive, T& value)
    {
      if (archive.str[archive.offset] == archive.typecode<T>())
        archive.offset += 1;
      else
        throw ArchiveUnmatchingTypeError(archive.description(archive.offset), archive.str[archive.offset], archive.typecode<T>());
      archive.unserialize<T>(value);
      return archive;
    }
  };

  template<typename T, bool archivable>
  struct Serializer<T, archivable, true>
  {
    static IArchive& unserialize(IArchive& archive, T& value)
    {
      bool is_null = archive.str[archive.offset] == '0';

      archive.offset += 1;
      if (!is_null)
      {
        if (value == nullptr)
          throw ArchiveNullPointerError(archive.description(archive.offset));
        Serializer<typename std::pointer_traits<T>::element_type>::unserialize(archive, *value);
      }
    }
  };

  template<typename T>
  struct Serializer<T, true, false>
  {
    static IArchive& unserialize(IArchive& archive, T& value)
    {
      value.serialize(archive);
      return archive;
    }
  };

  template<typename T, bool reservable = false, bool archivable = std::is_base_of<Archivable, T>::value>
  struct ArraySerializer
  {
    template<typename ARRAY>
    static IArchive& unserialize(IArchive& archive, ARRAY& value)
    {
      unsigned long length;

      if (archive.str[archive.offset] != Archive::array_typecode)
        throw ArchiveUnmatchingTypeError(archive.description(archive.offset), archive.str[archive.offset], Archive::array_typecode);
      if (archive.str[archive.offset + 1] != archive.typecode<T>())
        throw ArchiveUnmatchingTypeError(archive.description(archive.offset), archive.str[archive.offset + 1], archive.typecode<T>());
      archive.offset += 2;
      archive.unserialize_number<unsigned long>(length);
      ArrayReserve<ARRAY, reservable>::reserve(value, length);
      for (unsigned long i = 0 ; i < length ; ++i)
      {
        T item;
        archive.unserialize<T>(item);
        value.push_back(item);
      }
      return archive;
    }
  };

  template<typename T, bool reservable>
  struct ArraySerializer<T, reservable, true>
  {
    template<typename ARRAY>
    static IArchive& unserialize(IArchive& archive, ARRAY& value)
    {
      unsigned long length;

      if (archive.str[archive.offset] != Archive::array_typecode)
        throw ArchiveUnmatchingTypeError(archive.description(archive.offset), archive.str[archive.offset], Archive::array_typecode);
      archive.offset += 1;
      archive.unserialize_number<unsigned long>(length);
      ArrayReserve<ARRAY, reservable>::reserve(value, length);
      for (unsigned long i = 0 ; i < length ; ++i)
      {
        value.push_back(T());
        value.rbegin()->serialize(archive);
      }
      return archive;
    }
  };

public:
  IArchive() : offset(0) {}

  void set_data(const std::string& str)
  {
    this->str = str;
    offset = 0;
  }

  template<typename T>
  IArchive& operator&(T& value)
  {
    return Serializer<T>::unserialize(*this, value);
  }

  template<typename T>
  IArchive& operator&(std::vector<T>& value)
  {
    return ArraySerializer<T, true>::unserialize(*this, value);
  }

  template<typename T>
  IArchive& operator&(std::list<T>& value)
  {
    return ArraySerializer<T, false>::unserialize(*this, value);
  }

  template<typename T>
  IArchive& operator&(std::set<T>& value)
  {
    return ArraySerializer<T, false>::unserialize(*this, value);
  }

  template<typename KEY, typename VALUE>
  IArchive& operator&(std::map<KEY, VALUE>& map)
  {
    int length;

    if (str[offset] != Archive::map_typecode)
      throw ArchiveUnmatchingTypeError(description(offset), str[offset], Archive::array_typecode);
    offset += 1;
    unserialize_number<int>(length);
    for (int i = 0 ; i < length ; ++i)
    {
      KEY   key;
      VALUE value;

      (*this) & key & value;
      map.emplace(key, value);
    }
    return *this;
  }

  template<typename T>
  IArchive& operator&(std::shared_ptr<T>& value)
  {
    offset += 1;
    if (str[offset - 1] == Archive::pointer_typecode)
    {
      value = std::make_shared<T>();
      return this->operator&<T>(*value);
    }
    else
      value = nullptr;
    return *this;
  }

  template<typename T>
  IArchive& operator&(std::unique_ptr<T>& value)
  {
    offset += 1;
    if (str[offset - 1] == Archive::pointer_typecode)
    {
      value.reset(new T);
      return this->operator&<T>(*value);
    }
    else
      value.reset(nullptr);
    return *this;
  }

  template<typename T>
  void unserialize(T&)
  {
    throw ArchiveUnimplementedUnserializer(typeid(T).name());
  }

  void output_next_value(std::ostream&);

protected:
  unsigned int offset;

  template<typename NUMERICAL_TYPE>
  void unserialize_number(NUMERICAL_TYPE& value)
  {
    int length = 0;
    std::stringstream stream;

    for (size_t i = offset ; i < str.size() && str[i] != ';' ; ++i)
    {
      stream << str[i];
      ++length;
    }
    stream >> std::hex >> value;
    offset += length + 1;
  }

  template<typename CHAR_TYPE>
  void unserialize_char(CHAR_TYPE& value)
  {
    value = (CHAR_TYPE)(str[offset]);
    offset += 1;
  }
};

#endif
