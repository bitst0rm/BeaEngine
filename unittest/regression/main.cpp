#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <list>
#include <string.h>

#include <beaengine/BeaEngine.h>

#include "unittest/regression/table_reader.hpp"
#include "unittest/regression/dasm_xml.hpp"
#include "unittest/regression/dasm_result.hpp"
#include "unittest/regression/Timer.h"



// ===================================================================
void do_disasm_test (const char* table_file, results_list_t& results)
{
  std::cout << "Loading table from " << table_file << std::endl;
  table_reader_c reader (table_file);
  std::cout << "Table loaded. Number of entries: " 
	    << reader.items () << std::endl;
  double total_time = 0.0;
  for (size_t i=0; i<reader.items (); i++)
    {
      const table_item_c& ti = reader.at (i);
      DISASM* dasm = new DISASM;
      memset (dasm, 0, sizeof(DISASM));
      dasm->EIP = (UIntPtr) ti.bytes ();
      Timer timer;
      timer.start ();
      int len = Disasm (dasm);
      timer.stop ();
      double e = timer.getElapsedTimeInMicroSec ();
      total_time = total_time + e;
      results.push_back (dasm_result_s (dasm, ti, len));
      delete dasm; 
    }
  double items = reader.items ();
  std::cout << "Total disassembly time: " << total_time << " micro-secs" << std::endl
	    << "Avg. disassembly time: " << total_time / items << std::endl;
}
// ----------------------------------------------------------
int main (int argc, char* argv [])
{
  if (argc != 2)
    {
      std::cerr << "USAGE: " << argv [0] << " <table file name>" << std::endl;
      return 1;
    }
  const char* table_file = argv [1];
  try
    {
      results_list_t dasm_results;
      do_disasm_test (table_file, dasm_results);
      dasm_to_sexp (std::cout, dasm_results);
      return 0;
    }
  catch (std::exception& e)
    {
      std::cerr << "Error: " << e.what () << std::endl;
    }
  return 1;
}
