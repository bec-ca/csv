#include "csv_file.hpp"

#include "bee/file_writer.hpp"
#include "bee/format_vector.hpp"
#include "bee/testing.hpp"

using bee::FileWriter;
using bee::print_line;
using std::string;

namespace csv {
namespace {

const string content = R"(h1,h2,h3
123,23,390
40,50,
aaa,,foobar
xxx
a,b
)";

TEST(basic)
{
  // TODO: create a unique temporary file
  must_unit(
    FileWriter::save_file(bee::FilePath::of_string("/tmp/tmp.csv"), content));
  must(csv_file, CsvFile::open_csv("/tmp/tmp.csv"));
  for (int n = 1;; n++) {
    must(row, csv_file->next_row());
    if (!row.has_value()) { break; }
    print_line("Row:$", n);
    for (const auto& el : *row) { print_line("$:$", el.col(), el.value()); }
    print_line("-----");
  }
}

} // namespace
} // namespace csv
