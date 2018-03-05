#include "persistent_ntree.hpp"
#include "persistent_map.hpp"
#include "persistent_string.hpp"
#include "persistent_shortcuts.hpp"
#include "file_system.hpp"
#include "print_ntree.hpp"
#include "print_vector.hpp"
#include "print_map.hpp"
#include "print_string.hpp"
#include "build.hpp"
#include <string>
#include <map>

////////////////////////////////////////////////////////////////////////////////

#define NUMBER 1000
#define DATA "ntree_test.tmp"
#define MASTER "ntree_test.dump"

////////////////////////////////////////////////////////////////////////////////

typedef stlplus::ntree<std::string> string_tree;

////////////////////////////////////////////////////////////////////////////////

void dump_string_tree(stlplus::dump_context& context, const string_tree& data)
{
  stlplus::dump_ntree(context, data, stlplus::dump_string);
}

void restore_string_tree(stlplus::restore_context& context, string_tree& data)
{
  stlplus::restore_ntree(context, data, stlplus::restore_string);
}

// recursive part of the comparison
bool compare_r(const string_tree& left, const string_tree::const_iterator& left_i,
               const string_tree& right, const string_tree::const_iterator& right_i)
{
  bool result = true;
  // compare this node, then recursively compare the children
  if (*left_i != *right_i)
  {
    std::cerr << "left = \"" << *left_i << "\" is different from right = \"" << *right_i << "\"" << std::endl;
    result = false;
  }
  if (left.children(left_i) != right.children(right_i))
  {
    std::cerr << "left = \"" << *left_i << "\" has different number of children from right = \"" << *right_i << "\"" << std::endl;
    result = false;
  }
  else
  {
    for (unsigned i = 0; i < left.children(left_i); i++)
      result &= compare_r(left, left.child(left_i,i), right, right.child(right_i,i));
  }
  return result;
}

bool compare(const string_tree& left, const string_tree& right)
{
  // start the recursion
  if (left.empty() && right.empty())
    return true;
  if (left.empty())
  {
    std::cerr << "different size - left is empty right = " << right.size() << std::endl;
    return false;
  }
  if (right.empty())
  {
    std::cerr << "different size - right is empty left = " << left.size() << std::endl;
    return false;
  }
  return compare_r(left, left.root(), right, right.root());
}

std::ostream& operator << (std::ostream& device, const string_tree& tree)
{
  device << "tree: ";
  stlplus::print_ntree(device, tree, stlplus::print_string, std::string("\n"), std::string("  "));
  device << std::endl;
  return device;
}

////////////////////////////////////////////////////////////////////////////////

void dump_string_tree_iterator(stlplus::dump_context& context, const string_tree::iterator& data)
{
  stlplus::dump_ntree_iterator(context,data);
}
void restore_string_tree_iterator(stlplus::restore_context& context, string_tree::iterator& data)
{
  stlplus::restore_ntree_iterator(context,data);
}

void print_string_tree_iterator(std::ostream& device, const string_tree::iterator& data)
{
  if (data.null())
    device << "<null>";
  else if (data.end())
    device << "<end>";
  else
    device << *data;
}

void print_string_tree_const_iterator(std::ostream& device, const string_tree::const_iterator& data)
{
  print_string_tree_iterator(device, data.deconstify());
}

void print_string_tree_iterator_vector(std::ostream& device, const string_tree::iterator_vector& data)
{
  device << data.size() << ": ";
  stlplus::print_vector(device, data, print_string_tree_iterator, ", ");
}

void print_string_tree_const_iterator_vector(std::ostream& device, const string_tree::const_iterator_vector& data)
{
  device << data.size() << ": ";
  stlplus::print_vector(device, data, print_string_tree_const_iterator, ", ");
}

std::ostream& operator << (std::ostream& device, const string_tree::iterator& data)
{
  print_string_tree_iterator(device, data);
  return device;
}

std::ostream& operator << (std::ostream& device, const string_tree::prefix_iterator& data)
{
  print_string_tree_iterator(device, data.simplify());
  return device;
}

std::ostream& operator << (std::ostream& device, const string_tree::postfix_iterator& data)
{
  print_string_tree_iterator(device, data.simplify());
  return device;
}

std::ostream& operator << (std::ostream& device, const string_tree::iterator_vector& data)
{
  print_string_tree_iterator_vector(device, data);
  return device;
}

std::ostream& operator << (std::ostream& device, const string_tree::const_iterator_vector& data)
{
  print_string_tree_const_iterator_vector(device, data);
  return device;
}

////////////////////////////////////////////////////////////////////////////////
// a tree map is a map from the content of a node to its iterator

typedef std::map<std::string,string_tree::iterator> tree_map;

void dump_tree_map(stlplus::dump_context& context, const tree_map& data)
{
  stlplus::dump_map(context, data, stlplus::dump_string, dump_string_tree_iterator);
}

void restore_tree_map(stlplus::restore_context& context, tree_map& data)
{
  stlplus::restore_map(context, data, stlplus::restore_string, restore_string_tree_iterator);
}

std::ostream& operator << (std::ostream& device, const tree_map& mappings)
{
  device << "map: ";
  stlplus::print_map(device, mappings, stlplus::print_string, print_string_tree_iterator, "=", ",");
  device << std::endl;
  return device;
}

////////////////////////////////////////////////////////////////////////////////
// a test class that contains a tree and a tree map
// usage: build a tree and then call add_mappings to built the tree map from the tree

class mapped_tree
{
public:
  string_tree m_tree;
  tree_map m_map;

  void add_mappings(void)
    {
      std::cerr << "adding mappings" << std::endl;
      m_map.clear();
      for (string_tree::prefix_iterator i = m_tree.prefix_begin(); i != m_tree.prefix_end(); i++)
      {
        std::cerr << "mapping " << *i << " => " << i << std::endl;
        m_map[*i] = i.simplify();
      }
    }
};

void dump_mapped_tree(stlplus::dump_context& context, const mapped_tree& data)
{
  dump_string_tree(context,data.m_tree);
  dump_tree_map(context,data.m_map);
}

void restore_mapped_tree(stlplus::restore_context& context, mapped_tree& data)
{
  restore_string_tree(context,data.m_tree);
  restore_tree_map(context,data.m_map);
}
std::ostream& operator << (std::ostream& device, const mapped_tree& data)
{
  device << data.m_tree;
  device << data.m_map;
  return device;
}
bool compare(const mapped_tree& left, const mapped_tree& right)
{
  // TODO - compare maps too
  return compare(left.m_tree,right.m_tree);
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  std::cerr << stlplus::build() << std::endl;
  bool result = true;

  try
  {
    // build the sample data structure
    mapped_tree data;
    std::cerr << "null root = " << data.m_tree.root() << std::endl;
    // get breadth-first traversal of an empty tree
    std::cerr << "empty breadth-first traversal = " << data.m_tree.breadth_first_traversal() << std::endl;

    string_tree::iterator root = data.m_tree.insert("root");
    std::cerr << "added root " << root << std::endl;

    string_tree::iterator left = data.m_tree.insert(root,"left");
    std::cerr << "added left " << left << std::endl;

    string_tree::iterator left_left = data.m_tree.insert(left,"left_left");
    std::cerr << "added left_left " << left_left << std::endl;

    string_tree::iterator left_right = data.m_tree.insert(left,"left_right");
    std::cerr << "added left_right " << left_right << std::endl;

    string_tree::iterator right = data.m_tree.insert(root,"right");
    std::cerr << "added right " << right << std::endl;

    string_tree::iterator right_left = data.m_tree.insert(right,"right_left");
    std::cerr << "added right_left " << right_left << std::endl;

    string_tree::iterator right_right = data.m_tree.insert(right,"right_right");
    std::cerr << "added right_right " << right_right << std::endl;

    data.add_mappings();
    std::cerr << "tree = " << std::endl << data;

    // get breadth-first traversal
    std::cerr << "breadth-first traversal = " << data.m_tree.breadth_first_traversal() << std::endl;

    // copy the tree
    mapped_tree copied;
    copied.m_tree = data.m_tree;
    copied.add_mappings();
    std::cerr << "copied tree = " << std::endl << copied;
    result &= compare(copied,data);

    // move the tree by cutting
    mapped_tree moved;
    moved.m_tree.move(copied.m_tree);
    moved.add_mappings();
    copied.add_mappings();
    std::cerr << "moved tree = " << std::endl << moved;
    std::cerr << "source of move = " << std::endl << copied;
    result &= compare(moved,data);

    // test relationships
    if (data.m_tree.parent(left) == root)
      std::cerr << "Success: root is parent of left" << std::endl;
    else
    {
      std::cerr << "ERROR: root is not parent of left" << std::endl;
      result = false;
    }
    if (data.m_tree.parent(left) != right)
      std::cerr << "Success: right is not parent of left" << std::endl;
    else
    {
      std::cerr << "ERROR: right is parent of left" << std::endl;
      result = false;
    }

    // test use with algorithms
    // find an existent string
    string_tree::prefix_iterator found = std::find(data.m_tree.prefix_begin(), data.m_tree.prefix_end(), "left_left");
    if (found.valid())
    {
      std::cerr << "Success: found present search term" << std::endl;
    }
    else
    {
      std::cerr << "ERROR: failed to find present search term" << std::endl;
      result = false;
    }
    // find a non-existent string
    string_tree::postfix_iterator not_found = std::find(data.m_tree.postfix_begin(), data.m_tree.postfix_end(), "not_found");
    if (!not_found.valid())
    {
      std::cerr << "Success: didn't find not-present search term" << std::endl;
    }
    else
    {
      std::cerr << "ERROR: found not-present search term" << std::endl;
      result = false;
    }


    // now dump to the file
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DATA,dump_mapped_tree,0);

    // now restore the same file and compare
    std::cerr << "restoring" << std::endl;
    mapped_tree restored;
    stlplus::restore_from_file(DATA,restored,restore_mapped_tree,0);
    std::cerr << "restored = " << std::endl << restored;
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
    {
      std::cerr << "creating master" << std::endl;
      stlplus::file_copy(DATA,MASTER);
    }
    else
    {
      std::cerr << "restoring master" << std::endl;
      mapped_tree master;
      stlplus::restore_from_file(MASTER,master,restore_mapped_tree,0);
      result &= compare(data,master);
    }

    // experiment with iterators
    // erase children but keep the node
    data.m_tree.erase_children(right);
    std::cerr << "erased right's children - tree = " << std::endl << data;

    // get breadth-first traversal
    std::cerr << "breadth-first traversal = " << ((const string_tree)data.m_tree).breadth_first_traversal() << std::endl;

    // now erase the node
    data.m_tree.erase(right);
    std::cerr << "erased right - tree = " << std::endl << data;

    // dereference an erased iterator
    try
    {
      std::cerr << "Test Illegal dereference of iterator" << std::endl;
      string_tree::iterator illegal = data.m_tree.parent(right_right);
      // should not reach here
      std::cerr << "ERROR: failed to throw exception for iterator" << illegal << std::endl;
      result = false;
    }
    catch(std::exception& except)
    {
      std::cerr << "Success: caught exception " << except.what() << std::endl;
    }

    // use an iterator on the wrong tree
    try
    {
      std::cerr << "Test Illegal use of iterator on wrong tree" << std::endl;
      string_tree::iterator illegal = restored.m_tree.parent(left);
      // should not reach here
      std::cerr << "ERROR: failed to throw exception for iterator" << illegal << std::endl;
      result = false;
    }
    catch(std::exception& except)
    {
      std::cerr << "Success: caught exception " << except.what() << std::endl;
    }

    // test for cut - testing a bug fix
    // TODO - how to verify this?
    string_tree branch = data.m_tree.cut(left);
    std::cerr << "cut left = " << std::endl << branch;
    std::cerr << "cut remainder = " << std::endl << data.m_tree;

  }
  catch(std::exception& except)
  {
    std::cerr << "caught standard exception " << except.what() << std::endl;
    result = false;
  }
  catch(...)
  {
    std::cerr << "caught unknown exception" << std::endl;
    result = false;
  }

  // test the child_offset method
  // build a new simpler tree to test this
  string_tree simple_tree;
  string_tree::iterator root = simple_tree.insert("root");
  string_tree::iterator left = simple_tree.append(root, "left");
  string_tree::iterator middle = simple_tree.append(root, "middle");
  string_tree::iterator right = simple_tree.append(root, "right");
  std::cerr << "testing child offset handling, initial:\n" << simple_tree;
  // now find the iterator offsets
  unsigned left_offset = simple_tree.child_offset(root, left);
  std::cerr << "left offset: " << left_offset << std::endl;
  if (left_offset != 0)
  {
    std::cerr << "ERROR: left offset is wrong, should be 0, is " << left_offset << std::endl;
    result = false;
  }
  unsigned right_offset = simple_tree.child_offset(root, right);
  std::cerr << "right offset: " << right_offset << std::endl;
  if (right_offset != 2)
  {
    std::cerr << "ERROR: right offset is wrong, should be 1, is " << right_offset << std::endl;
    result = false;
  }
  unsigned not_found_offset = simple_tree.child_offset(left, right);
  std::cerr << "not_found offset: " << not_found_offset << std::endl;
  if (not_found_offset != (unsigned)-1)
  {
    std::cerr << "ERROR: not found offset is wrong, should be -1, is " << not_found_offset << std::endl;
    result = false;
  }

  // test the reorder method
  simple_tree.reorder(root, 0, 1);
  std::cerr << "testing child offset handling, reordered:\n" << simple_tree;
  unsigned reordered_left_offset = simple_tree.child_offset(root, left);
  std::cerr << "reordered left offset: " << reordered_left_offset << std::endl;
  if (reordered_left_offset != 1)
  {
    std::cerr << "ERROR: reordered left offset is wrong, should be 1, is " << reordered_left_offset << std::endl;
    result = false;
  }
  // test reordering to the end
  simple_tree.reorder(root, 0, 2);
  std::cerr << "testing child offset handling, reordered:\n" << simple_tree;
  unsigned reordered_middle_offset = simple_tree.child_offset(root, middle);
  std::cerr << "reordered middle offset: " << reordered_middle_offset << std::endl;
  if (reordered_middle_offset != 2)
  {
    std::cerr << "ERROR: reordered middle offset is wrong, should be 2, is " << reordered_middle_offset << std::endl;
    result = false;
  }
  // test exception handling
  std::cerr << "testing out of range exception" << std::endl;
  try
  {
    simple_tree.reorder(root, 0, 3);
    std::cerr << "ERROR: failed to throw out of range exception in reorder" << std::endl;
    result = false;
  }
  catch(std::exception& except)
  {
    std::cerr << "success: caught standard exception " << except.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "ERROR: caught unknown exception" << std::endl;
    result = false;
  }

  // test the swap method
  simple_tree.swap(root, 1, 2);
  std::cerr << "testing child offset handling, swapped:\n" << simple_tree;
  unsigned reordered_right_offset = simple_tree.child_offset(root, right);
  std::cerr << "reordered right offset: " << reordered_right_offset << std::endl;
  if (reordered_right_offset != 2)
  {
    std::cerr << "ERROR: reordered right offset is wrong, should be 1, is " << reordered_right_offset << std::endl;
    result = false;
  }


  if (!result)
    std::cerr << "test failed" << std::endl;
  else
    std::cerr << "test passed" << std::endl;
  return result ? 0 : 1;
}
