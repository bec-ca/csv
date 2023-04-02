#include "csv_file.hpp"

#include "bee/error.hpp"
#include "bee/file_reader.hpp"
#include "bee/filesystem.hpp"

#include <string>
#include <vector>

using std::nullopt;
using std::optional;
using std::string;
using std::vector;

namespace csv {
namespace {

bee::OrError<vector<string>> parse_row(const string& row)
{
  string value;
  vector<string> output;
  auto flush = [&]() {
    output.push_back(value);
    value.clear();
  };
  for (char c : row) {
    if (c == ',') {
      flush();
    } else if (c == '\n') {
      flush();
      break;
    } else {
      value += c;
    }
  }
  flush();
  return output;
}

bee::OrError<vector<string>> parse_from_file(bee::FileReader& file)
{
  bail(line, file.read_line());
  return parse_row(line);
}

} // namespace

using it = vector<string>::const_iterator;

////////////////////////////////////////////////////////////////////////////////
// CsvElement
//

const string& CsvElement::col() const { return *_col; }
const string& CsvElement::value() const { return *_value; };

const CsvElement& CsvElement::operator*() const { return *this; }

CsvElement::CsvElement(const it& col, const it& value)
    : _col(col), _value(value)
{}

////////////////////////////////////////////////////////////////////////////////
// CsvRowIter
//

struct CsvRow;

CsvElement CsvRowIter::operator*() const { return CsvElement(_col, _value); }
CsvElement CsvRowIter::operator->() const { return CsvElement(_col, _value); }

CsvRowIter& CsvRowIter::operator++()
{
  _col++;
  _value++;
  return *this;
}

CsvRowIter CsvRowIter::operator++(int) { return CsvRowIter(++_col, ++_value); }

bool CsvRowIter::operator==(const CsvRowIter& other) const
{
  return _col == other._col || _value == other._value;
}

bool CsvRowIter::operator!=(const CsvRowIter& other) const
{
  return !(*this == other);
}

CsvRowIter::CsvRowIter(const it& col, const it& value)
    : _col(col), _value(value)
{}

////////////////////////////////////////////////////////////////////////////////
// CsvRow
//

CsvRow::CsvRow(const vector<string>& header, vector<string>&& values)
    : _header(header), _values(std::move(values))
{}

CsvRow::~CsvRow() {}

CsvRowIter CsvRow::begin() const
{
  return CsvRowIter(_header.begin(), _values.begin());
}

CsvRowIter CsvRow::end() const
{
  return CsvRowIter(_header.end(), _values.end());
}

const vector<string>& CsvRow::values() const { return _values; }

////////////////////////////////////////////////////////////////////////////////
// CsvFile
//

CsvFile::CsvFile(vector<string>&& header, bee::FileReader::ptr&& file)
    : _header(std::move(header)), _file(std::move(file))
{}

CsvFile::~CsvFile() {}

bee::OrError<CsvFile::ptr> CsvFile::open_csv(const string& filename)
{
  vector<string> output;
  bail(file, bee::FileReader::open(bee::FilePath::of_string(filename)));
  if (file->is_eof()) shot("No header");
  bail(header, parse_from_file(*file));
  return make_unique<CsvFile>(std::move(header), std::move(file));
}

bee::OrError<optional<CsvRow>> CsvFile::next_row()
{
  if (_file->is_eof()) { return nullopt; }
  bail(row, parse_from_file(*_file));
  return CsvRow(_header, std::move(row));
}

const vector<string>& CsvFile::header() const { return _header; }

} // namespace csv
