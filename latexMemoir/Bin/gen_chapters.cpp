// basic file operations
#include <iostream>
using std::cout; using std::endl;
#include <fstream>
using std::ofstream;
#include <string>
using std::string; using std::to_string;
#include <sstream>
using std::stringstream;
using std::ostringstream;
#include<vector>
using std::vector;

#include<ostream>
using std::ostream;

#include<iomanip>
using std::setw;
using std::setfill;

#include<algorithm>
using std::for_each;


class Stub;
template<typename ContainedT> class AbstractSection;
template<typename ContainedT> class Subsec;
template<typename ContainedT> class Sec;
template<typename ContainedT> class Chap;

using SUBSECTYPE=Subsec<Stub>;
using SECTYPE=Sec<SUBSECTYPE>;
using CHAPTYPE=Chap<SECTYPE>;


// Gen Chapters namespace
namespace GC{
  const string PercentSep="%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";

  ostream& printCommentAboveSection(ostream& os, const string& sectype)
  {
    os << PercentSep << "\n";
    os << PercentSep << "\n";
    os << "%%%% " << sectype << "\n";
    os << PercentSep << "\n";
    os << PercentSep << "\n";
    return os;
  }

  ostream& printNewlineAfterSection(ostream& os)
  {
    os << "\n\n\n\n\n";
    return os;
  }

  unsigned AppendixStart=0;
  char AppenNum='A';
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class Stub
{
public:
  Stub(const string&, const string&)
  {}

  ostream& print(ostream& os) const
  { return os; }
};


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
template<typename ContainedT>
class AbstractSection
{
public:
  AbstractSection(const string& title, const string& label)
    :Title_(title), Label_(label), SectionsVec_(vector<ContainedT>{})
  {}

  virtual ~AbstractSection() = 0;

  virtual ostream& print(ostream& os) const = 0;

  ostream& printSectionsVec(ostream& os) const
  {
    for_each(SectionsVec_.begin(),SectionsVec_.end(),
             [&](const auto& sec)
             {
               sec.print(os);
               os << "\n\n\n";
             });
    return os;
  }

  template<typename ...Params>
  void emplace_back(Params&&... params)
  {
    SectionsVec_.emplace_back(std::forward<Params>(params)...);
  }

  void push_back(const ContainedT& foo)
  {
    SectionsVec_.push_back(foo);
  }

  ContainedT& back()
  {
    return SectionsVec_.back();
  }

  const ContainedT& back() const
  {
    return SectionsVec_.back();
  }

  const string& title() const
  {
    return Title_;
  }

  const string& label() const
  {
    return Label_;
  }

private:
  string Title_;
  string Label_;
  vector<ContainedT> SectionsVec_ = vector<ContainedT>{};
};

template<typename ContainedT>
AbstractSection<ContainedT>::~AbstractSection()
{}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
template<typename ContainedT>
class Subsec:public AbstractSection<ContainedT>
{
public:
  Subsec(const string& title, const string& label, 
         unsigned chapnum, unsigned secnum, unsigned subsecnum)
    : AbstractSection<ContainedT>(title,label),
      Chapnum_(chapnum), Appennum_(GC::AppenNum),
      Secnum_(secnum), Subsecnum_(subsecnum)
  {}

  virtual ~Subsec() = default;

  virtual ostream& print(ostream& os) const override
  {
    GC::printCommentAboveSection(os,"NEW Subsection");

    os << "\\subsection{" <<this->title()<<"\n";
    if(Chapnum_ < GC::AppendixStart)
    {
      os << "  \\label{sec"<<std::setw(2)<<std::setfill('0')<<Chapnum_
                           <<std::setw(2)<<std::setfill('0')<<Secnum_
                           <<std::setw(2)<<std::setfill('0')<<Subsecnum_
         <<this->label()<<"}}\n";
    }
    else
    {
      os << "  \\label{sec"<<Appennum_
                           <<std::setw(2)<<std::setfill('0')<<Secnum_
                           <<std::setw(2)<<std::setfill('0')<<Subsecnum_
         <<this->label()<<"}}\n";
    }
    GC::printNewlineAfterSection(os);

    this->printSectionsVec(os);

    return os;
  }

private:
  unsigned Chapnum_;
  char Appennum_;
  unsigned Secnum_;
  unsigned Subsecnum_;
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
template<typename ContainedT>
class Sec:public AbstractSection<ContainedT>
{
public:
  Sec(const string& title, const string& label, 
      unsigned chapnum, unsigned secnum)
    : AbstractSection<ContainedT>(title,label),
      Chapnum_(chapnum), Appennum_(GC::AppenNum),Secnum_(secnum)
  {}

  virtual ~Sec() = default;

  virtual ostream& print(ostream& os) const override
  {
    GC::printCommentAboveSection(os,"NEW Section");

    os << "\\section{" <<this->title()<<"\n";
    if(Chapnum_<GC::AppendixStart)
    {
      os << "  \\label{sec"<<std::setw(2)<<std::setfill('0')<<Chapnum_
                           <<std::setw(2)<<std::setfill('0')<<Secnum_
       <<this->label()<<"}}\n";
    }
    else
    {
      os << "  \\label{sec"<<Appennum_
                           <<std::setw(2)<<std::setfill('0')<<Secnum_
       <<this->label()<<"}}\n";
    }
    GC::printNewlineAfterSection(os);

    this->printSectionsVec(os);

    return os;
  }

private:
  unsigned Chapnum_;
  char Appennum_;
  unsigned Secnum_;
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
template<typename ContainedT>
class Chap:public AbstractSection<ContainedT>
{
public:
  Chap(const string& title, const string& label, 
       unsigned chapnum)
    : AbstractSection<ContainedT>(title,label),
      Chapnum_(chapnum), Appennum_(' '),texfilepath_("./../Contents/")
  {
    // check if Appennum_ should be used instead.
    if(Chapnum_ < GC::AppendixStart)
    {
      Appennum_=' ';
    }
    else
    {
      Appennum_=GC::AppenNum++;
    }

    // set up the filepath.
    ostringstream ss;
    if(Appennum_==' ')
    {
      ss <<"./../Contents/ch"<<std::setw(2)<<std::setfill('0')
         <<Chapnum_<<label<<".tex";
    }
    else
    {
      ss <<"./../Contents/ch"<<Appennum_<<label<<".tex";
    }
    texfilepath_ = ss.str();
  }

  virtual ~Chap() = default;

  virtual ostream& print(ostream& os) const override
  {
    GC::printCommentAboveSection(os,"NEW Chapter");

    os << "\\chapter["<<this->title()<<"]\n";
    os << "{"<<this->title()<<"\n";
    if(Appennum_==' ')
    {
      os << "\\label{ch"<<std::setw(2)<<std::setfill('0')<<Chapnum_
         << this->label()<<"}}\n";
    }
    else
    {
      os << "\\label{ch"<<Appennum_
         << this->label()<<"}}\n";
    }
    os << "\\chaptermark{"<<this->title()<<"}";
    os << "\n\n\n\n\n";

    this->printSectionsVec(os);

    return os;
  }

  string texfilepath() const
  {
    return texfilepath_;
  }

private:
  unsigned Chapnum_;
  char Appennum_;
  string texfilepath_;
};



namespace GC
{
  unsigned chapCount = 0;
  unsigned secCount = 0;
  unsigned subsecCount = 0;

  unsigned currChap = 0;
  unsigned currSec = 0;
  unsigned currSubsec = 0;

  vector<CHAPTYPE> ChapVec;

  void printOut()
  {
    for_each(ChapVec.begin(),ChapVec.end(),
        [&](const auto& chap){
          ofstream file;
          file.open(chap.texfilepath());
          chap.print(file);
          file.close();     
        });
  }

  
  void addChap(const string& title, const string& label)
  {
    // increment chapCount, reset secCount.
    ++chapCount; secCount = 0;
    currChap = chapCount;
    ChapVec.emplace_back(title,label,currChap);
  }

  void addSec(const string& title, const string& label)
  {
    // increment secCount, reset subsecCount.
    ++secCount; subsecCount = 0;
    currSec = secCount;
    ChapVec.back().emplace_back(title,label,currChap,currSec);
  }

  void addSubsec(const string& title, const string& label)
  {
    // increment subsecCount
    ++subsecCount;
    currSubsec = subsecCount;
    ChapVec.back().back().emplace_back(title,label,
                                       currChap,currSec,currSubsec);
  }
}

void formatLabels(vector<string>& s)
{
  auto isLowercaseVowel = [](char c)->bool
  {
    return (c=='a' || c=='e' || c=='i' || c=='o' || c=='u');
  };

  // loop through the words
  for(auto& w : s)
  {
    // Loop through the character in each word.
    w[0]=std::toupper(w[0]);
    for(decltype(w.size()) ci=1; ci<w.size(); ++ci)
    {
      // capitalize first letter of each word: if the previous char is
      // not alpha and this is, then capitalize it.
      if((!std::isalpha(w[ci-1]))&&
         (std::isalpha(w[ci])))
      {
        w[ci]=std::toupper(w[ci]);
      }
    }

    for(auto& c:w)
    {
      // if this is not alphabet, then set it to whitespace
      if(isLowercaseVowel(c) || !std::isalpha(c))
      {
        c=' ';
      }
    }

    // Now remove all whitespace from w.
    w.erase(remove_if(w.begin(),w.end(),::isspace),w.end());
  }
}

// Then I need to write a function to convert all the exercise to answers.
int main ()
{
  using namespace GC;

  vector<vector<int>>secnums{
      {1,0,0},
      {1,1,0},
      {1,1,1},
      {1,1,2},
      {1,1,3},
      {1,2,0},
      {1,3,0},
      {1,3,1},
      {1,3,2},
      {1,3,3},
      {1,4,0},
      {1,5,0},
      {1,6,0},
      {1,6,1},
      {1,6,2},
      {1,6,3},
      {1,7,0},
      {2,0,0},
      {2,1,0},
      {2,1,1},
      {2,1,2},
      {2,2,0},
      {2,3,0},
      {2,3,1},
      {2,4,0},
      {2,5,0},
      {2,6,0},
      {2,7,0},
      {2,8,0},
      {2,9,0},
      {2,10,0},
      {2,11,0},
      {3,0,0}, 
      {3,1,0}, 
      {3,2},
      {3,3},
      {3,4},
      {3,5},
      {4},
      {4,1},
      {4,1,1}, 
      {4,1,2}, 
      {4,1,3}, 
      {4,2},
      {4,3},
      {4,4},
      {4,4,1}, 
      {4,4,2}, 
      {4,4,3}, 
      {4,4,4}, 
      {4,4,5}, 
      {4,5},
      {5},
      {5,1},
      {5,2},
      {5,3},
      {5,4},
      {5,5},
      {5,5,1},
      {5,5,2},
      {5,6},
      {5,7},
      {5,8},
      {6},
      {6,1},
      {6,2},
      {6,3},
      {6,4},
      {6,5},
      {6,6},
      {7},
      {7,1},
      {7,2},
      {7,2,1}, 
      {7,2,2}, 
      {7,2,3}, 
      {7,3},
      {7,4},
      {7,4,1}, 
      {7,5},
      {7,6},
      {7,7},
      {8},
      {8,1},
      {8,2},
      {8,3},
      {8,4},
      {8,4,1}, 
      {8,5},
      {8,6},
      {9},
      {9,1},
      {9,1,1},
      {9,1,2},
      {9,2},
      {9,3},
      {9,4},
      {9,5},
      {9,6},
      {10},
      {10,1},
      {10,2},
      {10,3},
      {10,3,1},
      {10,4},
      {10,5},
      {10,6},
      {11},
      {11,1},
      {11,1,1},
      {11,1,2},
      {11,1,3},
      {11,2},
      {11,2,1},
      {11,2,2},
      {11,2,3},
      {11,3},
      {11,4},
      {11,5},
      {11,6},
      {11,7},
      {12},
      {12,1},
      {12,1,1},
      {12,1,2},
      {12,1,3},
      {12,2},
      {12,2,1},
      {12,2,2},
      {12,2,3},
      {12,2,4},
      {12,2,5},
      {12,3},
      {12,3,1},
      {12,3,2},
      {12,3,3},
      {12,3,4},
      {12,3,5},
      {12,4},
      {12,4,1},
      {12,4,2},
      {12,4,3},
      {12,4,4},
      {12,4,5},
      {12,4,6},
      {12,5},
      {12,5,1},
      {12,5,2},
      {12,5,3},
      {12,6},
      {13},
      {13,1},
      {13,2},
      {13,2,1},
      {13,2,2},
      {13,2,3},
      {13,2,4},
      {13,3},
      {13,3,1},
      {13,3,2},
      {13,3,3},
      {13,3,4},
      {13,3,5},
      {13,3,6},
      {13,3,7},
      {13,4},
      {13,4,1},
      {13,4,2},
      {13,5},
      {14},
      {14,1},
      {14,2},
      {14,2,1},
      {14,2,2},
      {14,3},
      {14,3,1},
      {14,3,2},
      {14,3,3},
      {14,4},
      {14,4,1},
      {14,4,2},
      {14,4,3},
      {14,5},
      {14,5,1},
      {14,5,2},
      {14,6},
      {14,7},
      {14,8},
      {15},
      {15,1},
      {15,2},
      {15,3},
      {15,4},
      {15,5},
      {15,5,1},
      {15,6},
      {15,6,1},
      {15,6,2},
      {15,6,3},
      {15,6,4},
      {15,7},
      {15,7,1},
      {15,8},
      {15,8,1},
      {15,8,2},
      {15,8,3},
      {15,8,4},
      {15,9},
      {15,10}, 
      {15,10,1},
      {15,10,2},
      {15,10,3},
      {15,10,4},
      {15,10,5},
      {15,10,6},
      {15,11},
      {15,12},
      {15,12,1},
      {15,12,2},
      {15,12,3},
      {15,13},
      {16},
      {16,1},
      {16,1,1}, 
      {16,1,2}, 
      {16,2},
      {16,2,1},
      {16,2,2},
      {16,2,3},
      {16,2,4},
      {16,2,5},
      {16,3},
      {16,3,1},
      {16,3,2},
      {16,3,3},
      {16,3,4},
      {16,4},
      {16,5},
      {16,6},
      {17},
      {17,1},
      {17,2},
      {17,3},
      {17,4},
      {17,5},
      {17,6},
      {17,7},
      {17,8},
      {17,9},
      {17,10},
      {17,11},
      {18},
      {18,1},
      {18,2},
      {18,3},
      {18,4},
      {18,5},
      {18,6},
      {18,7},
      {19},
      {19,1},
      {19,1,1}, 
      {19,1,2}, 
      {19,1,3}, 
      {19,2},
      {19,2,1}, 
      {19,2,2}, 
      {19,2,3}, 
      {19,2,4}, 
      {19,3},
      {19,3,1}, 
      {19,3,2}, 
      {19,3,3}, 
      {19,3,4}, 
      {19,4},
      {19,4,1}, 
      {19,4,2}, 
      {19,4,3}, 
      {19,4,4}, 
      {19,5},
      {19,6},
      {19,6,1}, 
      {19,6,2}, 
      {19,6,3}, 
      {19,6,4}, 
      {19,7},
      {19,7,1}, 
      {19,7,2}, 
      {19,7,3}, 
      {19,8},
      {19,8,1}, 
      {19,8,2}, 
      {19,8,3}, 
      {19,8,4}, 
      {19,8,5}, 
      {19,9},
      {19,9,1}, 
      {19,10},
      {19,11},
      {20},
      {20,1},
      {20,2},
      {20,3},
      {20,3,1}, 
      {20,3,2}, 
      {20,3,3}, 
      {20,3,4}, 
      {20,4},
      {20,4,1}, 
      {20,4,2}, 
      {20,5},
      {20,6},
      {20,7},
      {21},
      {21,1},
      {21,1,1}, 
      {21,1,2}, 
      {21,2},
      {21,2,1}, 
      {21,2,2}, 
      {21,2,3}, 
      {21,3},
      {21,3,1}, 
      {21,3,2}, 
      {21,4},
      {21,5},
      {22},
      {22,1},
      {22,2},
      {22,3},
      {22,4},
      {22,5},
      {22,6},
      {22,7},
      {23},
      {23,1},
      {23,1,1},
      {23,1,2},
      {23,1,3},
      {23,1,4},
      {23,2},
      {23,3},
      {23,3,1},
      {23,4},
      {23,5},
      {23,6},
      {23,7},
      {24},
      {24,1},
      {24,2},
      {24,2,1}, 
      {24,2,2}, 
      {24,2,3}, 
      {24,2,4}, 
      {24,2,5}, 
      {24,2,6}, 
      {24,2,7}, 
      {24,3},
      {24,3,1}, 
      {24,4},
      {24,5},
      {24,6},
      {25},
      {25,1},
      {25,1,1},
      {25,1,2},
      {25,2},
      {25,2,1},
      {25,2,2},
      {25,3},
      {25,3,1},
      {25,3,2},
      {25,3,3},
      {25,3,4},
      {25,3,5},
      {25,3,6},
      {25,4},
      {25,5},
      {25,5,1},
      {25,5,2},
      {25,6},
      {25,7},
      {26},
      {26,1},
      {26,2},
      {26,3},
      {26,4},
      {26,4,1},
      {26,4,2},
      {26,4,3},
      {26,5},
      {26,5,1},
      {26,5,2},
      {26,6},
      {26,7},
      {27},
      {27,1},
      {27,2},
      {27,2,1},
      {27,2,2},
      {27,2,3},
      {27,2,4},
      {27,2,5},
      {27,3},
      {27,4},
      {28},
      {28,1},
      {28,2},
      {28,3},
      {28,4},
      {28,5},
      {28,6},
      {29},
      {29,1},
      {29,2},
      {29,3},
      {29,3,1}, 
      {29,3,2}, 
      {29,3,3}, 
      {30},
      {30,1},
      {30,1,1},
      {30,2},
      {30,2,1}, 
      {30,3},
      {30,4},
      {31},
      {31,1},
      {31,2},
      {31,2,1}, 
      {31,2,2}, 
      {31,3},
      {31,3,1}, 
      {31,3,2}, 
      {31,3,3}, 
      {31,3,4}, 
      {31,3,5}, 
      {31,3,6}, 
      {32},
      {32,1},
      {32,1,1}, 
      {32,1,2}, 
      {32,2},
      {32,2,1}, 
      {32,2,2}, 
      {32,3},
      {32,3,1}, 
      {32,3,2}, 
      {32,3,3}, 
      {32,4},
      {32,5},
      {32,6},
      {32,7},
      {33},
      {33,1},
      {33,2},
      {33,3},
      {33,3,1}, 
      {33,3,2}, 
      {33,4},
      {33,4,1}, 
      {33,4,2}, 
      {33,4,3}}; 

  vector<string> titles={
      "Function Templates",
      "A First Look at Function Templates",
      "Defining the Template",
      "Using the Template",
      "Two-Phase Translation",
      "Template Argument Deduction",
      "Multiple Template Parameters",
      "Template Parameters for Return Types",
      "Deducing the Return Type",
      "Return Type as Common Type",
      "Default Template Arguments",
      "Overloading Function Templates",
      "But, Shouldn't We ...?",
      "Pass by Value or by Reference?",
      "Why Not \\texttt{inline}?",
      "Why Not \\texttt{constexpr}?",
      "Summary",
      "Class Templates",
      "Implementation of Class Template \\texttt{Stack}",
      "Declaration of Class Templates",
      "Implementation of Member Functions",
      "Use of Class Template \\texttt{Stack}",
      "Partial Usage of Class Templates",
      "Concepts",
      "Friends",
      "Specializations of Class Templates",
      "Partial Specialization",
      "Default Class Template Arguments",
      "Type Aliases",
      "Class Template Argument Deduction",
      "Templatized Aggregates",
      "Summary",
      "Nontype Template Parameters",
      "Nontype Class Template Parameters",
      "Nontype Function Template Parameters",
      "Restrictions for Nontype Template Parameters",
      "Template Parameter Type \\texttt{auto}",
      "Summary",
      "Variadic Templates",
      "Variadic Templates",
      "Variadic Templates by Example",
      "Overloading Variadic and Nonvariadic Templates",
      "Operator \\texttt{sizeof}...",
      "Fold Expressions",
      "Application of Variadic Templates",
      "Variadic Class Templates and Variadic Expressions",
      "Variadic Expressions",
      "Variadic Indices",
      "Variadic Class Templates",
      "Variadic Deduction Guides",
      "Variadic Base Classes and \\texttt{using}",
      "Summary",
      "Tricky Basics",
      "Keyword \\texttt{typename}",
      "Zero Initialization",
      "Using \\texttt{this->}",
      "Templates for Raw Arrays and String Literals",
      "Member Templates",
      "The \\texttt{.template} Construct",
      "Generic Lambdas and Member Templates",
      "Variable Templates",
      "Template Template Parameters",
      "Summary",
      "Move Semantics and enable\\_if<>",
      "Perfect Forwarding",
      "Special Member Function Templates",
      "Disable Templates with \\texttt{enable\\_if<>}",
      "Using \\texttt{enable\\_if<>}",
      "Using Concepts to Simplify \\texttt{enable\\_if<>} Expressions",
      "Summary",
      "By Value or by Reference?",
      "Passing by Value",
      "Passing by Reference",
      "Passing by Constant Reference",
      "Passing by Nonconstant Reference",
      "Passing by Forwarding Reference",
      "Using \\texttt{std::ref()} and \\texttt{std::cref()}",
      "Dealing with String Literals and Raw Arrays",
      "Special Implementations for String Literals and Raw Arrays",
      "Dealing with Return Values",
      "Recommended Template Parameter Declarations",
      "Summary",
      "Compile-Time Programming",
      "Template Metaprogramming",
      "Computing with \\texttt{constexpr}",
      "Execution Path Selection with Partial Specialization",
      "SFINAE (Substitution Failure Is Not An Error)",
      "Expression SFINAE with \\texttt{decltype}",
      "Compile-Time \\texttt{if}",
      "Summary",
      "Using Templates in Practice",
      "The Inclusion Model",
      "Linker Errors",
      "Templates in Header Files",
      "Templates and \\texttt{inline}",
      "Precompiled Headers",
      "Decoding the Error Novel",
      "Afternotes",
      "Summary",
      "Basic Template Terminology",
      "``Class Template'' or ``Template Class''?",
      "Substitution, Instantiation, and Specialization",
      "Declarations versus Definitions",
      "Complete versus Incomplete Types",
      "The One-Definition Rule",
      "Template Arguments versus Template Parameters",
      "Summary",
      "Generic Libraries",
      "Callables",
      "Supporting Function Objects",
      "Dealing with Member Functions and Additional Arguments",
      "Wrapping Function Calls",
      "Other Utilities to Implement Generic Libraries",
      "Type Traits",
      "\\texttt{std::addressof()}",
      "\\texttt{std::declval()}",
      "Perfect Forwarding Temporaries",
      "References as Template Parameters",
      "Defer Evaluations",
      "Things to Consider When Writing Generic Libraries",
      "Summary",
      "Fundamentals in Depth",
      "Parameterized Declarations",
      "Virtual Member Functions",
      "Linkage of Templates",
      "Primary Templates",
      "Template Parameters",
      "Type Parameters",
      "Nontype Parameters",
      "Template Template Parameters",
      "Template Parameter Packs",
      "Default Template Arguments",
      "Template Arguments",
      "Function Template Arguments",
      "Type Arguments",
      "Nontype Arguments",
      "Template Template Arguments",
      "Equivalence",
      "Variadic Templates",
      "Pack Expansions",
      "Where Can Pack Expansions Occur?",
      "Function Parameter Packs",
      "Multiple and Nested Pack Expansions",
      "Zero-Length Pack Expansions",
      "Fold Expressions",
      "Friends",
      "Friend Classes of Class Templates",
      "Friend Functions of Class Templates",
      "Friend Templates",
      "Afternotes",
      "Names in Templates",
      "Name Taxonomy",
      "Looking Up Names",
      "Argument-Dependent Lookup",
      "Argument-Dependent Lookup of Friend Declarations",
      "Injected Class Names",
      "Current Instantiations",
      "Parsing Templates",
      "Context Sensitivity in Nontemplates",
      "Dependent Names of Types",
      "Dependent Names of Templates",
      "Dependent Names in Using Declarations",
      "ADL and Explicit Template Arguments",
      "Dependent Expressions",
      "Compiler Errors",
      "Inheritance and Class Templates",
      "Nondependent Base Classes",
      "Dependent Base Classes",
      "Afternotes",
      "Instantiation",
      "On-Demand Instantiation",
      "Lazy Instantiation",
      "Partial and Full Instantiation",
      "Instantiated Components",
      "The C++ Instantiation Model",
      "Two-Phase Lookup",
      "Points of Instantiation",
      "The Inclusion Model",
      "Implementation Schemes",
      "Greedy Instantiation",
      "Queried Instantiation",
      "Iterated Instantiation",
      "Explicit Instantiation",
      "Manual Instantiation",
      "Explicit Instantiation Declarations",
      "Compile-Time if Statements",
      "In the Standard Library",
      "Afternotes",
      "Template Argument Deduction",
      "The Deduction Process",
      "Deduced Contexts",
      "Special Deduction Situations",
      "Initializer Lists",
      "Parameter Packs",
      "Literal Operator Templates",
      "Rvalue References",
      "Reference Collapsing Rules",
      "Forwarding References",
      "Perfect Forwarding",
      "Deduction Surprises",
      "SFINAE (Substitution Failure Is Not An Error)",
      "Immediate Context",
      "Limitations of Deduction",
      "Allowable Argument Conversions",
      "Class Template Arguments",
      "Default Call Arguments",
      "Exception Specifications",
      "Explicit Function Template Arguments",
      "Deduction from Initializers and Expressions",
      "The \\texttt{auto} Type Specifier",
      "Expressing the Type of an Expression with \\texttt{decltype}",
      "\\texttt{decltype(auto)}",
      "Special Situations for \\texttt{auto} Deduction",
      "Structured Bindings",
      "Generic Lambdas",
      "Alias Templates",
      "Class Template Argument Deduction",
      "Deduction Guides",
      "Implicit Deduction Guides",
      "Other Subtleties",
      "Afternotes",
      "Specialization and Overloading",
      "When ``Generic Code'' Doesn't Quite Cut It",
      "Transparent Customization",
      "Semantic Transparency",
      "Overloading Function Templates",
      "Signatures",
      "Partial Ordering of Overloaded Function Templates",
      "Formal Ordering Rules",
      "Templates and Nontemplates",
      "Variadic Function Templates",
      "Explicit Specialization",
      "Full Class Template Specialization",
      "Full Function Template Specialization",
      "Full Variable Template Specialization",
      "Full Member Specialization",
      "Partial Class Template Specialization",
      "Partial Variable Template Specialization",
      "Afternotes",
      "Future Directions",
      "Relaxed \\texttt{typename} Rules",
      "Generalized Nontype Template Parameters",
      "Partial Specialization of Function Templates",
      "Named Template Arguments",
      "Overloaded Class Templates",
      "Deduction for Nonfinal Pack Expansions",
      "Regularization of void",
      "Type Checking for Templates",
      "Reflective Metaprogramming",
      "Pack Facilities",
      "ModulesPart III: Templates and Design",
      "The Polymorphic Power of Templates",
      "Dynamic Polymorphism",
      "Static Polymorphism",
      "Dynamic versus Static Polymorphism",
      "Using Concepts",
      "New Forms of Design Patterns",
      "Generic Programming",
      "Afternotes",
      "Implementing Traits",
      "An Example: Accumulating a Sequence",
      "Fixed Traits",
      "Value Traits",
      "Parameterized Traits",
      "Traits versus Policies and Policy Classes",
      "Traits and Policies: What's the Difference?",
      "Member Templates versus Template Template Parameters",
      "Combining Multiple Policies and/or Traits",
      "Accumulation with General Iterators",
      "Type Functions",
      "Element Types",
      "Transformation Traits",
      "Predicate Traits",
      "Result Type Traits",
      "SFINAE-Based Traits",
      "SFINAE Out Function Overloads",
      "SFINAE Out Partial Specializations",
      "Using Generic Lambdas for SFINAE",
      "SFINAE-Friendly Traits",
      "IsConvertibleT",
      "Detecting Members",
      "Detecting Member Types",
      "Detecting Arbitrary Member Types",
      "Detecting Nontype Members",
      "Using Generic Lambdas to Detect Members",
      "Other Traits Techniques",
      "If-Then-Else",
      "Detecting Nonthrowing Operations",
      "Traits Convenience",
      "Type Classification",
      "Determining Fundamental Types",
      "Determining Compound Types",
      "Identifying Function Types",
      "Determining Class Types",
      "Determining Enumeration Types",
      "Policy Traits",
      "Read-Only Parameter Types",
      "In the Standard Library",
      "Afternotes",
      "Overloading on Type Properties",
      "Algorithm Specialization",
      "Tag Dispatching",
      "Enabling/Disabling Function Templates",
      "Providing Multiple Specializations",
      "Where Does the \\texttt{EnableIf} Go?",
      "Compile-Time \\texttt{if}",
      "Concepts",
      "Class Specialization",
      "Enabling/Disabling Class Templates",
      "Tag Dispatching for Class Templates",
      "Instantiation-Safe Templates",
      "In the Standard Library",
      "Afternotes",
      "Templates and Inheritance",
      "The Empty Base Class Optimization (EBCO)",
      "Layout Principles",
      "Members as Base Classes",
      "The Curiously Recurring Template Pattern (CRTP)",
      "The Barton-Nackman Trick",
      "Operator Implementations",
      "Facades",
      "Mixins",
      "Curious Mixins",
      "Parameterized Virtuality",
      "Named Template Arguments",
      "Afternotes",
      "Bridging Static and Dynamic Polymorphism",
      "Function Objects, Pointers, and \\texttt{std::function<>}",
      "Generalized Function Pointers",
      "Bridge Interface",
      "Type Erasure",
      "Optional Bridging",
      "Performance Considerations",
      "Afternotes",
      "Metaprogramming",
      "The State of Modern C++ Metaprogramming",
      "Value Metaprogramming",
      "Type Metaprogramming",
      "Hybrid Metaprogramming",
      "Hybrid Metaprogramming for Unit Types",
      "The Dimensions of Reflective Metaprogramming",
      "The Cost of Recursive Instantiation",
      "Tracking All Instantiations",
      "Computational Completeness",
      "Recursive Instantiation versus Recursive Template Arguments",
      "Enumeration Values versus Static Constants",
      "Afternotes",
      "Typelists",
      "Anatomy of a Typelist",
      "Typelist Algorithms",
      "Indexing",
      "Finding the Best Match",
      "Appending to a Typelist",
      "Reversing a Typelist",
      "Transforming a Typelist",
      "Accumulating Typelists",
      "Insertion Sort",
      "Nontype Typelists",
      "Deducible Nontype Parameters",
      "Optimizing Algorithms with Pack Expansions",
      "Cons-style Typelists",
      "Afternotes",
      "Tuples",
      "Basic Tuple Design",
      "Storage",
      "Construction",
      "Basic Tuple Operations",
      "Comparison",
      "Output",
      "Tuple Algorithms",
      "Tuples as Typelists",
      "Adding to and Removing from a Tuple",
      "Reversing a Tuple",
      "Index Lists",
      "Reversal with Index Lists",
      "Shuffle and Select",
      "Expanding Tuples",
      "Optimizing Tuple",
      "Tuples and the EBCO",
      "Constant-time \\texttt{get()}",
      "Tuple Subscript",
      "Afternotes",
      "Discriminated Unions",
      "Storage",
      "Design",
      "Value Query and Extraction",
      "Element Initialization, Assignment and Destruction",
      "Initialization",
      "Destruction",
      "Assignment",
      "Visitors",
      "Visit Result Type",
      "Common Result Type",
      "Variant Initialization and Assignment",
      "Afternotes",
      "Expression Templates",
      "Temporaries and Split Loops",
      "Encoding Expressions in Template Arguments",
      "Operands of the Expression Templates",
      "The Array Type",
      "The Operators",
      "Review",
      "Expression Templates Assignments",
      "Performance and Limitations of Expression Templates",
      "Afternotes",
      "Debugging Templates",
      "Shallow Instantiation",
      "Static Assertions",
      "Archetypes",
      "Tracers",
      "Oracles",
      "Afternotes ",
      "The One-Definition Rule",
      "Translation Units",
      "Declarations and Definitions",
      "The One-Definition Rule in Detail",
      "One-per-Program Constraints",
      "One-per-Translation Unit Constraints",
      "Cross-Translation Unit Equivalence Constraints",
      "Value Categories",
      "Traditional Lvalues and Rvalues",
      "Lvalue-to-Rvalue Conversions",
      "Value Categories Since C++11",
      "Temporary Materialization",
      "Checking Value Categories with \\texttt{decltype}",
      "Reference Types",
      "Overload Resolution",
      "When Does Overload Resolution Kick In?",
      "Simplified Overload Resolution",
      "The Implied Argument for Member Functions",
      "Refining the Perfect Match",
      "Overloading Details",
      "Prefer Nontemplates or More Specialized Templates",
      "Conversion Sequences",
      "Pointer Conversions",
      "Initializer Lists",
      "Functors and Surrogate Functions",
      "Other Overloading Contexts",
      "Standard Type Utilities",
      "Using Type Traits",
      "\\texttt{std::integral\\_constant} and \\texttt{std::bool\\_constant}",
      "Things You Should Know When Using Traits",
      "Primary and Composite Type Categories",
      "Testing for the Primary Type Category",
      "Test for Composite Type Categories",
      "Type Properties and Operations",
      "Other Type Properties",
      "Test for Specific Operations",
      "Relationships Between Types",
      "Type Construction",
      "Other Traits",
      "Combining Type Traits",
      "Other Utilities",
      "Concepts",
      "Using Concepts",
      "Defining Concepts",
      "Overloading on Constraints",
      "Constraint Subsumption",
      "Constraints and Tag Dispatching",
      "Concept Tips",
      "Testing Concepts",
      "Concept Granularity",
      "Binary Compatibility"};

  vector<string> labels={
      "FnctnTmplts",
      "FrstLkAtFnctnTmplts",
      "DfnngTheTmplt",
      "UsngTheTmplt",
      "TwPhsTrnsltn",
      "TmpltArgumntDdctn",
      "MultplTmpltPrmtrs",
      "Template Parameters for Return Types",
      "Deducing the Return Type",
      "Return Type as Common Type",
      "Default Template Arguments",
      "Overloading Function Templates",
      "But, Shouldn't We ...?",
      "Pass by Value or by Reference?",
      "Why NotInline",
      "Why NotConstexpr",
      "Summary",
      "Class Templates",
      "Implementation of Class Template Stack",
      "Declaration of Class Templates",
      "Implementation of Member Functions",
      "Use of Class Template Stack",
      "Partial Usage of Class Templates",
      "Concepts",
      "Friends",
      "Specializations of Class Templates",
      "Partial Specialization",
      "Default Class Template Arguments",
      "Type Aliases",
      "Class Template Argument Deduction",
      "Templatized Aggregates",
      "Summary",
      "Nontype Template Parameters",
      "Nontype Class Template Parameters",
      "Nontype Function Template Parameters",
      "Restrictions for Nontype Template Parameters",
      "Template Parameter Type auto",
      "Summary",
      "Variadic Templates",
      "Variadic Templates",
      "Variadic Templates by Example",
      "Overloading Variadic and Nonvariadic Templates",
      "Operator sizeof",
      "Fold Expressions",
      "Application of Variadic Templates",
      "Variadic Class Templates and Variadic Expressions",
      "Variadic Expressions",
      "Variadic Indices",
      "Variadic Class Templates",
      "Variadic Deduction Guides",
      "Variadic Base Classes and using",
      "Summary",
      "Tricky Basics",
      "Keyword typename",
      "Zero Initialization",
      "Using this->",
      "Templates for Raw Arrays and String Literals",
      "Member Templates",
      "The DotTemplate Construct",
      "Generic Lambdas and Member Templates",
      "Variable Templates",
      "Template Template Parameters",
      "Summary",
      "Move Semantics and enableIf",
      "Perfect Forwarding",
      "Special Member Function Templates",
      "Disable Templates with EnableIf",
      "Using EnableIf",
      "Using Concepts to Simplify EnableIf Expressions",
      "Summary",
      "By Value or by Reference?",
      "Passing by Value",
      "Passing by Reference",
      "Passing by Constant Reference",
      "Passing by Nonconstant Reference",
      "Passing by Forwarding Reference",
      "Using Std::Ref and StdCref",
      "Dealing with String Literals and Raw Arrays",
      "Special Implementations for String Literals and Raw Arrays",
      "Dealing with Return Values",
      "Recommended Template Parameter Declarations",
      "Summary",
      "Compile-Time Programming",
      "Template Metaprogramming",
      "Computing with constexpr",
      "Execution Path Selection with Partial Specialization",
      "SFINAE Substitution Failure Is Not An Error",
      "Expression SFINAE with decltype",
      "Compile-Time if",
      "Summary",
      "Using Templates in Practice",
      "The Inclusion Model",
      "Linker Errors",
      "Templates in Header Files",
      "Templates and inline",
      "Precompiled Headers",
      "Decoding the Error Novel",
      "Afternotes",
      "Summary",
      "Basic Template Terminology",
      "Class Template'' or ``Template Class''?",
      "Substitution, Instantiation, and Specialization",
      "Declarations versus Definitions",
      "Complete versus Incomplete Types",
      "The One-Definition Rule",
      "Template Arguments versus Template Parameters",
      "Summary",
      "Generic Libraries",
      "Callables",
      "Supporting Function Objects",
      "Dealing with Member Functions and Additional Arguments",
      "Wrapping Function Calls",
      "Other Utilities to Implement Generic Libraries",
      "Type Traits",
      "StdAddressof",
      "StdDeclval",
      "Perfect Forwarding Temporaries",
      "References as Template Parameters",
      "Defer Evaluations",
      "Things to Consider When Writing Generic Libraries",
      "Summary",
      "Fundamentals in Depth",
      "Parameterized Declarations",
      "Virtual Member Functions",
      "Linkage of Templates",
      "Primary Templates",
      "Template Parameters",
      "Type Parameters",
      "Nontype Parameters",
      "Template Template Parameters",
      "Template Parameter Packs",
      "Default Template Arguments",
      "Template Arguments",
      "Function Template Arguments",
      "Type Arguments",
      "Nontype Arguments",
      "Template Template Arguments",
      "Equivalence",
      "Variadic Templates",
      "Pack Expansions",
      "Where Can Pack Expansions Occur?",
      "Function Parameter Packs",
      "Multiple and Nested Pack Expansions",
      "Zero-Length Pack Expansions",
      "Fold Expressions",
      "Friends",
      "Friend Classes of Class Templates",
      "Friend Functions of Class Templates",
      "Friend Templates",
      "Afternotes",
      "Names in Templates",
      "Name Taxonomy",
      "Looking Up Names",
      "Argument-Dependent Lookup",
      "Argument-Dependent Lookup of Friend Declarations",
      "Injected Class Names",
      "Current Instantiations",
      "Parsing Templates",
      "Context Sensitivity in Nontemplates",
      "Dependent Names of Types",
      "Dependent Names of Templates",
      "Dependent Names in Using Declarations",
      "ADL and Explicit Template Arguments",
      "Dependent Expressions",
      "Compiler Errors",
      "Inheritance and Class Templates",
      "Nondependent Base Classes",
      "Dependent Base Classes",
      "Afternotes",
      "Instantiation",
      "On-Demand Instantiation",
      "Lazy Instantiation",
      "Partial and Full Instantiation",
      "Instantiated Components",
      "The Cpp Instantiation Model",
      "Two-Phase Lookup",
      "Points of Instantiation",
      "The Inclusion Model",
      "Implementation Schemes",
      "Greedy Instantiation",
      "Queried Instantiation",
      "Iterated Instantiation",
      "Explicit Instantiation",
      "Manual Instantiation",
      "Explicit Instantiation Declarations",
      "Compile-Time if Statements",
      "In the Standard Library",
      "Afternotes",
      "Template Argument Deduction",
      "The Deduction Process",
      "Deduced Contexts",
      "Special Deduction Situations",
      "Initializer Lists",
      "Parameter Packs",
      "Literal Operator Templates",
      "Rvalue References",
      "Reference Collapsing Rules",
      "Forwarding References",
      "Perfect Forwarding",
      "Deduction Surprises",
      "SFINAE (Substitution Failure Is Not An Error)",
      "Immediate Context",
      "Limitations of Deduction",
      "Allowable Argument Conversions",
      "Class Template Arguments",
      "Default Call Arguments",
      "Exception Specifications",
      "Explicit Function Template Arguments",
      "Deduction from Initializers and Expressions",
      "The auto Type Specifier",
      "Expressing the Type of an Expression with decltype",
      "Decltype Auto",
      "Special Situations for auto Deduction",
      "Structured Bindings",
      "Generic Lambdas",
      "Alias Templates",
      "Class Template Argument Deduction",
      "Deduction Guides",
      "Implicit Deduction Guides",
      "Other Subtleties",
      "Afternotes",
      "Specialization and Overloading",
      "When ``Generic Code'' Doesn't Quite Cut It",
      "Transparent Customization",
      "Semantic Transparency",
      "Overloading Function Templates",
      "Signatures",
      "Partial Ordering of Overloaded Function Templates",
      "Formal Ordering Rules",
      "Templates and Nontemplates",
      "Variadic Function Templates",
      "Explicit Specialization",
      "Full Class Template Specialization",
      "Full Function Template Specialization",
      "Full Variable Template Specialization",
      "Full Member Specialization",
      "Partial Class Template Specialization",
      "Partial Variable Template Specialization",
      "Afternotes",
      "Future Directions",
      "Relaxed typename Rules",
      "Generalized Nontype Template Parameters",
      "Partial Specialization of Function Templates",
      "Named Template Arguments",
      "Overloaded Class Templates",
      "Deduction for Nonfinal Pack Expansions",
      "Regularization of void",
      "Type Checking for Templates",
      "Reflective Metaprogramming",
      "Pack Facilities",
      "ModulesPart III: Templates and Design",
      "The Polymorphic Power of Templates",
      "Dynamic Polymorphism",
      "Static Polymorphism",
      "Dynamic versus Static Polymorphism",
      "Using Concepts",
      "New Forms of Design Patterns",
      "Generic Programming",
      "Afternotes",
      "Implementing Traits",
      "An Example: Accumulating a Sequence",
      "Fixed Traits",
      "Value Traits",
      "Parameterized Traits",
      "Traits versus Policies and Policy Classes",
      "Traits and Policies: What's the Difference?",
      "Member Templates versus Template Template Parameters",
      "Combining Multiple Policies and/or Traits",
      "Accumulation with General Iterators",
      "Type Functions",
      "Element Types",
      "Transformation Traits",
      "Predicate Traits",
      "Result Type Traits",
      "SFINAE-Based Traits",
      "SFINAE Out Function Overloads",
      "SFINAE Out Partial Specializations",
      "Using Generic Lambdas for SFINAE",
      "SFINAE-Friendly Traits",
      "IsConvertibleT",
      "Detecting Members",
      "Detecting Member Types",
      "Detecting Arbitrary Member Types",
      "Detecting Nontype Members",
      "Using Generic Lambdas to Detect Members",
      "Other Traits Techniques",
      "If-Then-Else",
      "Detecting Nonthrowing Operations",
      "Traits Convenience",
      "Type Classification",
      "Determining Fundamental Types",
      "Determining Compound Types",
      "Identifying Function Types",
      "Determining Class Types",
      "Determining Enumeration Types",
      "Policy Traits",
      "Read-Only Parameter Types",
      "In the Standard Library",
      "Afternotes",
      "Overloading on Type Properties",
      "Algorithm Specialization",
      "Tag Dispatching",
      "Enabling/Disabling Function Templates",
      "Providing Multiple Specializations",
      "Where Does the EnableIf Go?",
      "Compile-Time if",
      "Concepts",
      "Class Specialization",
      "Enabling/Disabling Class Templates",
      "Tag Dispatching for Class Templates",
      "Instantiation-Safe Templates",
      "In the Standard Library",
      "Afternotes",
      "Templates and Inheritance",
      "The Empty Base Class Optimization (EBCO)",
      "Layout Principles",
      "Members as Base Classes",
      "The Curiously Recurring Template Pattern (CRTP)",
      "The Barton-Nackman Trick",
      "Operator Implementations",
      "Facades",
      "Mixins",
      "Curious Mixins",
      "Parameterized Virtuality",
      "Named Template Arguments",
      "Afternotes",
      "Bridging Static and Dynamic Polymorphism",
      "Function Objects, Pointers, and StdFunction",
      "Generalized Function Pointers",
      "Bridge Interface",
      "Type Erasure",
      "Optional Bridging",
      "Performance Considerations",
      "Afternotes",
      "Metaprogramming",
      "The State of Modern Cpp Metaprogramming",
      "Value Metaprogramming",
      "Type Metaprogramming",
      "Hybrid Metaprogramming",
      "Hybrid Metaprogramming for Unit Types",
      "The Dimensions of Reflective Metaprogramming",
      "The Cost of Recursive Instantiation",
      "Tracking All Instantiations",
      "Computational Completeness",
      "Recursive Instantiation versus Recursive Template Arguments",
      "Enumeration Values versus Static Constants",
      "Afternotes",
      "Typelists",
      "Anatomy of a Typelist",
      "Typelist Algorithms",
      "Indexing",
      "Finding the Best Match",
      "Appending to a Typelist",
      "Reversing a Typelist",
      "Transforming a Typelist",
      "Accumulating Typelists",
      "Insertion Sort",
      "Nontype Typelists",
      "Deducible Nontype Parameters",
      "Optimizing Algorithms with Pack Expansions",
      "Cons-style Typelists",
      "Afternotes",
      "Tuples",
      "Basic Tuple Design",
      "Storage",
      "Construction",
      "Basic Tuple Operations",
      "Comparison",
      "Output",
      "Tuple Algorithms",
      "Tuples as Typelists",
      "Adding to and Removing from a Tuple",
      "Reversing a Tuple",
      "Index Lists",
      "Reversal with Index Lists",
      "Shuffle and Select",
      "Expanding Tuples",
      "Optimizing Tuple",
      "Tuples and the EBCO",
      "Constant-time get",
      "Tuple Subscript",
      "Afternotes",
      "Discriminated Unions",
      "Storage",
      "Design",
      "Value Query and Extraction",
      "Element Initialization, Assignment and Destruction",
      "Initialization",
      "Destruction",
      "Assignment",
      "Visitors",
      "Visit Result Type",
      "Common Result Type",
      "Variant Initialization and Assignment",
      "Afternotes",
      "Expression Templates",
      "Temporaries and Split Loops",
      "Encoding Expressions in Template Arguments",
      "Operands of the Expression Templates",
      "The Array Type",
      "The Operators",
      "Review",
      "Expression Templates Assignments",
      "Performance and Limitations of Expression Templates",
      "Afternotes",
      "Debugging Templates",
      "Shallow Instantiation",
      "Static Assertions",
      "Archetypes",
      "Tracers",
      "Oracles",
      "Afternotes ",
      "The One-Definition Rule",
      "Translation Units",
      "Declarations and Definitions",
      "The One-Definition Rule in Detail",
      "One-per-Program Constraints",
      "One-per-Translation Unit Constraints",
      "Cross-Translation Unit Equivalence Constraints",
      "Value Categories",
      "Traditional Lvalues and Rvalues",
      "Lvalue-To-Rvalue Conversions",
      "Value Categories Since Cpp11",
      "Temporary Materialization",
      "Checking Value Categories with decltype",
      "Reference Types",
      "Overload Resolution",
      "When Does Overload Resolution Kick In?",
      "Simplified Overload Resolution",
      "The Implied Argument for Member Functions",
      "Refining the Perfect Match",
      "Overloading Details",
      "Prefer Nontemplates or More Specialized Templates",
      "Conversion Sequences",
      "Pointer Conversions",
      "Initializer Lists",
      "Functors and Surrogate Functions",
      "Other Overloading Contexts",
      "Standard Type Utilities",
      "Using Type Traits",
      "StdIntegralConstantAndStdBoolConstant",
      "Things You Should Know When Using Traits",
      "Primary and Composite Type Categories",
      "Testing for the Primary Type Category",
      "Test for Composite Type Categories",
      "Type Properties and Operations",
      "Other Type Properties",
      "Test for Specific Operations",
      "Relationships Between Types",
      "Type Construction",
      "Other Traits",
      "Combining Type Traits",
      "Other Utilities",
      "Concepts",
      "Using Concepts",
      "Defining Concepts",
      "Overloading on Constraints",
      "Constraint Subsumption",
      "Constraints and Tag Dispatching",
      "Concept Tips",
      "Testing Concepts",
      "Concept Granularity",
      "Binary Compatibility"};

  formatLabels(labels);

  if(secnums.size() != titles.size() ||
     secnums.size() != labels.size())
  {
    cout << "Oh noes: " << endl;
    cout << "secnums.size(): " << secnums.size() << endl;
    cout << "titles.size(): " << titles.size() << endl;
    cout << "labels.size(): " << labels.size() << endl;
    exit(EXIT_FAILURE);
  }

  ////////////////// This is important, set the appendix start.
  GC::AppendixStart=29;

  int chapDone=0, secDone=0, subsecDone=0, index=0;
  for(const auto& sec:secnums)
  {
    // Do we add a chapter?
    if((sec[0]-chapDone)==1)
    {
      addChap(titles[index],labels[index]);
      // update the chapters done.
      chapDone=sec[0];
      // Reset section and subsection counter.
      secDone=subsecDone=0;
    }else if((sec[1]-secDone)==1)
    {
      addSec(titles[index],labels[index]);
      secDone=sec[1];
      subsecDone=0;
    }
    else if((sec[2]-subsecDone) == 1)
    {
      addSubsec(titles[index],labels[index]);
      subsecDone=sec[2];
    }
    ++index;
  }

  printOut();

  return 0;
}
