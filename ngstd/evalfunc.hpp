#ifndef FILE_EVALFUNC
#define FILE_EVALFUNC

/**************************************************************************/
/* File:   evalfunc.hpp                                                   */
/* Author: Joachim Schoeberl                                              */
/* Date:   01. Oct. 95                                                    */
/**************************************************************************/


namespace ngstd
{


/**
   Numerical expression parser.
   The expression is stored in reverse Polnish notation.
   The evaluation tree can be filled form an external parser,
   see Addxxx methods.
*/
class NGS_DLL_HEADER EvalFunction
{

  ///
  enum EVAL_TOKEN
  {
    ADD = '+', SUB = '-', MULT = '*', DIV = '/', LP ='(', RP = ')',
    COMMA = ',',
    NEG = 100, 
    VEC_ADD, VEC_SUB, VEC_SCAL_MULT, SCAL_VEC_MULT, VEC_VEC_MULT, VEC_SCAL_DIV,
    AND, OR, NOT, GREATER, LESS, GREATEREQUAL, LESSEQUAL, EQUAL,
    CONSTANT, IMAG, VARIABLE, FUNCTION, GLOBVAR, /* COEFF_FUNC,*/ END, STRING,
    SIN, COS, TAN, ATAN, ATAN2, EXP, LOG, ABS, SIGN, SQRT, STEP,
    BESSELJ0, BESSELY0, BESSELJ1, BESSELY1
  };

public:
  /// 
  EvalFunction ();
  /// parse from input stream
  EvalFunction (istream & aist);
  /// parse from string
  EvalFunction (const string & str);
  ///
  EvalFunction (const EvalFunction & eval2);
  /// 
  virtual ~EvalFunction ();

  /// parse from stream
  void Parse (istream & aist);
  /// define constant 
  void DefineConstant (const char * name, double val);
  /// define constant 
  void DefineGlobalVariable (const char * name, double * var);
  /// define arguments 
  void DefineArgument (const char * name, int num, int vecdim = 1, bool iscomplex = false);

  /// evaluate function
  double Eval (const double * x = NULL) const;
  /// evaluate multi-value function
  void Eval (const double * x, double * y, int ydim) const;

  /// evaluate function
  complex<double> Eval (const complex<double> * x = NULL) const;
  /// evaluate multi-value complex function
  void Eval (const complex<double> * x, complex<double> * y, int ydim) const;

  /*
  /// evaluate multi-value function
  template <typename TIN>
  void Eval (const TIN * x, complex<double> * y, int ydim) const;
  */
  template <typename TIN, typename TOUT, typename TCALC>
  void Eval (const TIN * x, TOUT * y) const;


  /// is expression complex valued ?
  bool IsComplex () const;

  /// is expression a constant ?
  bool IsConstant () const;

  /// vector dimension of result
  int Dimension() const { return res_type.vecdim; }

  /// push constant on stack. 
  void AddConstant (double val)
  { program.Append (step (val)); }

  /// push variable x[varnum-1].
  void AddVariable (int varnum)
  { program.Append (step(varnum)); }

  /// push pointer to global double value.
  void AddGlobVariable (const double * dp)
  { program.Append (step(dp)); }

  /// push operation. 
  void AddOperation (EVAL_TOKEN op)
  { program.Append (step(op)); }

  /// push function call. 
  void AddFunction (double (*fun) (double))
  { program.Append (step(fun)); }

  /// print expression
  void Print (ostream & ost) const;
protected:
   
  /// one step of evaluation
  class step
  {
  public:
    ///
    EVAL_TOKEN op;
    /// the data 
    union UNION_OP
    {
      /// a constant value
      double val;
      /// a pointer to a global variable
      const double *globvar;
      /// the input argument number varnum
      int varnum;
      /// a pointer to a unary function
      double (*fun) (double);
    }; 
    ///
    UNION_OP operand;

    /// dimension of vector
    short int vecdim;

    step () { ; }

    step (EVAL_TOKEN hop)
    { 
      op = hop;
      operand.val = 0;
    }

    step (double hval)
    { 
      op = CONSTANT;
      operand.val = hval;
    }

    step (int varnum)
    { 
      op = VARIABLE;
      operand.varnum = varnum;
    }

    step (const double * aglobvar)
    { 
      op = GLOBVAR;
      operand.globvar = aglobvar;
    }

    step (double (*fun) (double))
    {
      op = FUNCTION;
      operand.fun = fun;
    }
  };

  /// the evaluation sequence
  Array<step> program;

  class ResultType
  {
  public:
    int vecdim;
    bool isbool;
    bool iscomplex;
    ResultType ()
      : vecdim(1), isbool(false), iscomplex(false)
    { ; }
  };

  ResultType res_type;
  const double eps;

  /// parsing expression (standard parsing grammer)
  ResultType ParseExpression ();
  /// parsing expression (standard parsing grammer)
  ResultType ParseExpression2 ();
  /// parsing expression (standard parsing grammer)
  ResultType ParseSubExpression ();
  /// parsing expression (standard parsing grammer)
  ResultType ParseTerm ();
  /// parsing expression (standard parsing grammer)
  ResultType ParsePrimary ();

  /// parse from stream
  istream * ist;

  ///
  EVAL_TOKEN token;
  ///
  double num_value;
  ///
  char string_value[1000];
  ///
  int var_num, var_dim;
  ///
  double * globvar;
 
  typedef double(*TFUNP) (double);
  /// registerd functions
  static SymbolTable<TFUNP> functions;

  /// registerd constants
  SymbolTable<double> constants;

  /// registerd variables
  SymbolTable<double*> globvariables;
  
public:
  /// the arguments passed to the function
  struct argtype
  {
    int argnum;
    int dim;
    bool iscomplex;
  public:
    argtype ()
      : argnum(-1), dim(1), iscomplex(false) { ; }
    argtype (int aanum, int adim = 1, bool acomplex = false)
      : argnum(aanum), dim(adim), iscomplex(acomplex) { ; }
  };
  SymbolTable<argtype> arguments;
  int num_arguments;

  /// returns last token
  EVAL_TOKEN GetToken() const
    { return token; }

  /// returns num_value of last token
  double GetNumValue() const
    { return num_value; }

  /// returns variable number of last token
  int GetVariableNumber() const
    { return var_num; }
  /// returns dimension of variable of last token
  int GetVariableDimension() const
    { return var_dim; }

  /// returns identifier of last token
  const char * GetStringValue() const
    { return string_value; }
  
  /// read next token
  void ReadNext();

  bool ToBool (double x)  const { return x > eps; }
  bool ToBool (complex<double> x) const { return x.real() > eps; }
  double CheckReal (double x)  const { return x; }
  double CheckReal (complex<double> x) const { cerr << "illegal complex value" << endl; return 0; }

  double Abs (double x) const { return fabs(x); }
  double Abs (complex<double> x) const { return abs(x); }
};


}


#endif


