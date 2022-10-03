#ifndef  LIBTEXT_ARCHIVE_ARCHIVABLE_HPP
# define LIBTEXT_ARCHIVE_ARCHIVABLE_HPP

class OArchive;
class IArchive;

struct Archivable
{
  virtual void serialize(IArchive&) = 0;
  virtual void serialize(OArchive&) = 0;
};

# define define_serializer(body) \
  template<typename ARCHIVE> \
  void serialize(ARCHIVE& archive) \
  { body }

#endif
