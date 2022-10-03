#ifndef  CRAILS_ARCHIVE_ERRORS_HPP
# define CRAILS_ARCHIVE_ERRORS_HPP

# include <exception>

struct ArchiveException : public std::exception
{
};

struct ArchiveNullPointerError : public ArchiveException
{
  const std::string dump;

  ArchiveNullPointerError(const std::string& dump) : dump(dump) {}

  const char* what() const noexcept override
  {
    return "Archive: tried to unserialize to a null pointer";
  };
};

struct ArchiveUnmatchingTypeError : public ArchiveException
{
  const std::string   dump;
  const unsigned char received_type;
  const unsigned char expected_type;
  std::string   error;

  ArchiveUnmatchingTypeError(const std::string& dump, unsigned char receive, unsigned char expected)
    : dump(dump), received_type(receive), expected_type(expected)
  {
    error = std::string("Archive: unmatching types (found ") +
      static_cast<char>(received_type) +
      ", expected " +
      static_cast<char>(expected_type) +
      ')';
  }

  const char* what() const noexcept override
  {
    return error.c_str();
  }
};

struct ArchiveUnimplementedSerializer : public ArchiveException
{
  const char* what() const noexcept override
  {
    return "Archive: unimplemented serializer";
  }
};

struct ArchiveUnimplementedUnserializer : public ArchiveException
{
  const char* what() const noexcept override
  {
    return "Archive: unimplemented unserializer";
  }
};

#endif
