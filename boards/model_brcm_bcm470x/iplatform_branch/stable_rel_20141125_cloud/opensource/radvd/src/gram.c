/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_INTERFACE = 258,
     T_PREFIX = 259,
     T_ROUTE = 260,
     T_RDNSS = 261,
     T_DNSSL = 262,
     T_CLIENTS = 263,
     STRING = 264,
     NUMBER = 265,
     SIGNEDNUMBER = 266,
     DECIMAL = 267,
     SWITCH = 268,
     IPV6ADDR = 269,
     INFINITY = 270,
     T_IgnoreIfMissing = 271,
     T_AdvSendAdvert = 272,
     T_MaxRtrAdvInterval = 273,
     T_MinRtrAdvInterval = 274,
     T_MinDelayBetweenRAs = 275,
     T_AdvManagedFlag = 276,
     T_AdvOtherConfigFlag = 277,
     T_AdvLinkMTU = 278,
     T_AdvReachableTime = 279,
     T_AdvRetransTimer = 280,
     T_AdvCurHopLimit = 281,
     T_AdvDefaultLifetime = 282,
     T_AdvDefaultPreference = 283,
     T_AdvSourceLLAddress = 284,
     T_AdvOnLink = 285,
     T_AdvAutonomous = 286,
     T_AdvValidLifetime = 287,
     T_AdvPreferredLifetime = 288,
     T_AdvRouterAddr = 289,
     T_AdvHomeAgentFlag = 290,
     T_AdvIntervalOpt = 291,
     T_AdvHomeAgentInfo = 292,
     T_Base6to4Interface = 293,
     T_UnicastOnly = 294,
     T_HomeAgentPreference = 295,
     T_HomeAgentLifetime = 296,
     T_AdvRoutePreference = 297,
     T_AdvRouteLifetime = 298,
     T_AdvRDNSSPreference = 299,
     T_AdvRDNSSOpenFlag = 300,
     T_AdvRDNSSLifetime = 301,
     T_AdvDNSSLLifetime = 302,
     T_AdvMobRtrSupportFlag = 303,
     T_BAD_TOKEN = 304
   };
#endif
/* Tokens.  */
#define T_INTERFACE 258
#define T_PREFIX 259
#define T_ROUTE 260
#define T_RDNSS 261
#define T_DNSSL 262
#define T_CLIENTS 263
#define STRING 264
#define NUMBER 265
#define SIGNEDNUMBER 266
#define DECIMAL 267
#define SWITCH 268
#define IPV6ADDR 269
#define INFINITY 270
#define T_IgnoreIfMissing 271
#define T_AdvSendAdvert 272
#define T_MaxRtrAdvInterval 273
#define T_MinRtrAdvInterval 274
#define T_MinDelayBetweenRAs 275
#define T_AdvManagedFlag 276
#define T_AdvOtherConfigFlag 277
#define T_AdvLinkMTU 278
#define T_AdvReachableTime 279
#define T_AdvRetransTimer 280
#define T_AdvCurHopLimit 281
#define T_AdvDefaultLifetime 282
#define T_AdvDefaultPreference 283
#define T_AdvSourceLLAddress 284
#define T_AdvOnLink 285
#define T_AdvAutonomous 286
#define T_AdvValidLifetime 287
#define T_AdvPreferredLifetime 288
#define T_AdvRouterAddr 289
#define T_AdvHomeAgentFlag 290
#define T_AdvIntervalOpt 291
#define T_AdvHomeAgentInfo 292
#define T_Base6to4Interface 293
#define T_UnicastOnly 294
#define T_HomeAgentPreference 295
#define T_HomeAgentLifetime 296
#define T_AdvRoutePreference 297
#define T_AdvRouteLifetime 298
#define T_AdvRDNSSPreference 299
#define T_AdvRDNSSOpenFlag 300
#define T_AdvRDNSSLifetime 301
#define T_AdvDNSSLLifetime 302
#define T_AdvMobRtrSupportFlag 303
#define T_BAD_TOKEN 304




/* Copy the first part of user declarations.  */
#line 16 "gram.y"

#include "config.h"
#include "includes.h"
#include "radvd.h"
#include "defaults.h"

extern struct Interface *IfaceList;
struct Interface *iface = NULL;
struct AdvPrefix *prefix = NULL;
struct AdvRoute *route = NULL;
struct AdvRDNSS *rdnss = NULL;
struct AdvDNSSL *dnssl = NULL;

extern char *conf_file;
extern int num_lines;
extern char *yytext;
extern int sock;

static void cleanup(void);
static void yyerror(char *msg);

#if 0 /* no longer necessary? */
#ifndef HAVE_IN6_ADDR_S6_ADDR
# ifdef __FreeBSD__
#  define s6_addr32 __u6_addr.__u6_addr32
#  define s6_addr16 __u6_addr.__u6_addr16
# endif
#endif
#endif

#define ABORT	do { cleanup(); YYABORT; } while (0);
#define ADD_TO_LL(type, list, value) \
	do { \
		if (iface->list == NULL) \
			iface->list = value; \
		else { \
			type *current = iface->list; \
			while (current->next != NULL) \
				current = current->next; \
			current->next = value; \
		} \
	} while (0)




/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 129 "gram.y"
{
	unsigned int		num;
	int			snum;
	double			dec;
	struct in6_addr		*addr;
	char			*str;
	struct AdvPrefix	*pinfo;
	struct AdvRoute		*rinfo;
	struct AdvRDNSS		*rdnssinfo;
	struct AdvDNSSL		*dnsslinfo;
	struct Clients		*ainfo;
}
/* Line 193 of yacc.c.  */
#line 253 "gram.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 266 "gram.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   171

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  85
/* YYNRULES -- Number of states.  */
#define YYNSTATES  187

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   304

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    53,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    52,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,     2,    51,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,    14,    17,    19,    20,    23,
      25,    27,    29,    31,    33,    35,    39,    43,    47,    51,
      55,    59,    63,    67,    71,    75,    79,    83,    87,    91,
      95,    99,   103,   107,   111,   115,   119,   123,   127,   131,
     137,   140,   144,   150,   155,   156,   158,   161,   163,   167,
     171,   175,   179,   183,   187,   193,   198,   199,   201,   204,
     206,   210,   214,   220,   223,   225,   227,   230,   231,   233,
     236,   238,   242,   246,   250,   256,   259,   261,   263,   266,
     267,   269,   272,   274,   278,   280
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    55,    56,    -1,    56,    -1,    57,    50,
      59,    51,    52,    -1,     3,    58,    -1,     9,    -1,    -1,
      60,    59,    -1,    61,    -1,    64,    -1,    62,    -1,    69,
      -1,    74,    -1,    81,    -1,    19,    10,    52,    -1,    18,
      10,    52,    -1,    20,    10,    52,    -1,    19,    12,    52,
      -1,    18,    12,    52,    -1,    20,    12,    52,    -1,    16,
      13,    52,    -1,    17,    13,    52,    -1,    21,    13,    52,
      -1,    22,    13,    52,    -1,    23,    10,    52,    -1,    24,
      10,    52,    -1,    25,    10,    52,    -1,    27,    10,    52,
      -1,    28,    11,    52,    -1,    26,    10,    52,    -1,    29,
      13,    52,    -1,    36,    13,    52,    -1,    37,    13,    52,
      -1,    35,    13,    52,    -1,    40,    10,    52,    -1,    41,
      10,    52,    -1,    39,    13,    52,    -1,    48,    13,    52,
      -1,     8,    50,    63,    51,    52,    -1,    14,    52,    -1,
      63,    14,    52,    -1,    65,    50,    66,    51,    52,    -1,
       4,    14,    53,    10,    -1,    -1,    67,    -1,    67,    68,
      -1,    68,    -1,    30,    13,    52,    -1,    31,    13,    52,
      -1,    34,    13,    52,    -1,    32,    88,    52,    -1,    33,
      88,    52,    -1,    38,    58,    52,    -1,    70,    50,    71,
      51,    52,    -1,     5,    14,    53,    10,    -1,    -1,    72,
      -1,    72,    73,    -1,    73,    -1,    42,    11,    52,    -1,
      43,    88,    52,    -1,    77,    50,    78,    51,    52,    -1,
      75,    76,    -1,    76,    -1,    14,    -1,     6,    75,    -1,
      -1,    79,    -1,    79,    80,    -1,    80,    -1,    44,    10,
      52,    -1,    45,    13,    52,    -1,    46,    88,    52,    -1,
      84,    50,    85,    51,    52,    -1,    82,    83,    -1,    83,
      -1,     9,    -1,     7,    82,    -1,    -1,    86,    -1,    86,
      87,    -1,    87,    -1,    47,    88,    52,    -1,    10,    -1,
      15,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   144,   144,   145,   148,   194,   209,   216,   218,   221,
     222,   223,   224,   225,   226,   229,   233,   237,   241,   245,
     249,   253,   257,   261,   265,   269,   273,   277,   281,   285,
     289,   293,   297,   301,   305,   309,   313,   317,   321,   327,
     333,   344,   359,   390,   453,   454,   457,   458,   461,   465,
     469,   476,   480,   484,   496,   504,   528,   529,   532,   533,
     537,   541,   547,   554,   555,   558,   593,   602,   603,   606,
     607,   611,   615,   619,   632,   639,   640,   643,   685,   694,
     695,   698,   699,   703,   716,   720
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_INTERFACE", "T_PREFIX", "T_ROUTE",
  "T_RDNSS", "T_DNSSL", "T_CLIENTS", "STRING", "NUMBER", "SIGNEDNUMBER",
  "DECIMAL", "SWITCH", "IPV6ADDR", "INFINITY", "T_IgnoreIfMissing",
  "T_AdvSendAdvert", "T_MaxRtrAdvInterval", "T_MinRtrAdvInterval",
  "T_MinDelayBetweenRAs", "T_AdvManagedFlag", "T_AdvOtherConfigFlag",
  "T_AdvLinkMTU", "T_AdvReachableTime", "T_AdvRetransTimer",
  "T_AdvCurHopLimit", "T_AdvDefaultLifetime", "T_AdvDefaultPreference",
  "T_AdvSourceLLAddress", "T_AdvOnLink", "T_AdvAutonomous",
  "T_AdvValidLifetime", "T_AdvPreferredLifetime", "T_AdvRouterAddr",
  "T_AdvHomeAgentFlag", "T_AdvIntervalOpt", "T_AdvHomeAgentInfo",
  "T_Base6to4Interface", "T_UnicastOnly", "T_HomeAgentPreference",
  "T_HomeAgentLifetime", "T_AdvRoutePreference", "T_AdvRouteLifetime",
  "T_AdvRDNSSPreference", "T_AdvRDNSSOpenFlag", "T_AdvRDNSSLifetime",
  "T_AdvDNSSLLifetime", "T_AdvMobRtrSupportFlag", "T_BAD_TOKEN", "'{'",
  "'}'", "';'", "'/'", "$accept", "grammar", "ifacedef", "ifacehead",
  "name", "ifaceparams", "ifaceparam", "ifaceval", "clientslist",
  "v6addrlist", "prefixdef", "prefixhead", "optional_prefixplist",
  "prefixplist", "prefixparms", "routedef", "routehead",
  "optional_routeplist", "routeplist", "routeparms", "rdnssdef",
  "rdnssaddrs", "rdnssaddr", "rdnsshead", "optional_rdnssplist",
  "rdnssplist", "rdnssparms", "dnssldef", "dnsslsuffixes", "dnsslsuffix",
  "dnsslhead", "optional_dnsslplist", "dnsslplist", "dnsslparms",
  "number_or_infinity", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     123,   125,    59,    47
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    55,    56,    57,    58,    59,    59,    60,
      60,    60,    60,    60,    60,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    62,
      63,    63,    64,    65,    66,    66,    67,    67,    68,    68,
      68,    68,    68,    68,    69,    70,    71,    71,    72,    72,
      73,    73,    74,    75,    75,    76,    77,    78,    78,    79,
      79,    80,    80,    80,    81,    82,    82,    83,    84,    85,
      85,    86,    86,    87,    88,    88
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     5,     2,     1,     0,     2,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     5,
       2,     3,     5,     4,     0,     1,     2,     1,     3,     3,
       3,     3,     3,     3,     5,     4,     0,     1,     2,     1,
       3,     3,     5,     2,     1,     1,     2,     0,     1,     2,
       1,     3,     3,     3,     5,     2,     1,     1,     2,     0,
       1,     2,     1,     3,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     3,     0,     6,     5,     1,     2,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     7,     9,    11,
      10,     0,    12,     0,    13,     0,    14,     0,     0,     0,
      65,    66,    64,    77,    78,    76,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     8,    44,    56,    67,    79,     0,     0,    63,
      75,     0,     0,    21,    22,    16,    19,    15,    18,    17,
      20,    23,    24,    25,    26,    27,    30,    28,    29,    31,
      34,    32,    33,    37,    35,    36,    38,     4,     0,     0,
       0,     0,     0,     0,     0,    45,    47,     0,     0,     0,
      57,    59,     0,     0,     0,     0,    68,    70,     0,     0,
      80,    82,    43,    55,    40,     0,     0,     0,     0,    84,
      85,     0,     0,     0,     0,     0,    46,     0,     0,     0,
      58,     0,     0,     0,     0,    69,     0,     0,    81,    41,
      39,    48,    49,    51,    52,    50,    53,    42,    60,    61,
      54,    71,    72,    73,    62,    83,    74
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,     6,    36,    37,    38,    39,    92,
      40,    41,   124,   125,   126,    42,    43,   129,   130,   131,
      44,    51,    52,    45,   135,   136,   137,    46,    54,    55,
      47,   139,   140,   141,   151
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -88
static const yytype_int8 yypact[] =
{
       4,     2,     6,   -88,   -11,   -88,   -88,   -88,   -88,    -4,
      26,    31,    35,    41,     8,    48,    49,    -2,    36,    47,
      50,    51,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    69,    70,    68,    19,    -4,   -88,   -88,
     -88,    23,   -88,    32,   -88,    33,   -88,    34,    37,    38,
     -88,    35,   -88,   -88,    41,   -88,    71,    20,    40,    42,
      43,    44,    45,    46,    52,    53,    54,    66,    67,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,   -88,    22,   -13,   -18,    39,    89,    90,   -88,
     -88,    85,    -9,   -88,   -88,   -88,   -88,   -88,   -88,   -88,
     -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,
     -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,    88,    94,
      28,    28,    95,     2,    87,    22,   -88,    91,    28,    92,
     -13,   -88,    93,    96,    28,    97,   -18,   -88,    28,    98,
      39,   -88,   -88,   -88,   -88,    99,   100,   101,   102,   -88,
     -88,   103,   104,   105,   106,   107,   -88,   108,   109,   110,
     -88,   111,   112,   113,   114,   -88,   115,   116,   -88,   -88,
     -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,
     -88,   -88,   -88,   -88,   -88,   -88,   -88
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -88,   -88,    86,   -88,   -36,   132,   -88,   -88,   -88,   -88,
     -88,   -88,   -88,   -88,   -32,   -88,   -88,   -88,   -88,   -41,
     -88,   -88,   119,   -88,   -88,   -88,   -26,   -88,   -88,   117,
     -88,   -88,   -88,   -29,   -87
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      10,    11,    12,    13,    14,   145,     7,     1,    59,     1,
      60,     5,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,   132,   133,   134,   127,
     128,    29,    30,    31,   152,    32,    33,    34,   149,     9,
      48,   158,   146,   150,    35,    49,    61,   163,    62,    50,
      53,   166,   118,   119,   120,   121,   122,    63,    56,    64,
     123,    57,    58,    65,    66,    67,    68,    69,    70,    71,
      81,    72,    93,    83,    73,    74,    75,    76,    77,    78,
      79,    80,    84,    85,    86,    91,   138,   154,     8,   160,
      87,    88,    94,   156,    95,    96,    97,    98,    99,   142,
     143,   147,   157,   161,   100,   101,   102,   148,   153,   162,
     165,   168,     0,     0,     0,     0,     0,     0,   103,   104,
       0,     0,     0,     0,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   144,   155,     0,
       0,     0,     0,   159,     0,     0,     0,     0,   164,   167,
       0,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,    82,
      89,    90
};

static const yytype_int16 yycheck[] =
{
       4,     5,     6,     7,     8,    14,     0,     3,    10,     3,
      12,     9,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    44,    45,    46,    42,
      43,    35,    36,    37,   121,    39,    40,    41,    10,    50,
      14,   128,    51,    15,    48,    14,    10,   134,    12,    14,
       9,   138,    30,    31,    32,    33,    34,    10,    50,    12,
      38,    13,    13,    13,    13,    10,    10,    10,    10,    10,
      51,    11,    52,    50,    13,    13,    13,    13,    13,    10,
      10,    13,    50,    50,    50,    14,    47,   123,     2,   130,
      53,    53,    52,   125,    52,    52,    52,    52,    52,    10,
      10,    13,    11,    10,    52,    52,    52,    13,    13,    13,
     136,   140,    -1,    -1,    -1,    -1,    -1,    -1,    52,    52,
      -1,    -1,    -1,    -1,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    51,    -1,
      -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,    51,    51,
      -1,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    37,
      51,    54
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    55,    56,    57,     9,    58,     0,    56,    50,
       4,     5,     6,     7,     8,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    35,
      36,    37,    39,    40,    41,    48,    59,    60,    61,    62,
      64,    65,    69,    70,    74,    77,    81,    84,    14,    14,
      14,    75,    76,     9,    82,    83,    50,    13,    13,    10,
      12,    10,    12,    10,    12,    13,    13,    10,    10,    10,
      10,    10,    11,    13,    13,    13,    13,    13,    10,    10,
      13,    51,    59,    50,    50,    50,    50,    53,    53,    76,
      83,    14,    63,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    30,    31,
      32,    33,    34,    38,    66,    67,    68,    42,    43,    71,
      72,    73,    44,    45,    46,    78,    79,    80,    47,    85,
      86,    87,    10,    10,    52,    14,    51,    13,    13,    10,
      15,    88,    88,    13,    58,    51,    68,    11,    88,    51,
      73,    10,    13,    88,    51,    80,    88,    51,    87,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 149 "gram.y"
    {
			struct Interface *iface2;

			iface2 = IfaceList;
			while (iface2)
			{
				if (!strcmp(iface2->Name, iface->Name))
				{
					flog(LOG_ERR, "duplicate interface "
						"definition for %s", iface->Name);
					ABORT;
				}
				iface2 = iface2->next;
			}

			if (check_device(sock, iface) < 0) {
				if (iface->IgnoreIfMissing) {
					dlog(LOG_DEBUG, 4, "interface %s did not exist, ignoring the interface", iface->Name);
				}
				else {
					flog(LOG_ERR, "interface %s does not exist", iface->Name);
					ABORT;
				}
			}
			if (setup_deviceinfo(sock, iface) < 0)
				if (!iface->IgnoreIfMissing)
				ABORT;
			if (check_iface(iface) < 0)
				if (!iface->IgnoreIfMissing)
				ABORT;
			if (setup_linklocal_addr(sock, iface) < 0)
				if (!iface->IgnoreIfMissing)
				ABORT;
			if (setup_allrouters_membership(sock, iface) < 0)
				if (!iface->IgnoreIfMissing)
				ABORT;

			dlog(LOG_DEBUG, 4, "interface definition for %s is ok", iface->Name);

			iface->next = IfaceList;
			IfaceList = iface;

			iface = NULL;
		}
    break;

  case 5:
#line 195 "gram.y"
    {
			iface = malloc(sizeof(struct Interface));

			if (iface == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			iface_init_defaults(iface);
			strncpy(iface->Name, (yyvsp[(2) - (2)].str), IFNAMSIZ-1);
			iface->Name[IFNAMSIZ-1] = '\0';
		}
    break;

  case 6:
#line 210 "gram.y"
    {
			/* check vality */
			(yyval.str) = (yyvsp[(1) - (1)].str);
		}
    break;

  case 10:
#line 222 "gram.y"
    { ADD_TO_LL(struct AdvPrefix, AdvPrefixList, (yyvsp[(1) - (1)].pinfo)); }
    break;

  case 11:
#line 223 "gram.y"
    { ADD_TO_LL(struct Clients, ClientList, (yyvsp[(1) - (1)].ainfo)); }
    break;

  case 12:
#line 224 "gram.y"
    { ADD_TO_LL(struct AdvRoute, AdvRouteList, (yyvsp[(1) - (1)].rinfo)); }
    break;

  case 13:
#line 225 "gram.y"
    { ADD_TO_LL(struct AdvRDNSS, AdvRDNSSList, (yyvsp[(1) - (1)].rdnssinfo)); }
    break;

  case 14:
#line 226 "gram.y"
    { ADD_TO_LL(struct AdvDNSSL, AdvDNSSLList, (yyvsp[(1) - (1)].dnsslinfo)); }
    break;

  case 15:
#line 230 "gram.y"
    {
			iface->MinRtrAdvInterval = (yyvsp[(2) - (3)].num);
		}
    break;

  case 16:
#line 234 "gram.y"
    {
			iface->MaxRtrAdvInterval = (yyvsp[(2) - (3)].num);
		}
    break;

  case 17:
#line 238 "gram.y"
    {
			iface->MinDelayBetweenRAs = (yyvsp[(2) - (3)].num);
		}
    break;

  case 18:
#line 242 "gram.y"
    {
			iface->MinRtrAdvInterval = (yyvsp[(2) - (3)].dec);
		}
    break;

  case 19:
#line 246 "gram.y"
    {
			iface->MaxRtrAdvInterval = (yyvsp[(2) - (3)].dec);
		}
    break;

  case 20:
#line 250 "gram.y"
    {
			iface->MinDelayBetweenRAs = (yyvsp[(2) - (3)].dec);
		}
    break;

  case 21:
#line 254 "gram.y"
    {
			iface->IgnoreIfMissing = (yyvsp[(2) - (3)].num);
		}
    break;

  case 22:
#line 258 "gram.y"
    {
			iface->AdvSendAdvert = (yyvsp[(2) - (3)].num);
		}
    break;

  case 23:
#line 262 "gram.y"
    {
			iface->AdvManagedFlag = (yyvsp[(2) - (3)].num);
		}
    break;

  case 24:
#line 266 "gram.y"
    {
			iface->AdvOtherConfigFlag = (yyvsp[(2) - (3)].num);
		}
    break;

  case 25:
#line 270 "gram.y"
    {
			iface->AdvLinkMTU = (yyvsp[(2) - (3)].num);
		}
    break;

  case 26:
#line 274 "gram.y"
    {
			iface->AdvReachableTime = (yyvsp[(2) - (3)].num);
		}
    break;

  case 27:
#line 278 "gram.y"
    {
			iface->AdvRetransTimer = (yyvsp[(2) - (3)].num);
		}
    break;

  case 28:
#line 282 "gram.y"
    {
			iface->AdvDefaultLifetime = (yyvsp[(2) - (3)].num);
		}
    break;

  case 29:
#line 286 "gram.y"
    {
			iface->AdvDefaultPreference = (yyvsp[(2) - (3)].snum);
		}
    break;

  case 30:
#line 290 "gram.y"
    {
			iface->AdvCurHopLimit = (yyvsp[(2) - (3)].num);
		}
    break;

  case 31:
#line 294 "gram.y"
    {
			iface->AdvSourceLLAddress = (yyvsp[(2) - (3)].num);
		}
    break;

  case 32:
#line 298 "gram.y"
    {
			iface->AdvIntervalOpt = (yyvsp[(2) - (3)].num);
		}
    break;

  case 33:
#line 302 "gram.y"
    {
			iface->AdvHomeAgentInfo = (yyvsp[(2) - (3)].num);
		}
    break;

  case 34:
#line 306 "gram.y"
    {
			iface->AdvHomeAgentFlag = (yyvsp[(2) - (3)].num);
		}
    break;

  case 35:
#line 310 "gram.y"
    {
			iface->HomeAgentPreference = (yyvsp[(2) - (3)].num);
		}
    break;

  case 36:
#line 314 "gram.y"
    {
			iface->HomeAgentLifetime = (yyvsp[(2) - (3)].num);
		}
    break;

  case 37:
#line 318 "gram.y"
    {
			iface->UnicastOnly = (yyvsp[(2) - (3)].num);
		}
    break;

  case 38:
#line 322 "gram.y"
    {
			iface->AdvMobRtrSupportFlag = (yyvsp[(2) - (3)].num);
		}
    break;

  case 39:
#line 328 "gram.y"
    {
			(yyval.ainfo) = (yyvsp[(3) - (5)].ainfo);
		}
    break;

  case 40:
#line 334 "gram.y"
    {
			struct Clients *new = calloc(1, sizeof(struct Clients));
			if (new == NULL) {
				flog(LOG_CRIT, "calloc failed: %s", strerror(errno));
				ABORT;
			}

			memcpy(&(new->Address), (yyvsp[(1) - (2)].addr), sizeof(struct in6_addr));
			(yyval.ainfo) = new;
		}
    break;

  case 41:
#line 345 "gram.y"
    {
			struct Clients *new = calloc(1, sizeof(struct Clients));
			if (new == NULL) {
				flog(LOG_CRIT, "calloc failed: %s", strerror(errno));
				ABORT;
			}

			memcpy(&(new->Address), (yyvsp[(2) - (3)].addr), sizeof(struct in6_addr));
			new->next = (yyvsp[(1) - (3)].ainfo);
			(yyval.ainfo) = new;
		}
    break;

  case 42:
#line 360 "gram.y"
    {
			unsigned int dst;

			if (prefix->AdvPreferredLifetime >
			    prefix->AdvValidLifetime)
			{
				flog(LOG_ERR, "AdvValidLifeTime must be "
					"greater than AdvPreferredLifetime in %s, line %d",
					conf_file, num_lines);
				ABORT;
			}

			if( prefix->if6to4[0] )
			{
				if (get_v4addr(prefix->if6to4, &dst) < 0)
				{
					flog(LOG_ERR, "interface %s has no IPv4 addresses, disabling 6to4 prefix", prefix->if6to4 );
					prefix->enabled = 0;
				} else
				{
					*((uint16_t *)(prefix->Prefix.s6_addr)) = htons(0x2002);
					memcpy( prefix->Prefix.s6_addr + 2, &dst, sizeof( dst ) );
				}
			}

			(yyval.pinfo) = prefix;
			prefix = NULL;
		}
    break;

  case 43:
#line 391 "gram.y"
    {
			struct in6_addr zeroaddr;
			prefix = malloc(sizeof(struct AdvPrefix));

			if (prefix == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			prefix_init_defaults(prefix);

			if ((yyvsp[(4) - (4)].num) > MAX_PrefixLen)
			{
				flog(LOG_ERR, "invalid prefix length in %s, line %d", conf_file, num_lines);
				ABORT;
			}

			prefix->PrefixLen = (yyvsp[(4) - (4)].num);

			memcpy(&prefix->Prefix, (yyvsp[(2) - (4)].addr), sizeof(struct in6_addr));

			memset(&zeroaddr, 0, sizeof(zeroaddr));
			if (!memcmp((yyvsp[(2) - (4)].addr), &zeroaddr, sizeof(struct in6_addr))) {
#ifndef HAVE_IFADDRS_H
				flog(LOG_ERR, "invalid all-zeros prefix in %s, line %d", conf_file, num_lines);
				ABORT;
#else
				dlog(LOG_DEBUG, 5, "all-zeros prefix in %s, line %d, parsing..", conf_file, num_lines);
				struct ifaddrs *ifap, *ifa;
				if (getifaddrs(&ifap) != 0)
					flog(LOG_ERR, "getifaddrs failed: %s", strerror(errno));
			        for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
				        struct sockaddr_in6 *s6;
					char buf[INET6_ADDRSTRLEN];
					if (strncmp(ifa->ifa_name, iface->Name, IFNAMSIZ))
						continue;
                			if (ifa->ifa_addr->sa_family != AF_INET6)
			                        continue;
					s6 = (struct sockaddr_in6 *)(ifa->ifa_addr);
	                		if (IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr))
						continue;
					if (inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s6->sin6_addr), buf, sizeof(buf)) == NULL) {
						flog(LOG_ERR, "%s: inet_ntop failed in %s, line %d!", ifa->ifa_name, conf_file, num_lines);
					}
					else {
						dlog(LOG_DEBUG, 5, "auto-selected prefix %s on interface %s", buf, ifa->ifa_name);
						memcpy(&prefix->Prefix, &s6->sin6_addr, sizeof(struct in6_addr));
						prefix->AdvRouterAddr=1;
						prefix->AutoSelected=1;
					}
				}
				if (!memcmp(&prefix->Prefix, &zeroaddr, sizeof(struct in6_addr))) {
					prefix->enabled = 0;
					flog(LOG_WARNING, "no auto-selected prefix on interface %s, disabling advertisements",  iface->Name);
				}
				freeifaddrs(ifap);
				freeifaddrs(ifa);
#endif /* ifndef HAVE_IFADDRS_H */
			}
		}
    break;

  case 48:
#line 462 "gram.y"
    {
			prefix->AdvOnLinkFlag = (yyvsp[(2) - (3)].num);
		}
    break;

  case 49:
#line 466 "gram.y"
    {
			prefix->AdvAutonomousFlag = (yyvsp[(2) - (3)].num);
		}
    break;

  case 50:
#line 470 "gram.y"
    {
			if (prefix->AutoSelected && (yyvsp[(2) - (3)].num) == 0)
				flog(LOG_WARNING, "prefix automatically selected, AdvRouterAddr always enabled, ignoring config line %d", num_lines);
			else
				prefix->AdvRouterAddr = (yyvsp[(2) - (3)].num);
		}
    break;

  case 51:
#line 477 "gram.y"
    {
			prefix->AdvValidLifetime = (yyvsp[(2) - (3)].num);
		}
    break;

  case 52:
#line 481 "gram.y"
    {
			prefix->AdvPreferredLifetime = (yyvsp[(2) - (3)].num);
		}
    break;

  case 53:
#line 485 "gram.y"
    {
			if (prefix->AutoSelected) {
				flog(LOG_ERR, "automatically selecting the prefix and Base6to4Interface are mutually exclusive");
				ABORT;
			} /* fallthrough */
			dlog(LOG_DEBUG, 4, "using interface %s for 6to4", (yyvsp[(2) - (3)].str));
			strncpy(prefix->if6to4, (yyvsp[(2) - (3)].str), IFNAMSIZ-1);
			prefix->if6to4[IFNAMSIZ-1] = '\0';
		}
    break;

  case 54:
#line 497 "gram.y"
    {
			(yyval.rinfo) = route;
			route = NULL;
		}
    break;

  case 55:
#line 505 "gram.y"
    {
			route = malloc(sizeof(struct AdvRoute));

			if (route == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			route_init_defaults(route, iface);

			if ((yyvsp[(4) - (4)].num) > MAX_PrefixLen)
			{
				flog(LOG_ERR, "invalid route prefix length in %s, line %d", conf_file, num_lines);
				ABORT;
			}

			route->PrefixLen = (yyvsp[(4) - (4)].num);

			memcpy(&route->Prefix, (yyvsp[(2) - (4)].addr), sizeof(struct in6_addr));
		}
    break;

  case 60:
#line 538 "gram.y"
    {
			route->AdvRoutePreference = (yyvsp[(2) - (3)].snum);
		}
    break;

  case 61:
#line 542 "gram.y"
    {
			route->AdvRouteLifetime = (yyvsp[(2) - (3)].num);
		}
    break;

  case 62:
#line 548 "gram.y"
    {
			(yyval.rdnssinfo) = rdnss;
			rdnss = NULL;
		}
    break;

  case 65:
#line 559 "gram.y"
    {
			if (!rdnss) {
				/* first IP found */
				rdnss = malloc(sizeof(struct AdvRDNSS));

				if (rdnss == NULL) {
					flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
					ABORT;
				}

				rdnss_init_defaults(rdnss, iface);
			}

			switch (rdnss->AdvRDNSSNumber) {
				case 0:
					memcpy(&rdnss->AdvRDNSSAddr1, (yyvsp[(1) - (1)].addr), sizeof(struct in6_addr));
					rdnss->AdvRDNSSNumber++;
					break;
				case 1:
					memcpy(&rdnss->AdvRDNSSAddr2, (yyvsp[(1) - (1)].addr), sizeof(struct in6_addr));
					rdnss->AdvRDNSSNumber++;
					break;
				case 2:
					memcpy(&rdnss->AdvRDNSSAddr3, (yyvsp[(1) - (1)].addr), sizeof(struct in6_addr));
					rdnss->AdvRDNSSNumber++;
					break;
				default:
					flog(LOG_CRIT, "Too many addresses in RDNSS section");
					ABORT;
			}

		}
    break;

  case 66:
#line 594 "gram.y"
    {
			if (!rdnss) {
				flog(LOG_CRIT, "No address specified in RDNSS section");
				ABORT;
			}
		}
    break;

  case 71:
#line 612 "gram.y"
    {
			flog(LOG_WARNING, "Ignoring deprecated RDNSS preference.");
		}
    break;

  case 72:
#line 616 "gram.y"
    {
			flog(LOG_WARNING, "Ignoring deprecated RDNSS open flag.");
		}
    break;

  case 73:
#line 620 "gram.y"
    {
			if ((yyvsp[(2) - (3)].num) < iface->MaxRtrAdvInterval && (yyvsp[(2) - (3)].num) != 0) {
				flog(LOG_ERR, "AdvRDNSSLifetime must be at least MaxRtrAdvInterval");
				ABORT;
			}
			if ((yyvsp[(2) - (3)].num) > 2*(iface->MaxRtrAdvInterval))
				flog(LOG_WARNING, "Warning: AdvRDNSSLifetime <= 2*MaxRtrAdvInterval would allow stale DNS servers to be deleted faster");

			rdnss->AdvRDNSSLifetime = (yyvsp[(2) - (3)].num);
		}
    break;

  case 74:
#line 633 "gram.y"
    {
			(yyval.dnsslinfo) = dnssl;
			dnssl = NULL;
		}
    break;

  case 77:
#line 644 "gram.y"
    {
			char *ch;
			for (ch = (yyvsp[(1) - (1)].str);*ch != '\0';ch++) {
				if (*ch >= 'A' && *ch <= 'Z')
					continue;
				if (*ch >= 'a' && *ch <= 'z')
					continue;
				if (*ch >= '0' && *ch <= '9')
					continue;
				if (*ch == '-' || *ch == '.')
					continue;

				flog(LOG_CRIT, "Invalid domain suffix specified");
				ABORT;
			}

			if (!dnssl) {
				/* first domain found */
				dnssl = malloc(sizeof(struct AdvDNSSL));

				if (dnssl == NULL) {
					flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
					ABORT;
				}

				dnssl_init_defaults(dnssl, iface);
			}

			dnssl->AdvDNSSLNumber++;
			dnssl->AdvDNSSLSuffixes =
				realloc(dnssl->AdvDNSSLSuffixes,
					dnssl->AdvDNSSLNumber * sizeof(char*));
			if (dnssl->AdvDNSSLSuffixes == NULL) {
				flog(LOG_CRIT, "realloc failed: %s", strerror(errno));
				ABORT;
			}

			dnssl->AdvDNSSLSuffixes[dnssl->AdvDNSSLNumber - 1] = strdup((yyvsp[(1) - (1)].str));
		}
    break;

  case 78:
#line 686 "gram.y"
    {
			if (!dnssl) {
				flog(LOG_CRIT, "No domain specified in DNSSL section");
				ABORT;
			}
		}
    break;

  case 83:
#line 704 "gram.y"
    {
			if ((yyvsp[(2) - (3)].num) < iface->MaxRtrAdvInterval && (yyvsp[(2) - (3)].num) != 0) {
				flog(LOG_ERR, "AdvDNSSLLifetime must be at least MaxRtrAdvInterval");
				ABORT;
			}
			if ((yyvsp[(2) - (3)].num) > 2*(iface->MaxRtrAdvInterval))
				flog(LOG_WARNING, "Warning: AdvDNSSLLifetime <= 2*MaxRtrAdvInterval would allow stale DNS suffixes to be deleted faster");

			dnssl->AdvDNSSLLifetime = (yyvsp[(2) - (3)].num);
		}
    break;

  case 84:
#line 717 "gram.y"
    {
                                (yyval.num) = (yyvsp[(1) - (1)].num);
                        }
    break;

  case 85:
#line 721 "gram.y"
    {
                                (yyval.num) = (uint32_t)~0;
                        }
    break;


/* Line 1267 of yacc.c.  */
#line 2295 "gram.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 726 "gram.y"


static
void cleanup(void)
{
	if (iface)
		free(iface);

	if (prefix)
		free(prefix);

	if (route)
		free(route);

	if (rdnss)
		free(rdnss);

	if (dnssl) {
		int i;
		for (i = 0;i < dnssl->AdvDNSSLNumber;i++)
			free(dnssl->AdvDNSSLSuffixes[i]);
		free(dnssl->AdvDNSSLSuffixes);
		free(dnssl);
	}
}

static void
yyerror(char *msg)
{
	cleanup();
	flog(LOG_ERR, "%s in %s, line %d: %s", msg, conf_file, num_lines, yytext);
}

