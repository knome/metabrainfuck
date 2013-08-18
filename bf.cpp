
// Wherein we develop a comprehensive mathematical proof of the inexistance of a merciful god //
//                                           or                                               //
//                Implementing Brainfuck via C++ Template Metaprogramming                     //


// BrainFuck Implementation Details
//   Memory   : RAM Bound, expands in either direction as needed
//   Slots    : wrapping integers of value 0 to 255
//   Geometry : angles do not seem right, as if they don't quite fit.
//            : going in further to find the source of the whispering


//
// TYPE BOXED NUMBERS
//

template < unsigned char given >
struct BrainFuckSlot {
  typedef BrainFuckSlot< given + 1 > Increment ;
  typedef BrainFuckSlot< given - 1 > Decrement ;
  
  const static unsigned char value = given ;
};

template <>
struct BrainFuckSlot< 0 > {
  typedef BrainFuckSlot<   1 > Increment ;
  typedef BrainFuckSlot< 255 > Decrement ;
  
  const static unsigned char value = 0 ;
};

template <>
struct BrainFuckSlot< 255 > {
  typedef BrainFuckSlot<   0 > Increment ;
  typedef BrainFuckSlot< 254 > Decrement ;
  
  const static unsigned char value = 255 ;
};

//
// MEMORY
//

struct BrainFuckEOM {};

template < typename Current, typename Next > struct BrainFuckMemoryVector{
  typedef Current Head ;
  typedef Next    Tail ;
};

template < typename Current >
struct BrainFuckMemoryVector< Current, BrainFuckEOM > {
  typedef Current Head ;
  typedef BrainFuckMemoryVector< BrainFuckSlot< 0 >, BrainFuckEOM > Tail ;
};


template < typename Before, typename Current, typename After >
struct BrainFuckMemory {
  typedef BrainFuckMemory< Before, typename Current::Increment, After > Increment ;
  typedef BrainFuckMemory< Before, typename Current::Decrement, After > Decrement ;
  
  typedef BrainFuckMemory< typename Before::Tail,
                           typename Before::Head,
                           BrainFuckMemoryVector< Current, After > > Left ;
  
  typedef BrainFuckMemory< BrainFuckMemoryVector< Current, Before >,
                           typename After::Head,
                           typename After::Tail > Right ;
  
  template < typename Anything >
  struct Set {
    typedef BrainFuckMemory< Before, BrainFuckSlot< Anything::value >, After > Result ;
  };
  
  typedef Current Peek;
  
};

//
// STATE
//

template < unsigned long long int Ops, typename Memory, typename TheInput, typename TheOutput >
struct BrainFuckState {
  
  typedef BrainFuckState< Ops + 1, Memory, TheInput, TheOutput > Noop ;
  
  typedef BrainFuckState< Ops + 1, typename Memory::Increment, TheInput, TheOutput > Increment ;
  typedef BrainFuckState< Ops + 1, typename Memory::Decrement, TheInput, TheOutput > Decrement ;
  typedef BrainFuckState< Ops + 1, typename Memory::Left     , TheInput, TheOutput > Left      ;
  typedef BrainFuckState< Ops + 1, typename Memory::Right    , TheInput, TheOutput > Right     ;
  
  typedef BrainFuckState<
    Ops + 1,
    typename Memory::template Set< typename TheInput::Head >::Result,
    typename TheInput::Tail,
    TheOutput
    >
  Input ;
  
  typedef BrainFuckState<
    Ops + 1  ,
    Memory   ,
    TheInput ,
    typename TheOutput::template Push< Memory::Peek::value >::Result
    >
  Output ;
  
  typedef typename Memory::Peek Peek;
  
  typedef TheOutput GetOutput;
  
  template < typename Injectee >
  struct InjectState {
    typedef typename
      Injectee
      ::template ApplyToState< BrainFuckState< Ops, Memory, TheInput, TheOutput > >
      ::Result
    Result;
  };
  
};

//
// QUEUE
//

template < unsigned char... Chars > struct BrainFuckQueue ;

template < unsigned char Anything, unsigned char... Chars >
struct BrainFuckQueue < Anything, Chars... >{
  
  typedef BrainFuckSlot< Anything > Head ;
  typedef BrainFuckQueue< Chars... > Tail ;
  
  template < unsigned char InChar >
  struct Push {
    typedef BrainFuckQueue< InChar, Anything, Chars... > Result ;
  };
  
};

template <>
struct BrainFuckQueue <> {
  
  typedef BrainFuckSlot< 0 > Head ;
  typedef BrainFuckQueue<> Tail ;
  
  template < unsigned char InChar >
  struct Push {
    typedef BrainFuckQueue< InChar > Result ;
  };
  
};

//
// COMMAND TRAVERSER DECLARATIONS
//

template < typename TheApplicator, unsigned char... Chars > struct BrainFuckTraverser ;
template < unsigned char... Chars > struct Applicator ;

//
// COMMAND INTERPRETER
//

template < unsigned char... Commands > struct BrainFuckProgram ;

template < unsigned char... Commands >
struct BrainFuckProgram < '+', Commands... > {
  
  template < typename State >
  struct ApplyToState {
    typedef typename 
      BrainFuckProgram< Commands... >
      ::template ApplyToState< typename State::Increment >
      ::Result 
    Result
    ;
  };
  
};

template < unsigned char... Commands >
struct BrainFuckProgram < '-', Commands... > {
  
  template < typename State >
  struct ApplyToState {
    typedef typename
      BrainFuckProgram< Commands... >
      ::template ApplyToState< typename State::Decrement >
      ::Result
    Result
    ;
  };

};

template < unsigned char... Commands >
struct BrainFuckProgram < '<', Commands... > {
  
  template < typename State >
  struct ApplyToState {
    typedef typename
      BrainFuckProgram< Commands... >
      ::template ApplyToState< typename State::Left >
      ::Result
    Result
    ;
  };
  
};

template < unsigned char... Commands >
struct BrainFuckProgram < '>', Commands... > {
  
  template < typename State >
  struct ApplyToState {
    typedef typename
      BrainFuckProgram< Commands... >
      ::template ApplyToState< typename State::Right >
      ::Result
    Result
    ;
  };
  
};

template < unsigned char... Commands >
struct BrainFuckProgram < '.', Commands... > {
  
  template < typename State >
  struct ApplyToState {
    typedef typename
      BrainFuckProgram< Commands... >
      ::template ApplyToState< typename State::Output >
      ::Result
    Result
    ;
  };
};

template < unsigned char... Commands >
struct BrainFuckProgram < ',', Commands... > {
  
  template < typename State >
  struct ApplyToState {
    typedef typename 
      BrainFuckProgram< Commands... >
      ::template ApplyToState< typename State::Input >
      ::Result
    Result
    ;
  };
};

struct BrainFuckProgramFactory {
  
  template < unsigned char... Chars >
  struct ReceiveChars {
    typedef BrainFuckProgram< Chars... > Result ;
  };
  
};

template < unsigned char SlotValue, typename State, unsigned char... Commands >
struct HandleLoop {
  
  typedef typename
    BrainFuckTraverser< Applicator<>, Commands... >
    ::Until
    ::template InjectChars< BrainFuckProgramFactory >
    ::Result
    ::template ApplyToState< State >
    ::Result
    ::template InjectState< BrainFuckProgram< '[', Commands... > >
    ::Result
  Result;
  ;

};

template < typename State, unsigned char... Commands >
struct HandleLoop< 0, State, Commands... > {
  
  typedef typename
    BrainFuckTraverser< Applicator<>, Commands... >
    ::After
    ::template InjectChars< BrainFuckProgramFactory >
    ::Result
    ::template ApplyToState< State >
    ::Result
  Result;
  
};

template < unsigned char... Commands >
struct BrainFuckProgram < '[', Commands... > {
  
  template < typename State >
  struct ApplyToState {
    typedef typename
      HandleLoop<
        State::Peek::value,
        State,
        Commands...
      >
      ::Result
    Result
    ;
  };
  
};

template < unsigned char Anything, unsigned char... Commands >
struct BrainFuckProgram < Anything, Commands... > {
  // an ignored character
  
  template < typename State >
  struct ApplyToState {
    typedef typename 
      BrainFuckProgram< Commands... >
      ::template ApplyToState< typename State::Noop >
      ::Result
    Result
    ;
  };
  
};

template <>
struct BrainFuckProgram <> {
  // end of program
  
  template < typename State >
  struct ApplyToState {
    typedef typename State::Noop Result ;
  };
  
};

// 
// COMMAND TRAVERSER
// 

template < unsigned char... Chars > struct Applicator ;

template < unsigned char... Chars >
struct Applicator {
  
  template < typename Target >
  struct InjectChars {
    typedef typename Target::template ReceiveChars< Chars... >::Result Result;
  };
  
  template < unsigned char Char >
  struct Append {
    typedef Applicator< Chars..., Char > Result;
  };
  
  template < unsigned char... AdditionalChars >
  struct ReceiveChars {
    typedef Applicator< Chars..., AdditionalChars... > Result ;
  };
  
};

template < typename TheApplicator, unsigned char... Chars >
struct BrainFuckTraverser< TheApplicator, ']', Chars... > {
  // end of subsection
  
  typedef TheApplicator Until ;
  typedef Applicator< Chars... > After ;
  
};

template < typename TheApplicator, unsigned char... Chars >
struct BrainFuckTraverser< TheApplicator, '[', Chars... > {
  // start of subsection
  
  typedef typename
    BrainFuckTraverser< Applicator<>, Chars... >
      ::After
      ::template InjectChars<
        typename
          BrainFuckTraverser< Applicator<>, Chars... >
            ::Until
            ::template InjectChars< typename TheApplicator::template Append< '[' >::Result > ::Result
            ::template Append< ']' > ::Result
      >
      ::Result
  Until ;
  
  typedef typename
    BrainFuckTraverser< Applicator<>, Chars... >::After
  After;
  
};

template < typename TheApplicator, unsigned char Anything, unsigned char... Chars >
struct BrainFuckTraverser< TheApplicator, Anything, Chars... > {
  // anything else
  
  typedef typename
    BrainFuckTraverser< typename TheApplicator::template Append< Anything >::Result, Chars... >::Until
  Until
  ;
  
  typedef typename
    BrainFuckTraverser< Applicator<>, Chars... >::After
  After;
  
};

template < typename TheApplicator >
struct BrainFuckTraverser< TheApplicator > {
  // end of program
  
  typedef TheApplicator Until ;
  typedef Applicator<> After ;  
  
  // if we reach the end w/o finding the ] we just return nothing
  // this means our BF programs artificially terminate all dangling
  // ['s at the end of the program. A dangling [...] pair is malformed
  // input
  
};

//
// WONDER TWIN POWERS ACTIVATE
//


int main( int argc, char ** argv ){
  
  typedef BrainFuckMemory<
    BrainFuckMemoryVector< BrainFuckSlot< 0 >, BrainFuckEOM >,
    BrainFuckSlot< 0 >,
    BrainFuckMemoryVector< BrainFuckSlot< 0 >, BrainFuckEOM >
    >
    InitialMemoryState
    ;
  
  typedef BrainFuckQueue<> InitialOutput ;
  
  // from wikipedia
  struct HelloWorld {
    typedef BrainFuckProgram<
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '[', '>', '+', 
      '+', '+', '+', '+', '+', '+', '>', '+', '+', '+', '+', '+', '+', 
      '+', '+', '+', '+', '>', '+', '+', '+', '>', '+', '<', '<', '<', 
      '<', '-', ']', '>', '+', '+', '.', '>', '+', '.', '+', '+', '+', 
      '+', '+', '+', '+', '.', '.', '+', '+', '+', '.', '>', '+', '+', 
      '.', '<', '<', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', 
      '+', '+', '+', '+', '+', '.', '>', '.', '+', '+', '+', '.', '-', 
      '-', '-', '-', '-', '-', '.', '-', '-', '-', '-', '-', '-', '-', 
      '-', '.', '>', '+', '.', '>', '.'
      >
    Program
    ;
    
    typedef BrainFuckQueue<> Input ;
  };
  
  // from http://www.hevanet.com/cristofd/brainfuck/results1.txt
  // exceeded template depth on first attempt
  // exceeded available memory on all further attempts
  struct BrainFuckQuine {
    typedef BrainFuckProgram<
      '-', '>', '+', '>', '+', '+', '+', '>', '>', '+', '>', '+', '+',
      '>', '+', '>', '+', '+', '+', '>', '>', '+', '>', '+', '+', '>',
      '>', '>', '+', '>', '+', '>', '+', '>', '+', '+', '>', '+', '>',
      '>', '>', '>', '+', '+', '+', '>', '+', '>', '>', '+', '+', '>',
      '+', '>', '+', '+', '+', '>', '>', '+', '+', '>', '+', '+', '>',
      '>', '+', '>', '>', '+', '>', '+', '+', '>', '+', '+', '>', '+',
      '>', '>', '>', '>', '+', '+', '+', '>', '+', '>', '>', '>', '>',
      '+', '+', '>', '+', '+', '>', '>', '>', '>', '+', '>', '>', '+',
      '+', '>', '+', '>', '+', '+', '+', '>', '>', '>', '+', '+', '>',
      '>', '+', '+', '+', '+', '+', '+', '>', '>', '+', '>', '>', '+',
      '+', '>', '+', '>', '>', '>', '>', '+', '+', '+', '>', '>', '+',
      '+', '+', '+', '+', '>', '>', '+', '>', '+', '+', '+', '>', '>',
      '>', '+', '+', '>', '>', '+', '+', '>', '>', '+', '>', '>', '+',
      '+', '>', '+', '>', '+', '+', '+', '>', '>', '>', '+', '+', '>',
      '>', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '>', '>', '+', '>', '>', '+', '+', '>', '+', '>', '+', '+',
      '+', '>', '+', '>', '+', '+', '+', '>', '>', '>', '+', '+', '>',
      '>', '+', '+', '+', '+', '>', '>', '+', '>', '>', '+', '+', '>',
      '+', '>', '>', '>', '>', '+', '+', '+', '>', '>', '+', '+', '+',
      '+', '+', '>', '>', '>', '>', '+', '+', '>', '>', '>', '>', '+',
      '>', '+', '>', '+', '+', '>', '>', '+', '+', '+', '>', '+', '>',
      '>', '>', '>', '+', '+', '+', '>', '+', '>', '>', '>', '>', '+',
      '+', '+', '>', '+', '>', '>', '>', '>', '+', '+', '+', '>', '>',
      '+', '+', '>', '+', '+', '>', '+', '>', '+', '+', '+', '>', '+',
      '>', '+', '+', '>', '+', '+', '>', '>', '>', '>', '>', '>', '+',
      '+', '>', '+', '>', '+', '+', '+', '>', '>', '>', '>', '>', '+',
      '+', '+', '>', '>', '>', '+', '+', '>', '+', '>', '+', '+', '+',
      '>', '+', '>', '+', '>', '+', '+', '>', '>', '>', '>', '>', '>',
      '+', '+', '>', '>', '>', '+', '>', '>', '>', '+', '+', '>', '+',
      '>', '>', '>', '>', '+', '+', '+', '>', '+', '>', '>', '>', '+',
      '>', '>', '+', '+', '>', '+', '>', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '>',
      '>', '>', '>', '+', '>', '+', '>', '>', '>', '+', '>', '>', '+',
      '+', '>', '+', '>', '+', '+', '+', '>', '>', '>', '+', '+', '>',
      '>', '+', '+', '+', '+', '+', '+', '+', '+', '>', '>', '+', '>',
      '>', '+', '+', '>', '+', '>', '>', '>', '>', '+', '+', '+', '>',
      '>', '+', '+', '+', '+', '+', '+', '>', '>', '>', '+', '>', '+',
      '+', '>', '>', '+', '+', '+', '>', '+', '>', '+', '>', '+', '+',
      '>', '+', '>', '+', '+', '+', '>', '>', '>', '>', '>', '+', '+',
      '+', '>', '>', '>', '+', '>', '+', '>', '>', '+', '+', '>', '+',
      '>', '+', '+', '+', '>', '>', '>', '+', '+', '>', '>', '+', '+',
      '+', '+', '+', '+', '+', '+', '>', '>', '+', '>', '>', '+', '+',
      '>', '+', '>', '>', '>', '>', '+', '+', '+', '>', '>', '+', '+',
      '+', '+', '>', '>', '+', '>', '+', '+', '+', '>', '>', '>', '>',
      '>', '>', '+', '+', '>', '+', '>', '+', '+', '+', '>', '>', '+',
      '>', '+', '+', '>', '>', '>', '>', '+', '>', '+', '>', '+', '+',
      '>', '+', '>', '>', '>', '>', '+', '+', '+', '>', '>', '+', '+',
      '+', '>', '>', '>', '+', '[', '[', '-', '>', '>', '+', '<', '<',
      ']', '<', '+', ']', '+', '+', '+', '+', '+', '[', '-', '>', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '<', ']', '>', '.', '[',
      '+', ']', '>', '>', '[', '<', '<', '+', '+', '+', '+', '+', '+',
      '+', '[', '-', '>', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '<', ']', '>', '-', '.', '-', '-', '-', '-', '-', '-', '-', '-',
      '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '>', '-',
      '[', '-', '<', '.', '<', '+', '>', '>', ']', '<', '[', '+', ']',
      '<', '+', '>', '>', '>', ']', '<', '<', '<', '[', '-', '[', '-',
      '[', '-', '[', '>', '>', '+', '<', '+', '+', '+', '+', '+', '+',
      '[', '-', '>', '+', '+', '+', '+', '+', '<', ']', ']', '>', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '<', ']', '>', '+', '+', '+', '<', ']', '+', '+', '+', '+', '+',
      '+', '[', '-', '>', '+', '+', '+', '+', '+', '+', '+', '<', ']',
      '>', '+', '<', '<', '<', '-', '[', '-', '>', '>', '>', '+', '+',
      '<', '<', '<', ']', '>', '[', '-', '>', '>', '.', '<', '<', ']',
      '<', '<', ']'
      >
    Program;
    
    typedef BrainFuckQueue<> Input ;
    
  };
  
  // Exceeds template depth as expected
  struct InfiniteLoop {
    typedef BrainFuckProgram<
      '+', '[', ']'
      >
    Program
    ;
    
    typedef BrainFuckQueue<> Input ;
  };
  
  // http://codegolf.com/competition/run/8/1
  // hello world
  struct CodeGolf_01 {
    typedef BrainFuckProgram<
      '>', '+', '+', '+', '+', '+', '+', '+', '+', '+', '[', '<', '+',
      '+', '+', '+', '+', '+', '+', '+', '>', '-', ']', '<', '.', '>',
      '+', '+', '+', '+', '+', '+', '+', '[', '<', '+', '+', '+', '+',
      '>', '-', ']', '<', '+', '.', '+', '+', '+', '+', '+', '+', '+',
      '.', '.', '+', '+', '+', '.', '[', '-', ']', '>', '+', '+', '+',
      '+', '+', '+', '+', '+', '[', '<', '+', '+', '+', '+', '>', '-',
      ']', '<', '.', '>', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '[', '<', '+', '+', '+', '+', '+', '+', '+', '+', '>',
      '-', ']', '<', '-', '.', '-', '-', '-', '-', '-', '-', '-', '-',
      '.', '+', '+', '+', '.', '-', '-', '-', '-', '-', '-', '.', '-',
      '-', '-', '-', '-', '-', '-', '-', '.', '[', '-', ']', '>', '+',
      '+', '+', '+', '+', '+', '+', '+', '[', '<', '+', '+', '+', '+',
      '>', '-', ']', '<', '+', '.', '[', '-', ']', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '.'
      >
    Program;
    
    typedef BrainFuckQueue<> Input ;
  };
  
  // http://codegolf.com/competition/run/8/2
  // just another brainfuck hacker
  //  lol: (g++)   : error: template instantiation depth exceeds maximum of 16384 
  //  lol: (clang) : bf.cpp:194:41: fatal error: recursive template instantiation exceeded maximum depth of 32768
  //               : setting instantiation to 65536 exhausts memory
  struct CodeGolf_02 {
    typedef BrainFuckProgram<
      '+', '+', '+', '[', '>', '+', '+', '+', '+', '+', '<', '-', ']',
      '>', '[', '>', '+', '>', '+', '+', '+', '>', '+', '>', '+', '+',
      '>', '+', '+', '+', '+', '+', '>', '+', '+', '<', '[', '+', '+',
      '<', ']', '>', '-', '-', '-', ']', '>', '-', '>', '-', '.', '[',
      '>', '+', '+', '>', '+', '<', '<', '-', '-', ']', '>', '-', '-',
      '.', '-', '-', '.', '+', '.', '>', '>', '>', '+', '+', '.', '<',
      '<', '.', '<', '-', '-', '-', '-', '-', '-', '.', '+', '.', '+',
      '+', '+', '+', '+', '.', '>', '>', '-', '.', '<', '+', '+', '+',
      '+', '.', '<', '-', '-', '.', '>', '>', '>', '.', '<', '<', '-',
      '-', '-', '.', '<', '.', '-', '-', '>', '-', '.', '>', '+', '.',
      '[', '+', '+', '+', '+', '+', '.', '-', '-', '-', '<', ']', '>',
      '>', '[', '.', '-', '-', '-', '>', ']', '<', '<', '.', '<', '+',
      '.', '+', '+', '.', '+', '+', '>', '+', '+', '+', '[', '.', '<',
      ']', '[', '.', ']', '<', '+', '+', '.'
      >
    Program;
    
    typedef BrainFuckQueue<> Input ;
  };
  
  // oops : forgot to note where I got this
  // also hits memory exhaustion
  struct Fibonacci {
    typedef BrainFuckProgram<
      // number of numbers to generate
      // '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      
      // exhausts even for a single number, lol
      '+',
      
      // generator
      '>', '+', '>', '>', '>', '>', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '>', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '<', '<', '<', '<', '<', '<', '[', '>',
      '[', '>', '>', '>', '>', '>', '>', '+', '>', '+', '<', '<', '<',
      '<', '<', '<', '<', '-', ']', '>', '>', '>', '>', '>', '>', '>',
      '[', '<', '<', '<', '<', '<', '<', '<', '+', '>', '>', '>', '>',
      '>', '>', '>', '-', ']', '<', '[', '>', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '[', '-', '<', '-', '[', '>', '>', '+',
      '>', '+', '<', '<', '<', '-', ']', '>', '>', '>', '[', '<', '<',
      '<', '+', '>', '>', '>', '-', ']', '+', '<', '[', '>', '[', '-',
      ']', '<', '[', '-', ']', ']', '>', '[', '<', '<', '[', '>', '>',
      '>', '+', '<', '<', '<', '-', ']', '>', '>', '[', '-', ']', ']',
      '<', '<', ']', '>', '>', '>', '[', '>', '>', '+', '>', '+', '<',
      '<', '<', '-', ']', '>', '>', '>', '[', '<', '<', '<', '+', '>',
      '>', '>', '-', ']', '+', '<', '[', '>', '[', '-', ']', '<', '[',
      '-', ']', ']', '>', '[', '<', '<', '+', '>', '>', '[', '-', ']',
      ']', '<', '<', '<', '<', '<', '<', '<', ']', '>', '>', '>', '>',
      '>', '[', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '.', '[',
      '-', ']', ']', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '<', '[', '-', '>', '-', '<', ']', '>', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+',
      '+', '+', '+', '+', '.', '[', '-', ']', '<', '<', '<', '<', '<',
      '<', '<', '<', '<', '<', '<', '<', '[', '>', '>', '>', '+', '>',
      '+', '<', '<', '<', '<', '-', ']', '>', '>', '>', '>', '[', '<',
      '<', '<', '<', '+', '>', '>', '>', '>', '-', ']', '<', '-', '[',
      '>', '>', '.', '>', '.', '<', '<', '<', '[', '-', ']', ']', '<',
      '<', '[', '>', '>', '+', '>', '+', '<', '<', '<', '-', ']', '>',
      '>', '>', '[', '<', '<', '<', '+', '>', '>', '>', '-', ']', '<',
      '<', '[', '<', '+', '>', '-', ']', '>', '[', '<', '+', '>', '-',
      ']', '<', '<', '<', '-', ']'
      >
    Program;
    
    typedef BrainFuckQueue<> Input ;
  };

  // (g++)   57230ull steps to complete for input of 150
  //         input of 200 exhausted memory
  // 
  // (clang) 101305 steps to complete for input 200
  //         input of 255 exhausted memory
  struct Test {
    
    typedef BrainFuckProgram<
      ',',
      '[',
           '[',
                '>', '+', '<', '-',
           ']',
           '>', '-',
      ']'
      >
    Program;
    
    typedef BrainFuckQueue< 255 > Input ;
    
  };
  
  /////
  ////
  /// change running program here
  //
  
  typedef Test SelectedProgram ;
  
  //
  ///
  ////
  /////
  
  typedef 
    BrainFuckState< 0, InitialMemoryState, typename SelectedProgram::Input, InitialOutput >
    InitialState
    ;
  
  typedef typename
    SelectedProgram
    ::Program
    ::ApplyToState< InitialState >
    ::Result
    Final 
    ;
  
  // this causes a template error which prints out the final state of the 'program'
  // Final::GetOutput::Wat ;
  Final::Wat ;
  
  return 0;
}
