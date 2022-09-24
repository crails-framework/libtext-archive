#ifndef  CRAILS_ARCHIVE_ERRORS_HPP
# define CRAILS_ARCHIVE_ERRORS_HPP

# include <exception>

struct ArchiveUnmatchingTypeError : public std::exception
{
  unsigned char received_type;
  unsigned char expected_type;
  std::string   error;

  ArchiveUnmatchingTypeError(unsigned char receive, unsigned char expected)
    : received_type(receive), expected_type(expected)
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

struct ArchiveUnimplementedSerializer : public std::exception
{
  const char* what() const noexcept override
  {
    return "Archive: unimplemented serializer";
  }
};

struct ArchiveUnimplementedUnserializer : public std::exception
{
  const char* what() const noexcept override
  {
    return "Archive: unimplemented unserializer";
  }
};

#endif
