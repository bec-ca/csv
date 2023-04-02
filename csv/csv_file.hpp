#pragma once

#include "bee/error.hpp"

#include <memory>
#include <string>
#include <vector>

namespace bee {
struct FileReader;
}

namespace csv {

////////////////////////////////////////////////////////////////////////////////
// CsvElement
//

struct CsvElement {
 private:
  using it = std::vector<std::string>::const_iterator;

 public:
  const std::string& col() const;
  const std::string& value() const;

  const CsvElement& operator*() const;

 private:
  it _col;
  it _value;

  CsvElement(const it& col, const it& value);

  friend struct CsvRowIter;
};

////////////////////////////////////////////////////////////////////////////////
// CsvRowIter
//

struct CsvRowIter {
 public:
  CsvElement operator*() const;
  CsvElement operator->() const;

  CsvRowIter& operator++();

  CsvRowIter operator++(int);

  bool operator==(const CsvRowIter& other) const;
  bool operator!=(const CsvRowIter& other) const;

 private:
  using it = std::vector<std::string>::const_iterator;
  CsvRowIter(const it& col, const it& value);

  it _col;
  it _value;

  friend struct CsvRow;
};

////////////////////////////////////////////////////////////////////////////////
// CsvRow
//

struct CsvRow {
 public:
  ~CsvRow();

  CsvRowIter begin() const;

  CsvRowIter end() const;

  const std::vector<std::string>& values() const;

 private:
  CsvRow(
    const std::vector<std::string>& header, std::vector<std::string>&& values);

  const std::vector<std::string>& _header;
  std::vector<std::string> _values;

  friend struct CsvFile;
};

////////////////////////////////////////////////////////////////////////////////
// CsvFile
//

struct CsvFile {
 public:
  using ptr = std::unique_ptr<CsvFile>;

  CsvFile(
    std::vector<std::string>&& header, std::unique_ptr<bee::FileReader>&& file);
  ~CsvFile();

  static bee::OrError<ptr> open_csv(const std::string& filename);

  bee::OrError<std::optional<CsvRow>> next_row();

  const std::vector<std::string>& header() const;

 private:
  std::vector<std::string> _header;
  std::unique_ptr<bee::FileReader> _file;
};

} // namespace csv
