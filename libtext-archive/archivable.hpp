#ifndef  LIBTEXT_ARCHIVE_ARCHIVABLE_HPP
# define LIBTEXT_ARCHIVE_ARCHIVABLE_HPP

class OArchive;
class IArchive;

struct Archivable
{
  virtual void serialize(IArchive&) = 0;
  virtual void serialize(OArchive&) const = 0;
};

# define define_serializer(body) \
  void serialize(IArchive& archive) override { archive & body; } \
  void serialize(OArchive& archive) const override { archive & body; }

# define ARCHIVE_SERIALIZER(body) define_serializer(body)

#endif
