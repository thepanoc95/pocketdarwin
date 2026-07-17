/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 153 "parser.y"


#include <stdio.h>
#include "lexxer.h"
#include "strdefs.h"
#include "type.h"
#include "routine.h"
#include "statement.h"
#include "global.h"
#include "error.h"

static char *import_name();
extern int yylex(void);

/* forward declaration */
void yyerror(char *s);
  

#line 90 "parser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_sySkip = 3,                     /* sySkip  */
  YYSYMBOL_syRoutine = 4,                  /* syRoutine  */
  YYSYMBOL_sySimpleRoutine = 5,            /* sySimpleRoutine  */
  YYSYMBOL_sySubsystem = 6,                /* sySubsystem  */
  YYSYMBOL_syKernelUser = 7,               /* syKernelUser  */
  YYSYMBOL_syKernelServer = 8,             /* syKernelServer  */
  YYSYMBOL_syMsgOption = 9,                /* syMsgOption  */
  YYSYMBOL_syMsgSeqno = 10,                /* syMsgSeqno  */
  YYSYMBOL_syWaitTime = 11,                /* syWaitTime  */
  YYSYMBOL_sySendTime = 12,                /* sySendTime  */
  YYSYMBOL_syNoWaitTime = 13,              /* syNoWaitTime  */
  YYSYMBOL_syErrorProc = 14,               /* syErrorProc  */
  YYSYMBOL_syServerPrefix = 15,            /* syServerPrefix  */
  YYSYMBOL_syUserPrefix = 16,              /* syUserPrefix  */
  YYSYMBOL_syServerDemux = 17,             /* syServerDemux  */
  YYSYMBOL_syRCSId = 18,                   /* syRCSId  */
  YYSYMBOL_syImport = 19,                  /* syImport  */
  YYSYMBOL_syUImport = 20,                 /* syUImport  */
  YYSYMBOL_sySImport = 21,                 /* sySImport  */
  YYSYMBOL_syIImport = 22,                 /* syIImport  */
  YYSYMBOL_syDImport = 23,                 /* syDImport  */
  YYSYMBOL_syIn = 24,                      /* syIn  */
  YYSYMBOL_syOut = 25,                     /* syOut  */
  YYSYMBOL_syInOut = 26,                   /* syInOut  */
  YYSYMBOL_syUserImpl = 27,                /* syUserImpl  */
  YYSYMBOL_syServerImpl = 28,              /* syServerImpl  */
  YYSYMBOL_syRequestPort = 29,             /* syRequestPort  */
  YYSYMBOL_syReplyPort = 30,               /* syReplyPort  */
  YYSYMBOL_sySReplyPort = 31,              /* sySReplyPort  */
  YYSYMBOL_syUReplyPort = 32,              /* syUReplyPort  */
  YYSYMBOL_syType = 33,                    /* syType  */
  YYSYMBOL_syArray = 34,                   /* syArray  */
  YYSYMBOL_syStruct = 35,                  /* syStruct  */
  YYSYMBOL_syOf = 36,                      /* syOf  */
  YYSYMBOL_syInTran = 37,                  /* syInTran  */
  YYSYMBOL_syOutTran = 38,                 /* syOutTran  */
  YYSYMBOL_syDestructor = 39,              /* syDestructor  */
  YYSYMBOL_syCType = 40,                   /* syCType  */
  YYSYMBOL_syCUserType = 41,               /* syCUserType  */
  YYSYMBOL_syUserTypeLimit = 42,           /* syUserTypeLimit  */
  YYSYMBOL_syOnStackLimit = 43,            /* syOnStackLimit  */
  YYSYMBOL_syCServerType = 44,             /* syCServerType  */
  YYSYMBOL_syPointerTo = 45,               /* syPointerTo  */
  YYSYMBOL_syPointerToIfNot = 46,          /* syPointerToIfNot  */
  YYSYMBOL_syValueOf = 47,                 /* syValueOf  */
  YYSYMBOL_syCString = 48,                 /* syCString  */
  YYSYMBOL_sySecToken = 49,                /* sySecToken  */
  YYSYMBOL_syUserSecToken = 50,            /* syUserSecToken  */
  YYSYMBOL_syServerSecToken = 51,          /* syServerSecToken  */
  YYSYMBOL_syAuditToken = 52,              /* syAuditToken  */
  YYSYMBOL_syUserAuditToken = 53,          /* syUserAuditToken  */
  YYSYMBOL_syServerAuditToken = 54,        /* syServerAuditToken  */
  YYSYMBOL_syColon = 55,                   /* syColon  */
  YYSYMBOL_sySemi = 56,                    /* sySemi  */
  YYSYMBOL_syComma = 57,                   /* syComma  */
  YYSYMBOL_syPlus = 58,                    /* syPlus  */
  YYSYMBOL_syMinus = 59,                   /* syMinus  */
  YYSYMBOL_syStar = 60,                    /* syStar  */
  YYSYMBOL_syDiv = 61,                     /* syDiv  */
  YYSYMBOL_syLParen = 62,                  /* syLParen  */
  YYSYMBOL_syRParen = 63,                  /* syRParen  */
  YYSYMBOL_syEqual = 64,                   /* syEqual  */
  YYSYMBOL_syCaret = 65,                   /* syCaret  */
  YYSYMBOL_syTilde = 66,                   /* syTilde  */
  YYSYMBOL_syLAngle = 67,                  /* syLAngle  */
  YYSYMBOL_syRAngle = 68,                  /* syRAngle  */
  YYSYMBOL_syLBrack = 69,                  /* syLBrack  */
  YYSYMBOL_syRBrack = 70,                  /* syRBrack  */
  YYSYMBOL_syBar = 71,                     /* syBar  */
  YYSYMBOL_syError = 72,                   /* syError  */
  YYSYMBOL_syNumber = 73,                  /* syNumber  */
  YYSYMBOL_sySymbolicType = 74,            /* sySymbolicType  */
  YYSYMBOL_syIdentifier = 75,              /* syIdentifier  */
  YYSYMBOL_syString = 76,                  /* syString  */
  YYSYMBOL_syQString = 77,                 /* syQString  */
  YYSYMBOL_syFileName = 78,                /* syFileName  */
  YYSYMBOL_syIPCFlag = 79,                 /* syIPCFlag  */
  YYSYMBOL_YYACCEPT = 80,                  /* $accept  */
  YYSYMBOL_Statements = 81,                /* Statements  */
  YYSYMBOL_Statement = 82,                 /* Statement  */
  YYSYMBOL_Subsystem = 83,                 /* Subsystem  */
  YYSYMBOL_SubsystemStart = 84,            /* SubsystemStart  */
  YYSYMBOL_SubsystemMods = 85,             /* SubsystemMods  */
  YYSYMBOL_SubsystemMod = 86,              /* SubsystemMod  */
  YYSYMBOL_SubsystemName = 87,             /* SubsystemName  */
  YYSYMBOL_SubsystemBase = 88,             /* SubsystemBase  */
  YYSYMBOL_MsgOption = 89,                 /* MsgOption  */
  YYSYMBOL_UserTypeLimit = 90,             /* UserTypeLimit  */
  YYSYMBOL_OnStackLimit = 91,              /* OnStackLimit  */
  YYSYMBOL_WaitTime = 92,                  /* WaitTime  */
  YYSYMBOL_Error = 93,                     /* Error  */
  YYSYMBOL_ServerPrefix = 94,              /* ServerPrefix  */
  YYSYMBOL_UserPrefix = 95,                /* UserPrefix  */
  YYSYMBOL_ServerDemux = 96,               /* ServerDemux  */
  YYSYMBOL_Import = 97,                    /* Import  */
  YYSYMBOL_ImportIndicant = 98,            /* ImportIndicant  */
  YYSYMBOL_RCSDecl = 99,                   /* RCSDecl  */
  YYSYMBOL_TypeDecl = 100,                 /* TypeDecl  */
  YYSYMBOL_NamedTypeSpec = 101,            /* NamedTypeSpec  */
  YYSYMBOL_TransTypeSpec = 102,            /* TransTypeSpec  */
  YYSYMBOL_TypeSpec = 103,                 /* TypeSpec  */
  YYSYMBOL_NativeTypeSpec = 104,           /* NativeTypeSpec  */
  YYSYMBOL_BasicTypeSpec = 105,            /* BasicTypeSpec  */
  YYSYMBOL_PrimIPCType = 106,              /* PrimIPCType  */
  YYSYMBOL_IPCType = 107,                  /* IPCType  */
  YYSYMBOL_PrevTypeSpec = 108,             /* PrevTypeSpec  */
  YYSYMBOL_VarArrayHead = 109,             /* VarArrayHead  */
  YYSYMBOL_ArrayHead = 110,                /* ArrayHead  */
  YYSYMBOL_StructHead = 111,               /* StructHead  */
  YYSYMBOL_CStringSpec = 112,              /* CStringSpec  */
  YYSYMBOL_TypePhrase = 113,               /* TypePhrase  */
  YYSYMBOL_IntExp = 114,                   /* IntExp  */
  YYSYMBOL_RoutineDecl = 115,              /* RoutineDecl  */
  YYSYMBOL_Routine = 116,                  /* Routine  */
  YYSYMBOL_SimpleRoutine = 117,            /* SimpleRoutine  */
  YYSYMBOL_Arguments = 118,                /* Arguments  */
  YYSYMBOL_ArgumentList = 119,             /* ArgumentList  */
  YYSYMBOL_Argument = 120,                 /* Argument  */
  YYSYMBOL_Trailer = 121,                  /* Trailer  */
  YYSYMBOL_Direction = 122,                /* Direction  */
  YYSYMBOL_TrImplKeyword = 123,            /* TrImplKeyword  */
  YYSYMBOL_ArgumentType = 124,             /* ArgumentType  */
  YYSYMBOL_IPCFlags = 125,                 /* IPCFlags  */
  YYSYMBOL_LookString = 126,               /* LookString  */
  YYSYMBOL_LookFileName = 127,             /* LookFileName  */
  YYSYMBOL_LookQString = 128               /* LookQString  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   260

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  80
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  126
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  241

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   334


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   195,   195,   196,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   219,   221,   222,   223,   224,
     228,   241,   253,   254,   257,   268,   276,   279,   282,   299,
     302,   306,   312,   320,   328,   336,   344,   352,   363,   364,
     365,   366,   367,   370,   380,   390,   394,   396,   416,   436,
     451,   465,   474,   486,   488,   490,   492,   494,   496,   498,
     500,   504,   506,   509,   513,   519,   526,   532,   536,   538,
     562,   566,   568,   570,   575,   579,   583,   585,   590,   592,
     596,   598,   600,   602,   604,   606,   611,   612,   615,   619,
     623,   625,   630,   632,   634,   639,   646,   667,   677,   678,
     679,   680,   681,   682,   683,   684,   685,   686,   687,   688,
     689,   690,   691,   692,   693,   694,   699,   700,   704,   710,
     712,   717,   718,   725,   734,   738,   742
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "sySkip", "syRoutine",
  "sySimpleRoutine", "sySubsystem", "syKernelUser", "syKernelServer",
  "syMsgOption", "syMsgSeqno", "syWaitTime", "sySendTime", "syNoWaitTime",
  "syErrorProc", "syServerPrefix", "syUserPrefix", "syServerDemux",
  "syRCSId", "syImport", "syUImport", "sySImport", "syIImport",
  "syDImport", "syIn", "syOut", "syInOut", "syUserImpl", "syServerImpl",
  "syRequestPort", "syReplyPort", "sySReplyPort", "syUReplyPort", "syType",
  "syArray", "syStruct", "syOf", "syInTran", "syOutTran", "syDestructor",
  "syCType", "syCUserType", "syUserTypeLimit", "syOnStackLimit",
  "syCServerType", "syPointerTo", "syPointerToIfNot", "syValueOf",
  "syCString", "sySecToken", "syUserSecToken", "syServerSecToken",
  "syAuditToken", "syUserAuditToken", "syServerAuditToken", "syColon",
  "sySemi", "syComma", "syPlus", "syMinus", "syStar", "syDiv", "syLParen",
  "syRParen", "syEqual", "syCaret", "syTilde", "syLAngle", "syRAngle",
  "syLBrack", "syRBrack", "syBar", "syError", "syNumber", "sySymbolicType",
  "syIdentifier", "syString", "syQString", "syFileName", "syIPCFlag",
  "$accept", "Statements", "Statement", "Subsystem", "SubsystemStart",
  "SubsystemMods", "SubsystemMod", "SubsystemName", "SubsystemBase",
  "MsgOption", "UserTypeLimit", "OnStackLimit", "WaitTime", "Error",
  "ServerPrefix", "UserPrefix", "ServerDemux", "Import", "ImportIndicant",
  "RCSDecl", "TypeDecl", "NamedTypeSpec", "TransTypeSpec", "TypeSpec",
  "NativeTypeSpec", "BasicTypeSpec", "PrimIPCType", "IPCType",
  "PrevTypeSpec", "VarArrayHead", "ArrayHead", "StructHead", "CStringSpec",
  "TypePhrase", "IntExp", "RoutineDecl", "Routine", "SimpleRoutine",
  "Arguments", "ArgumentList", "Argument", "Trailer", "Direction",
  "TrImplKeyword", "ArgumentType", "IPCFlags", "LookString",
  "LookFileName", "LookQString", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-140)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-127)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -140,    60,  -140,   -23,   -21,   -36,   -18,  -140,  -140,    -5,
      -3,    20,    40,    42,   -26,    54,  -140,  -140,    70,  -140,
      72,    83,    84,    85,    92,   111,   112,   113,   114,   116,
     117,   118,  -140,  -140,     8,    78,   124,  -140,  -140,   132,
     132,  -140,  -140,  -140,  -140,   134,  -140,  -140,  -140,  -140,
      -7,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,   119,   120,  -140,  -140,  -140,  -140,  -140,
     121,   125,    80,  -140,  -140,   -24,  -140,  -140,  -140,  -140,
     130,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,   137,   148,   149,   133,   135,
     138,   140,   144,   150,   151,   142,   -60,   -24,  -140,  -140,
    -140,    81,  -140,  -140,  -140,   143,  -140,  -140,   -24,   -24,
     -24,  -140,  -140,  -140,  -140,   126,   126,   160,   160,   -44,
      -6,   141,   141,   141,   -28,   161,  -140,   162,   164,   165,
     166,   167,   168,   -60,  -140,  -140,  -140,  -140,  -140,   -38,
    -140,  -140,   -50,    -6,   188,  -140,    26,   101,  -140,   -48,
     -45,   -35,   170,   123,    -6,   152,   153,   154,   155,   156,
     157,  -140,   134,  -140,  -140,   169,    -6,   197,    86,  -140,
      -6,    -6,    -6,    -6,   198,   199,  -140,  -140,   141,  -140,
      -6,  -140,   105,   163,   171,   174,  -140,  -140,  -140,   158,
     127,  -140,  -140,    53,    53,  -140,  -140,  -140,  -140,   -17,
     131,    31,   177,   178,   172,   173,   205,  -140,  -140,  -140,
     175,   176,   180,   179,  -140,   181,   182,  -140,  -140,  -140,
    -140
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,    21,    32,     0,
       0,     0,     0,     0,     0,     0,    18,     3,     0,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    86,    87,     0,     0,     0,    19,    15,     0,
       0,    33,    34,    35,    36,     0,    44,    29,    30,     4,
       0,     6,     7,     8,     5,     9,    10,    11,    12,    16,
      17,    13,    14,     0,     0,    38,    39,    40,    41,    42,
       0,     0,    98,    88,    89,     0,    24,    25,    26,    23,
       0,    28,    31,    37,    43,   108,   115,   106,   107,    99,
     100,   101,   117,   116,   102,   103,   104,   105,   109,   111,
     110,   112,   114,   113,    90,     0,    92,    93,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    66,    67,
      70,    45,    46,    60,    53,    68,    64,    54,     0,     0,
       0,    59,    27,    20,    91,    98,    98,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    57,     0,     0,     0,
       0,     0,     0,     0,    55,    56,    58,    94,    95,     0,
     121,    97,     0,     0,     0,    84,     0,     0,    78,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    69,   118,   119,   120,    96,     0,     0,     0,    71,
       0,     0,     0,     0,     0,     0,    61,    79,     0,    63,
       0,    76,   121,     0,     0,     0,    50,    51,    52,     0,
       0,    72,    85,    80,    81,    82,    83,    74,    75,     0,
       0,     0,     0,     0,     0,   122,     0,    62,    77,    65,
       0,     0,     0,     0,    73,     0,     0,    49,   123,    47,
      48
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,    89,  -140,   -86,    93,  -140,   100,   139,  -140,  -140,
    -140,  -140,  -140,  -139,  -138,  -140,  -140,  -140,   214,   -12,
    -140,  -140,  -140,  -140,   122,    55,  -140,  -140,  -140
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    17,    18,    19,    50,    79,    80,   133,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    70,    29,
      30,    46,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   169,   166,    31,    32,    33,    73,   105,
     106,   107,   108,   109,   160,   185,    34,    35,    36
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      76,    77,   167,   170,   171,   186,   173,   112,   113,   114,
     110,   111,   198,   118,   119,   196,   162,    63,   163,    64,
     187,   112,   113,   114,   115,   188,   164,   197,   199,   165,
     197,   146,   172,    37,   163,    38,   202,   182,   116,    39,
     197,   117,   154,   155,   156,   165,   227,    47,   210,   118,
     119,   120,   213,   214,   215,   216,   163,    40,   197,   219,
       2,     3,   220,     4,     5,     6,     7,   165,    78,  -124,
      41,  -124,    42,     8,     9,    10,    11,    12,  -126,  -125,
    -125,  -125,  -125,  -125,   190,   191,   192,   193,   209,    85,
      86,    87,    88,    13,   229,    43,   194,    65,    66,    67,
      68,    69,    14,    15,    89,    90,    91,    92,    93,    94,
      95,    96,    97,   192,   193,    44,    16,    45,   147,   148,
     149,   150,   151,   157,   158,   152,    49,    48,    51,    98,
      99,   100,   101,   102,   103,    85,    86,    87,    88,    52,
      53,    54,    71,   104,   190,   191,   192,   193,    55,   212,
      89,    90,    91,    92,    93,    94,    95,    96,    97,   190,
     191,   192,   193,   190,   191,   192,   193,    56,    57,    58,
      59,   195,    60,    61,    62,    98,    99,   100,   101,   102,
     103,   190,   191,   192,   193,   190,   191,   192,   193,   190,
     191,   192,   193,   201,    72,    81,    82,   226,    75,    83,
     134,   228,    84,   132,   135,   136,   141,   139,   137,   140,
     138,   144,   142,   143,   153,   159,   168,   175,   174,   176,
     177,   178,   179,   180,   189,   200,   209,   203,   204,   205,
     206,   207,   208,   211,   217,   218,   224,   225,   222,   230,
     231,   234,   233,   237,   239,   240,   223,   232,   183,   238,
     235,   236,   184,   181,    74,   145,     0,   221,     0,     0,
     161
};

static const yytype_int16 yycheck[] =
{
       7,     8,   140,   142,   143,    55,   144,    45,    46,    47,
      34,    35,    57,    73,    74,    63,    60,     9,    62,    11,
      70,    45,    46,    47,    48,   163,    70,    75,    63,    73,
      75,   117,    60,    56,    62,    56,   174,    75,    62,    75,
      75,    65,   128,   129,   130,    73,    63,    73,   186,    73,
      74,    75,   190,   191,   192,   193,    62,    75,    75,   198,
       0,     1,   200,     3,     4,     5,     6,    73,    75,     9,
      75,    11,    75,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    58,    59,    60,    61,    57,     9,
      10,    11,    12,    33,    63,    75,    70,    19,    20,    21,
      22,    23,    42,    43,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    60,    61,    75,    56,    75,    37,    38,
      39,    40,    41,   135,   136,    44,    56,    73,    56,    49,
      50,    51,    52,    53,    54,     9,    10,    11,    12,    56,
      56,    56,    18,    63,    58,    59,    60,    61,    56,    63,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    58,
      59,    60,    61,    58,    59,    60,    61,    56,    56,    56,
      56,    70,    56,    56,    56,    49,    50,    51,    52,    53,
      54,    58,    59,    60,    61,    58,    59,    60,    61,    58,
      59,    60,    61,    70,    62,    76,    76,    70,    64,    78,
      63,    70,    77,    73,    56,    56,    62,    69,    75,    69,
      75,    69,    62,    62,    71,    55,    75,    55,    57,    55,
      55,    55,    55,    55,    36,    55,    57,    75,    75,    75,
      75,    75,    75,    36,    36,    36,    62,    79,    75,    62,
      62,    36,    69,    63,    63,    63,    75,    75,   159,    70,
      75,    75,   159,   153,    40,   116,    -1,   202,    -1,    -1,
     138
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    81,     0,     1,     3,     4,     5,     6,    13,    14,
      15,    16,    17,    33,    42,    43,    56,    82,    83,    84,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    99,
     100,   115,   116,   117,   126,   127,   128,    56,    56,    75,
      75,    75,    75,    75,    75,    75,   101,    73,    73,    56,
      85,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,     9,    11,    19,    20,    21,    22,    23,
      98,    18,    62,   118,   118,    64,     7,     8,    75,    86,
      87,    76,    76,    78,    77,     9,    10,    11,    12,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    49,    50,
      51,    52,    53,    54,    63,   119,   120,   121,   122,   123,
      34,    35,    45,    46,    47,    48,    62,    65,    73,    74,
      75,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,    73,    88,    63,    56,    56,    75,    75,    69,
      69,    62,    62,    62,    69,   107,   103,    37,    38,    39,
      40,    41,    44,    71,   103,   103,   103,   119,   119,    55,
     124,   124,    60,    62,    70,    73,   114,   114,    75,   113,
     113,   113,    60,   114,    57,    55,    55,    55,    55,    55,
      55,   106,    75,   101,   104,   125,    55,    70,   114,    36,
      58,    59,    60,    61,    70,    70,    63,    75,    57,    63,
      55,    70,   114,    75,    75,    75,    75,    75,    75,    57,
     114,    36,    63,   114,   114,   114,   114,    36,    36,   113,
     114,   125,    75,    75,    62,    79,    70,    63,    70,    63,
      62,    62,    75,    69,    36,    75,    75,    63,    70,    63,
      63
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    80,    81,    81,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      83,    84,    85,    85,    86,    86,    87,    88,    89,    90,
      91,    92,    92,    93,    94,    95,    96,    97,    98,    98,
      98,    98,    98,    99,   100,   101,   102,   102,   102,   102,
     102,   102,   102,   103,   103,   103,   103,   103,   103,   103,
     103,   104,   104,   104,   105,   105,   106,   106,   107,   107,
     108,   109,   109,   109,   110,   111,   112,   112,   113,   113,
     114,   114,   114,   114,   114,   114,   115,   115,   116,   117,
     118,   118,   119,   119,   119,   119,   120,   121,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   123,   123,   124,   124,
     124,   125,   125,   125,   126,   127,   128
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     2,
       4,     1,     0,     2,     1,     1,     1,     1,     3,     2,
       2,     3,     1,     2,     2,     2,     2,     3,     1,     1,
       1,     1,     1,     3,     2,     3,     1,     8,     8,     7,
       4,     4,     4,     1,     1,     2,     2,     2,     2,     1,
       1,     4,     6,     4,     1,     6,     1,     1,     1,     3,
       1,     4,     5,     7,     5,     5,     4,     6,     1,     2,
       3,     3,     3,     3,     1,     3,     1,     1,     3,     3,
       2,     3,     1,     1,     3,     3,     4,     3,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     0,     3,     5,     0,     0,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 14: /* Statement: RoutineDecl sySemi  */
#line 210 "parser.y"
{
    register statement_t *st = stAlloc();

    st->stKind = skRoutine;
    st->stRoutine = (yyvsp[-1].routine);
    rtCheckRoutine((yyvsp[-1].routine));
    if (BeVerbose)
	rtPrintRoutine((yyvsp[-1].routine));
}
#line 1400 "parser.c"
    break;

  case 15: /* Statement: sySkip sySemi  */
#line 220 "parser.y"
                                { rtSkip(); }
#line 1406 "parser.c"
    break;

  case 19: /* Statement: error sySemi  */
#line 225 "parser.y"
                                { yyerrok; }
#line 1412 "parser.c"
    break;

  case 20: /* Subsystem: SubsystemStart SubsystemMods SubsystemName SubsystemBase  */
#line 230 "parser.y"
{
    if (BeVerbose)
    {
	printf("Subsystem %s: base = %u%s%s\n\n",
	       SubsystemName, SubsystemBase,
	       IsKernelUser ? ", KernelUser" : "",
	       IsKernelServer ? ", KernelServer" : "");
    }
}
#line 1426 "parser.c"
    break;

  case 21: /* SubsystemStart: sySubsystem  */
#line 242 "parser.y"
{
    if (SubsystemName != strNULL)
    {
	warn("previous Subsystem decl (of %s) will be ignored", SubsystemName);
	IsKernelUser = FALSE;
	IsKernelServer = FALSE;
	strfree(SubsystemName);
    }
}
#line 1440 "parser.c"
    break;

  case 24: /* SubsystemMod: syKernelUser  */
#line 258 "parser.y"
{
    if (IsKernelUser)
	warn("duplicate KernelUser keyword");
    if (!UseMsgRPC) 
    {
	warn("with KernelUser the -R option is meaningless");
	UseMsgRPC = TRUE;
    }
    IsKernelUser = TRUE;
}
#line 1455 "parser.c"
    break;

  case 25: /* SubsystemMod: syKernelServer  */
#line 269 "parser.y"
{
    if (IsKernelServer)
	warn("duplicate KernelServer keyword");
    IsKernelServer = TRUE;
}
#line 1465 "parser.c"
    break;

  case 26: /* SubsystemName: syIdentifier  */
#line 276 "parser.y"
                                                { SubsystemName = (yyvsp[0].identifier); }
#line 1471 "parser.c"
    break;

  case 27: /* SubsystemBase: syNumber  */
#line 279 "parser.y"
                                                { SubsystemBase = (yyvsp[0].number); }
#line 1477 "parser.c"
    break;

  case 28: /* MsgOption: LookString syMsgOption syString  */
#line 283 "parser.y"
{
    if (streql((yyvsp[0].string), "MACH_MSG_OPTION_NONE"))
    {
	MsgOption = strNULL;
	if (BeVerbose)
	    printf("MsgOption: canceled\n\n");
    }
    else
    {
	MsgOption = (yyvsp[0].string);
	if (BeVerbose)
	    printf("MsgOption %s\n\n",(yyvsp[0].string));
    }
}
#line 1496 "parser.c"
    break;

  case 29: /* UserTypeLimit: syUserTypeLimit syNumber  */
#line 300 "parser.y"
                                {UserTypeLimit = (yyvsp[0].number); }
#line 1502 "parser.c"
    break;

  case 30: /* OnStackLimit: syOnStackLimit syNumber  */
#line 303 "parser.y"
                                {MaxMessSizeOnStack = (yyvsp[0].number); }
#line 1508 "parser.c"
    break;

  case 31: /* WaitTime: LookString syWaitTime syString  */
#line 307 "parser.y"
{
    WaitTime = (yyvsp[0].string);
    if (BeVerbose)
	printf("WaitTime %s\n\n", WaitTime);
}
#line 1518 "parser.c"
    break;

  case 32: /* WaitTime: syNoWaitTime  */
#line 313 "parser.y"
{
    WaitTime = strNULL;
    if (BeVerbose)
	printf("NoWaitTime\n\n");
}
#line 1528 "parser.c"
    break;

  case 33: /* Error: syErrorProc syIdentifier  */
#line 321 "parser.y"
{
    ErrorProc = (yyvsp[0].identifier);
    if (BeVerbose)
	printf("ErrorProc %s\n\n", ErrorProc);
}
#line 1538 "parser.c"
    break;

  case 34: /* ServerPrefix: syServerPrefix syIdentifier  */
#line 329 "parser.y"
{
    ServerPrefix = (yyvsp[0].identifier);
    if (BeVerbose)
	printf("ServerPrefix %s\n\n", ServerPrefix);
}
#line 1548 "parser.c"
    break;

  case 35: /* UserPrefix: syUserPrefix syIdentifier  */
#line 337 "parser.y"
{
    UserPrefix = (yyvsp[0].identifier);
    if (BeVerbose)
	printf("UserPrefix %s\n\n", UserPrefix);
}
#line 1558 "parser.c"
    break;

  case 36: /* ServerDemux: syServerDemux syIdentifier  */
#line 345 "parser.y"
{
    ServerDemux = (yyvsp[0].identifier);
    if (BeVerbose)
	printf("ServerDemux %s\n\n", ServerDemux);
}
#line 1568 "parser.c"
    break;

  case 37: /* Import: LookFileName ImportIndicant syFileName  */
#line 353 "parser.y"
{
    register statement_t *st = stAlloc();
    st->stKind = (yyvsp[-1].statement_kind);
    st->stFileName = (yyvsp[0].string);

    if (BeVerbose)
	printf("%s %s\n\n", import_name((yyvsp[-1].statement_kind)), (yyvsp[0].string));
}
#line 1581 "parser.c"
    break;

  case 38: /* ImportIndicant: syImport  */
#line 363 "parser.y"
                                                { (yyval.statement_kind) = skImport; }
#line 1587 "parser.c"
    break;

  case 39: /* ImportIndicant: syUImport  */
#line 364 "parser.y"
                                                { (yyval.statement_kind) = skUImport; }
#line 1593 "parser.c"
    break;

  case 40: /* ImportIndicant: sySImport  */
#line 365 "parser.y"
                                                { (yyval.statement_kind) = skSImport; }
#line 1599 "parser.c"
    break;

  case 41: /* ImportIndicant: syIImport  */
#line 366 "parser.y"
                                                { (yyval.statement_kind) = skIImport; }
#line 1605 "parser.c"
    break;

  case 42: /* ImportIndicant: syDImport  */
#line 367 "parser.y"
                                                { (yyval.statement_kind) = skDImport; }
#line 1611 "parser.c"
    break;

  case 43: /* RCSDecl: LookQString syRCSId syQString  */
#line 371 "parser.y"
{
    if (RCSId != strNULL)
	warn("previous RCS decl will be ignored");
    if (BeVerbose)
	printf("RCSId %s\n\n", (yyvsp[0].string));
    RCSId = (yyvsp[0].string);
}
#line 1623 "parser.c"
    break;

  case 44: /* TypeDecl: syType NamedTypeSpec  */
#line 381 "parser.y"
{
    register identifier_t name = (yyvsp[0].type)->itName;

    if (itLookUp(name) != itNULL)
	warn("overriding previous definition of %s", name);
    itInsert(name, (yyvsp[0].type));
}
#line 1635 "parser.c"
    break;

  case 45: /* NamedTypeSpec: syIdentifier syEqual TransTypeSpec  */
#line 391 "parser.y"
                                { itTypeDecl((yyvsp[-2].identifier), (yyval.type) = (yyvsp[0].type)); }
#line 1641 "parser.c"
    break;

  case 46: /* TransTypeSpec: TypeSpec  */
#line 395 "parser.y"
                                { (yyval.type) = itResetType((yyvsp[0].type)); }
#line 1647 "parser.c"
    break;

  case 47: /* TransTypeSpec: TransTypeSpec syInTran syColon syIdentifier syIdentifier syLParen syIdentifier syRParen  */
#line 398 "parser.y"
{
    (yyval.type) = (yyvsp[-7].type);

    if (((yyval.type)->itTransType != strNULL) && !streql((yyval.type)->itTransType, (yyvsp[-4].identifier)))
	warn("conflicting translation types (%s, %s)",
	     (yyval.type)->itTransType, (yyvsp[-4].identifier));
    (yyval.type)->itTransType = (yyvsp[-4].identifier);

    if (((yyval.type)->itInTrans != strNULL) && !streql((yyval.type)->itInTrans, (yyvsp[-3].identifier)))
	warn("conflicting in-translation functions (%s, %s)",
	     (yyval.type)->itInTrans, (yyvsp[-3].identifier));
    (yyval.type)->itInTrans = (yyvsp[-3].identifier);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[-1].identifier)))
	warn("conflicting server types (%s, %s)",
	     (yyval.type)->itServerType, (yyvsp[-1].identifier));
    (yyval.type)->itServerType = (yyvsp[-1].identifier);
}
#line 1670 "parser.c"
    break;

  case 48: /* TransTypeSpec: TransTypeSpec syOutTran syColon syIdentifier syIdentifier syLParen syIdentifier syRParen  */
#line 418 "parser.y"
{
    (yyval.type) = (yyvsp[-7].type);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[-4].identifier)))
	warn("conflicting server types (%s, %s)",
	     (yyval.type)->itServerType, (yyvsp[-4].identifier));
    (yyval.type)->itServerType = (yyvsp[-4].identifier);

    if (((yyval.type)->itOutTrans != strNULL) && !streql((yyval.type)->itOutTrans, (yyvsp[-3].identifier)))
	warn("conflicting out-translation functions (%s, %s)",
	     (yyval.type)->itOutTrans, (yyvsp[-3].identifier));
    (yyval.type)->itOutTrans = (yyvsp[-3].identifier);

    if (((yyval.type)->itTransType != strNULL) && !streql((yyval.type)->itTransType, (yyvsp[-1].identifier)))
	warn("conflicting translation types (%s, %s)",
	     (yyval.type)->itTransType, (yyvsp[-1].identifier));
    (yyval.type)->itTransType = (yyvsp[-1].identifier);
}
#line 1693 "parser.c"
    break;

  case 49: /* TransTypeSpec: TransTypeSpec syDestructor syColon syIdentifier syLParen syIdentifier syRParen  */
#line 438 "parser.y"
{
    (yyval.type) = (yyvsp[-6].type);

    if (((yyval.type)->itDestructor != strNULL) && !streql((yyval.type)->itDestructor, (yyvsp[-3].identifier)))
	warn("conflicting destructor functions (%s, %s)",
	     (yyval.type)->itDestructor, (yyvsp[-3].identifier));
    (yyval.type)->itDestructor = (yyvsp[-3].identifier);

    if (((yyval.type)->itTransType != strNULL) && !streql((yyval.type)->itTransType, (yyvsp[-1].identifier)))
	warn("conflicting translation types (%s, %s)",
	     (yyval.type)->itTransType, (yyvsp[-1].identifier));
    (yyval.type)->itTransType = (yyvsp[-1].identifier);
}
#line 1711 "parser.c"
    break;

  case 50: /* TransTypeSpec: TransTypeSpec syCType syColon syIdentifier  */
#line 452 "parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itUserType != strNULL) && !streql((yyval.type)->itUserType, (yyvsp[0].identifier)))
	warn("conflicting user types (%s, %s)",
	     (yyval.type)->itUserType, (yyvsp[0].identifier));
    (yyval.type)->itUserType = (yyvsp[0].identifier);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[0].identifier)))
	warn("conflicting server types (%s, %s)",
	     (yyval.type)->itServerType, (yyvsp[0].identifier));
    (yyval.type)->itServerType = (yyvsp[0].identifier);
}
#line 1729 "parser.c"
    break;

  case 51: /* TransTypeSpec: TransTypeSpec syCUserType syColon syIdentifier  */
#line 466 "parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itUserType != strNULL) && !streql((yyval.type)->itUserType, (yyvsp[0].identifier)))
	warn("conflicting user types (%s, %s)",
	     (yyval.type)->itUserType, (yyvsp[0].identifier));
    (yyval.type)->itUserType = (yyvsp[0].identifier);
}
#line 1742 "parser.c"
    break;

  case 52: /* TransTypeSpec: TransTypeSpec syCServerType syColon syIdentifier  */
#line 476 "parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[0].identifier)))
	warn("conflicting server types (%s, %s)",
	     (yyval.type)->itServerType, (yyvsp[0].identifier));
    (yyval.type)->itServerType = (yyvsp[0].identifier);
}
#line 1755 "parser.c"
    break;

  case 53: /* TypeSpec: BasicTypeSpec  */
#line 487 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1761 "parser.c"
    break;

  case 54: /* TypeSpec: PrevTypeSpec  */
#line 489 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1767 "parser.c"
    break;

  case 55: /* TypeSpec: VarArrayHead TypeSpec  */
#line 491 "parser.y"
                                { (yyval.type) = itVarArrayDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1773 "parser.c"
    break;

  case 56: /* TypeSpec: ArrayHead TypeSpec  */
#line 493 "parser.y"
                                { (yyval.type) = itArrayDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1779 "parser.c"
    break;

  case 57: /* TypeSpec: syCaret TypeSpec  */
#line 495 "parser.y"
                                { (yyval.type) = itPtrDecl((yyvsp[0].type)); }
#line 1785 "parser.c"
    break;

  case 58: /* TypeSpec: StructHead TypeSpec  */
#line 497 "parser.y"
                                { (yyval.type) = itStructDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1791 "parser.c"
    break;

  case 59: /* TypeSpec: CStringSpec  */
#line 499 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1797 "parser.c"
    break;

  case 60: /* TypeSpec: NativeTypeSpec  */
#line 501 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1803 "parser.c"
    break;

  case 61: /* NativeTypeSpec: syPointerTo syLParen TypePhrase syRParen  */
#line 505 "parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-1].identifier), TRUE, 0); }
#line 1809 "parser.c"
    break;

  case 62: /* NativeTypeSpec: syPointerToIfNot syLParen TypePhrase syComma TypePhrase syRParen  */
#line 508 "parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-3].identifier), TRUE, (yyvsp[-1].identifier)); }
#line 1815 "parser.c"
    break;

  case 63: /* NativeTypeSpec: syValueOf syLParen TypePhrase syRParen  */
#line 510 "parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-1].identifier), FALSE, 0); }
#line 1821 "parser.c"
    break;

  case 64: /* BasicTypeSpec: IPCType  */
#line 514 "parser.y"
{
    (yyval.type) = itShortDecl((yyvsp[0].symtype).innumber, (yyvsp[0].symtype).instr,
		     (yyvsp[0].symtype).outnumber, (yyvsp[0].symtype).outstr,
		     (yyvsp[0].symtype).size);
}
#line 1831 "parser.c"
    break;

  case 65: /* BasicTypeSpec: syLParen IPCType syComma IntExp IPCFlags syRParen  */
#line 521 "parser.y"
{
    error("Long form type declarations aren't allowed anylonger\n");
}
#line 1839 "parser.c"
    break;

  case 66: /* PrimIPCType: syNumber  */
#line 527 "parser.y"
{
    (yyval.symtype).innumber = (yyval.symtype).outnumber = (yyvsp[0].number);
    (yyval.symtype).instr = (yyval.symtype).outstr = strNULL;
    (yyval.symtype).size = 0;
}
#line 1849 "parser.c"
    break;

  case 67: /* PrimIPCType: sySymbolicType  */
#line 533 "parser.y"
                                { (yyval.symtype) = (yyvsp[0].symtype); }
#line 1855 "parser.c"
    break;

  case 68: /* IPCType: PrimIPCType  */
#line 537 "parser.y"
                                { (yyval.symtype) = (yyvsp[0].symtype); }
#line 1861 "parser.c"
    break;

  case 69: /* IPCType: PrimIPCType syBar PrimIPCType  */
#line 539 "parser.y"
{
    if ((yyvsp[-2].symtype).size != (yyvsp[0].symtype).size)
    {
	if ((yyvsp[-2].symtype).size == 0)
	    (yyval.symtype).size = (yyvsp[0].symtype).size;
	else if ((yyvsp[0].symtype).size == 0)
	    (yyval.symtype).size = (yyvsp[-2].symtype).size;
	else
	{
	    error("sizes in IPCTypes (%d, %d) aren't equal",
		  (yyvsp[-2].symtype).size, (yyvsp[0].symtype).size);
	    (yyval.symtype).size = 0;
	}
    }
    else
	(yyval.symtype).size = (yyvsp[-2].symtype).size;
    (yyval.symtype).innumber = (yyvsp[-2].symtype).innumber;
    (yyval.symtype).instr = (yyvsp[-2].symtype).instr;
    (yyval.symtype).outnumber = (yyvsp[0].symtype).outnumber;
    (yyval.symtype).outstr = (yyvsp[0].symtype).outstr;
}
#line 1887 "parser.c"
    break;

  case 70: /* PrevTypeSpec: syIdentifier  */
#line 563 "parser.y"
                                { (yyval.type) = itPrevDecl((yyvsp[0].identifier)); }
#line 1893 "parser.c"
    break;

  case 71: /* VarArrayHead: syArray syLBrack syRBrack syOf  */
#line 567 "parser.y"
                                { (yyval.number) = 0; }
#line 1899 "parser.c"
    break;

  case 72: /* VarArrayHead: syArray syLBrack syStar syRBrack syOf  */
#line 569 "parser.y"
                                { (yyval.number) = 0; }
#line 1905 "parser.c"
    break;

  case 73: /* VarArrayHead: syArray syLBrack syStar syColon IntExp syRBrack syOf  */
#line 572 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 1911 "parser.c"
    break;

  case 74: /* ArrayHead: syArray syLBrack IntExp syRBrack syOf  */
#line 576 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 1917 "parser.c"
    break;

  case 75: /* StructHead: syStruct syLBrack IntExp syRBrack syOf  */
#line 580 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 1923 "parser.c"
    break;

  case 76: /* CStringSpec: syCString syLBrack IntExp syRBrack  */
#line 584 "parser.y"
                                { (yyval.type) = itCStringDecl((yyvsp[-1].number), FALSE); }
#line 1929 "parser.c"
    break;

  case 77: /* CStringSpec: syCString syLBrack syStar syColon IntExp syRBrack  */
#line 587 "parser.y"
                                { (yyval.type) = itCStringDecl((yyvsp[-1].number), TRUE); }
#line 1935 "parser.c"
    break;

  case 78: /* TypePhrase: syIdentifier  */
#line 591 "parser.y"
                                { (yyval.identifier) = (yyvsp[0].identifier); }
#line 1941 "parser.c"
    break;

  case 79: /* TypePhrase: TypePhrase syIdentifier  */
#line 593 "parser.y"
                                { (yyval.identifier) = strphrase((yyvsp[-1].identifier), (yyvsp[0].identifier)); strfree((yyvsp[0].identifier)); }
#line 1947 "parser.c"
    break;

  case 80: /* IntExp: IntExp syPlus IntExp  */
#line 597 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number) + (yyvsp[0].number);	}
#line 1953 "parser.c"
    break;

  case 81: /* IntExp: IntExp syMinus IntExp  */
#line 599 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number) - (yyvsp[0].number);	}
#line 1959 "parser.c"
    break;

  case 82: /* IntExp: IntExp syStar IntExp  */
#line 601 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number) * (yyvsp[0].number);	}
#line 1965 "parser.c"
    break;

  case 83: /* IntExp: IntExp syDiv IntExp  */
#line 603 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number) / (yyvsp[0].number);	}
#line 1971 "parser.c"
    break;

  case 84: /* IntExp: syNumber  */
#line 605 "parser.y"
                                { (yyval.number) = (yyvsp[0].number);	}
#line 1977 "parser.c"
    break;

  case 85: /* IntExp: syLParen IntExp syRParen  */
#line 607 "parser.y"
                                { (yyval.number) = (yyvsp[-1].number);	}
#line 1983 "parser.c"
    break;

  case 86: /* RoutineDecl: Routine  */
#line 611 "parser.y"
                                                        { (yyval.routine) = (yyvsp[0].routine); }
#line 1989 "parser.c"
    break;

  case 87: /* RoutineDecl: SimpleRoutine  */
#line 612 "parser.y"
                                                        { (yyval.routine) = (yyvsp[0].routine); }
#line 1995 "parser.c"
    break;

  case 88: /* Routine: syRoutine syIdentifier Arguments  */
#line 616 "parser.y"
                                { (yyval.routine) = rtMakeRoutine((yyvsp[-1].identifier), (yyvsp[0].argument)); }
#line 2001 "parser.c"
    break;

  case 89: /* SimpleRoutine: sySimpleRoutine syIdentifier Arguments  */
#line 620 "parser.y"
                                { (yyval.routine) = rtMakeSimpleRoutine((yyvsp[-1].identifier), (yyvsp[0].argument)); }
#line 2007 "parser.c"
    break;

  case 90: /* Arguments: syLParen syRParen  */
#line 624 "parser.y"
                                { (yyval.argument) = argNULL; }
#line 2013 "parser.c"
    break;

  case 91: /* Arguments: syLParen ArgumentList syRParen  */
#line 626 "parser.y"
                                { (yyval.argument) = (yyvsp[-1].argument); }
#line 2019 "parser.c"
    break;

  case 92: /* ArgumentList: Argument  */
#line 631 "parser.y"
                                { (yyval.argument) = (yyvsp[0].argument); }
#line 2025 "parser.c"
    break;

  case 93: /* ArgumentList: Trailer  */
#line 633 "parser.y"
                                { (yyval.argument) = (yyvsp[0].argument); }
#line 2031 "parser.c"
    break;

  case 94: /* ArgumentList: Argument sySemi ArgumentList  */
#line 635 "parser.y"
{
    (yyval.argument) = (yyvsp[-2].argument);
    (yyval.argument)->argNext = (yyvsp[0].argument);
}
#line 2040 "parser.c"
    break;

  case 95: /* ArgumentList: Trailer sySemi ArgumentList  */
#line 640 "parser.y"
{
    (yyval.argument) = (yyvsp[-2].argument);
    (yyval.argument)->argNext = (yyvsp[0].argument);
}
#line 2049 "parser.c"
    break;

  case 96: /* Argument: Direction syIdentifier ArgumentType IPCFlags  */
#line 647 "parser.y"
{
    (yyval.argument) = argAlloc();
    (yyval.argument)->argKind = (yyvsp[-3].direction);
    (yyval.argument)->argName = (yyvsp[-2].identifier);
    (yyval.argument)->argType = (yyvsp[-1].type);
    (yyval.argument)->argFlags = (yyvsp[0].flag);
    if ((yyvsp[-1].type) && (yyvsp[-1].type)->itNative)
    {
        if ((yyvsp[-3].direction) != akIn && (yyvsp[-3].direction) != akOut && (yyvsp[-3].direction) != akInOut)
	    error("Illegal direction specified");
       
        if (!((yyvsp[-1].type)->itNativePointer) && (yyvsp[-3].direction) != akIn)
	    error("ValueOf only valid for in");

        if (((yyvsp[-1].type)->itBadValue) != NULL && (yyvsp[-3].direction) != akIn)
	    error("PointerToIfNot only valid for in");
    }
}
#line 2072 "parser.c"
    break;

  case 97: /* Trailer: TrImplKeyword syIdentifier ArgumentType  */
#line 668 "parser.y"
{
    (yyval.argument) = argAlloc();
    (yyval.argument)->argKind = (yyvsp[-2].direction);
    (yyval.argument)->argName = (yyvsp[-1].identifier);
    (yyval.argument)->argType = (yyvsp[0].type);
}
#line 2083 "parser.c"
    break;

  case 98: /* Direction: %empty  */
#line 677 "parser.y"
                                                { (yyval.direction) = akNone; }
#line 2089 "parser.c"
    break;

  case 99: /* Direction: syIn  */
#line 678 "parser.y"
                                                { (yyval.direction) = akIn; }
#line 2095 "parser.c"
    break;

  case 100: /* Direction: syOut  */
#line 679 "parser.y"
                                                { (yyval.direction) = akOut; }
#line 2101 "parser.c"
    break;

  case 101: /* Direction: syInOut  */
#line 680 "parser.y"
                                                { (yyval.direction) = akInOut; }
#line 2107 "parser.c"
    break;

  case 102: /* Direction: syRequestPort  */
#line 681 "parser.y"
                                                { (yyval.direction) = akRequestPort; }
#line 2113 "parser.c"
    break;

  case 103: /* Direction: syReplyPort  */
#line 682 "parser.y"
                                                { (yyval.direction) = akReplyPort; }
#line 2119 "parser.c"
    break;

  case 104: /* Direction: sySReplyPort  */
#line 683 "parser.y"
                                                { (yyval.direction) = akSReplyPort; }
#line 2125 "parser.c"
    break;

  case 105: /* Direction: syUReplyPort  */
#line 684 "parser.y"
                                                { (yyval.direction) = akUReplyPort; }
#line 2131 "parser.c"
    break;

  case 106: /* Direction: syWaitTime  */
#line 685 "parser.y"
                                                { (yyval.direction) = akWaitTime; }
#line 2137 "parser.c"
    break;

  case 107: /* Direction: sySendTime  */
#line 686 "parser.y"
                                                { (yyval.direction) = akSendTime; }
#line 2143 "parser.c"
    break;

  case 108: /* Direction: syMsgOption  */
#line 687 "parser.y"
                                                { (yyval.direction) = akMsgOption; }
#line 2149 "parser.c"
    break;

  case 109: /* Direction: sySecToken  */
#line 688 "parser.y"
                                                        { (yyval.direction) = akSecToken; }
#line 2155 "parser.c"
    break;

  case 110: /* Direction: syServerSecToken  */
#line 689 "parser.y"
                                                        { (yyval.direction) = akServerSecToken; }
#line 2161 "parser.c"
    break;

  case 111: /* Direction: syUserSecToken  */
#line 690 "parser.y"
                                                        { (yyval.direction) = akUserSecToken; }
#line 2167 "parser.c"
    break;

  case 112: /* Direction: syAuditToken  */
#line 691 "parser.y"
                                                        { (yyval.direction) = akAuditToken; }
#line 2173 "parser.c"
    break;

  case 113: /* Direction: syServerAuditToken  */
#line 692 "parser.y"
                                                        { (yyval.direction) = akServerAuditToken; }
#line 2179 "parser.c"
    break;

  case 114: /* Direction: syUserAuditToken  */
#line 693 "parser.y"
                                                        { (yyval.direction) = akUserAuditToken; }
#line 2185 "parser.c"
    break;

  case 115: /* Direction: syMsgSeqno  */
#line 694 "parser.y"
                                                { (yyval.direction) = akMsgSeqno; }
#line 2191 "parser.c"
    break;

  case 116: /* TrImplKeyword: syServerImpl  */
#line 699 "parser.y"
                                                { (yyval.direction) = akServerImpl; }
#line 2197 "parser.c"
    break;

  case 117: /* TrImplKeyword: syUserImpl  */
#line 700 "parser.y"
                                                { (yyval.direction) = akUserImpl; }
#line 2203 "parser.c"
    break;

  case 118: /* ArgumentType: syColon syIdentifier  */
#line 705 "parser.y"
{
    (yyval.type) = itLookUp((yyvsp[0].identifier));
    if ((yyval.type) == itNULL)
	error("type '%s' not defined", (yyvsp[0].identifier));
}
#line 2213 "parser.c"
    break;

  case 119: /* ArgumentType: syColon NamedTypeSpec  */
#line 711 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 2219 "parser.c"
    break;

  case 120: /* ArgumentType: syColon NativeTypeSpec  */
#line 713 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 2225 "parser.c"
    break;

  case 121: /* IPCFlags: %empty  */
#line 717 "parser.y"
                                { (yyval.flag) = flNone; }
#line 2231 "parser.c"
    break;

  case 122: /* IPCFlags: IPCFlags syComma syIPCFlag  */
#line 719 "parser.y"
{
    if ((yyvsp[-2].flag) & (yyvsp[0].flag))
	warn("redundant IPC flag ignored");
    else
	(yyval.flag) = (yyvsp[-2].flag) | (yyvsp[0].flag);
}
#line 2242 "parser.c"
    break;

  case 123: /* IPCFlags: IPCFlags syComma syIPCFlag syLBrack syRBrack  */
#line 726 "parser.y"
{
    if ((yyvsp[-2].flag) != flDealloc)
	warn("only Dealloc is variable");
    else
	(yyval.flag) = (yyvsp[-4].flag) | flMaybeDealloc;
}
#line 2253 "parser.c"
    break;

  case 124: /* LookString: %empty  */
#line 734 "parser.y"
                                { LookString(); }
#line 2259 "parser.c"
    break;

  case 125: /* LookFileName: %empty  */
#line 738 "parser.y"
                                { LookFileName(); }
#line 2265 "parser.c"
    break;

  case 126: /* LookQString: %empty  */
#line 742 "parser.y"
                                { LookQString(); }
#line 2271 "parser.c"
    break;


#line 2275 "parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 745 "parser.y"


void
yyerror(s)
    char *s;
{
    error(s);
}

static char *
import_name(sk)
    statement_kind_t sk;
{
    switch (sk)
    {
      case skImport:
	return "Import";
      case skSImport:
	return "SImport";
      case skUImport:
	return "UImport";
      case skIImport:
	return "IImport";
      case skDImport:
	return "DImport";
      default:
	fatal("import_name(%d): not import statement", (int) sk);
	/*NOTREACHED*/
        return strNULL;
    }
}
