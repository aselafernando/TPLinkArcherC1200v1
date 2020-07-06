/* A Bison parser, made from gram.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	T_INTERFACE	257
# define	T_PREFIX	258
# define	T_ROUTE	259
# define	STRING	260
# define	NUMBER	261
# define	SIGNEDNUMBER	262
# define	DECIMAL	263
# define	SWITCH	264
# define	IPV6ADDR	265
# define	INFINITY	266
# define	T_IgnoreIfMissing	267
# define	T_AdvSendAdvert	268
# define	T_MaxRtrAdvInterval	269
# define	T_MinRtrAdvInterval	270
# define	T_MinDelayBetweenRAs	271
# define	T_AdvManagedFlag	272
# define	T_AdvOtherConfigFlag	273
# define	T_AdvLinkMTU	274
# define	T_AdvReachableTime	275
# define	T_AdvRetransTimer	276
# define	T_AdvCurHopLimit	277
# define	T_AdvDefaultLifetime	278
# define	T_AdvDefaultPreference	279
# define	T_AdvSourceLLAddress	280
# define	T_AdvOnLink	281
# define	T_AdvAutonomous	282
# define	T_AdvValidLifetime	283
# define	T_AdvPreferredLifetime	284
# define	T_AdvRouterAddr	285
# define	T_AdvHomeAgentFlag	286
# define	T_AdvIntervalOpt	287
# define	T_AdvHomeAgentInfo	288
# define	T_Base6to4Interface	289
# define	T_UnicastOnly	290
# define	T_HomeAgentPreference	291
# define	T_HomeAgentLifetime	292
# define	T_AdvRoutePreference	293
# define	T_AdvRouteLifetime	294
# define	T_AdvMobRtrSupportFlag	295
# define	T_BAD_TOKEN	296

#line 16 "gram.y"

#include <config.h>
#include <includes.h>
#include <radvd.h>
#include <defaults.h>

extern struct Interface *IfaceList;
struct Interface *iface = NULL;
struct AdvPrefix *prefix = NULL;
struct AdvRoute *route = NULL;

extern char *conf_file;
extern int num_lines;
extern char *yytext;
extern int sock;

static void cleanup(void);
static void yyerror(char *msg);


#define ABORT	do { cleanup(); YYABORT; } while (0);


#line 103 "gram.y"
#ifndef YYSTYPE
typedef union {
	unsigned int		num;
	int			snum;
	double			dec;
	int			bool;
	struct in6_addr		*addr;
	char			*str;
	struct AdvPrefix	*pinfo;
	struct AdvRoute		*rinfo;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		144
#define	YYFLAG		-32768
#define	YYNTBASE	47

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 296 ? yytranslate[x] : 70)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    46,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    45,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    43,     2,    44,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     3,     5,    11,    14,    16,    20,    21,    23,
      24,    26,    27,    29,    32,    34,    38,    42,    46,    50,
      54,    58,    62,    66,    70,    74,    78,    82,    86,    90,
      94,    98,   102,   106,   110,   114,   118,   122,   126,   130,
     132,   135,   141,   146,   147,   149,   152,   154,   158,   162,
     166,   170,   174,   178,   180,   183,   189,   194,   195,   197,
     200,   202,   206,   210,   212
};
static const short yyrhs[] =
{
      47,    48,     0,    48,     0,    49,    43,    51,    44,    45,
       0,     3,    50,     0,     6,     0,    52,    53,    54,     0,
       0,    55,     0,     0,    57,     0,     0,    63,     0,    55,
      56,     0,    56,     0,    16,     7,    45,     0,    15,     7,
      45,     0,    17,     7,    45,     0,    16,     9,    45,     0,
      15,     9,    45,     0,    17,     9,    45,     0,    13,    10,
      45,     0,    14,    10,    45,     0,    18,    10,    45,     0,
      19,    10,    45,     0,    20,     7,    45,     0,    21,     7,
      45,     0,    22,     7,    45,     0,    24,     7,    45,     0,
      25,     8,    45,     0,    23,     7,    45,     0,    26,    10,
      45,     0,    33,    10,    45,     0,    34,    10,    45,     0,
      32,    10,    45,     0,    37,     7,    45,     0,    38,     7,
      45,     0,    36,    10,    45,     0,    41,    10,    45,     0,
      58,     0,    57,    58,     0,    59,    43,    60,    44,    45,
       0,     4,    11,    46,     7,     0,     0,    61,     0,    61,
      62,     0,    62,     0,    27,    10,    45,     0,    28,    10,
      45,     0,    31,    10,    45,     0,    29,    69,    45,     0,
      30,    69,    45,     0,    35,    50,    45,     0,    64,     0,
      63,    64,     0,    65,    43,    66,    44,    45,     0,     5,
      11,    46,     7,     0,     0,    67,     0,    67,    68,     0,
      68,     0,    39,     8,    45,     0,    40,    69,    45,     0,
       7,     0,    12,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   116,   117,   120,   164,   179,   186,   193,   194,   197,
     201,   204,   208,   211,   212,   215,   219,   223,   227,   231,
     235,   239,   243,   247,   251,   255,   259,   263,   267,   271,
     275,   279,   283,   287,   291,   295,   299,   303,   307,   313,
     317,   324,   355,   378,   379,   382,   383,   386,   390,   394,
     398,   402,   406,   414,   418,   425,   433,   457,   458,   461,
     462,   466,   470,   477,   481
};
#endif


#if YYDEBUG || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "T_INTERFACE", "T_PREFIX", "T_ROUTE", 
  "STRING", "NUMBER", "SIGNEDNUMBER", "DECIMAL", "SWITCH", "IPV6ADDR", 
  "INFINITY", "T_IgnoreIfMissing", "T_AdvSendAdvert", 
  "T_MaxRtrAdvInterval", "T_MinRtrAdvInterval", "T_MinDelayBetweenRAs", 
  "T_AdvManagedFlag", "T_AdvOtherConfigFlag", "T_AdvLinkMTU", 
  "T_AdvReachableTime", "T_AdvRetransTimer", "T_AdvCurHopLimit", 
  "T_AdvDefaultLifetime", "T_AdvDefaultPreference", 
  "T_AdvSourceLLAddress", "T_AdvOnLink", "T_AdvAutonomous", 
  "T_AdvValidLifetime", "T_AdvPreferredLifetime", "T_AdvRouterAddr", 
  "T_AdvHomeAgentFlag", "T_AdvIntervalOpt", "T_AdvHomeAgentInfo", 
  "T_Base6to4Interface", "T_UnicastOnly", "T_HomeAgentPreference", 
  "T_HomeAgentLifetime", "T_AdvRoutePreference", "T_AdvRouteLifetime", 
  "T_AdvMobRtrSupportFlag", "T_BAD_TOKEN", "'{'", "'}'", "';'", "'/'", 
  "grammar", "ifacedef", "ifacehead", "name", "ifaceparams", 
  "optional_ifacevlist", "optional_prefixlist", "optional_routelist", 
  "ifacevlist", "ifaceval", "prefixlist", "prefixdef", "prefixhead", 
  "optional_prefixplist", "prefixplist", "prefixparms", "routelist", 
  "routedef", "routehead", "optional_routeplist", "routeplist", 
  "routeparms", "number_or_infinity", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    47,    47,    48,    49,    50,    51,    52,    52,    53,
      53,    54,    54,    55,    55,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    57,
      57,    58,    59,    60,    60,    61,    61,    62,    62,    62,
      62,    62,    62,    63,    63,    64,    65,    66,    66,    67,
      67,    68,    68,    69,    69
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     2,     1,     5,     2,     1,     3,     0,     1,     0,
       1,     0,     1,     2,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       2,     5,     4,     0,     1,     2,     1,     3,     3,     3,
       3,     3,     3,     1,     2,     5,     4,     0,     1,     2,
       1,     3,     3,     1,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     0,     2,     0,     5,     4,     1,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     9,     8,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      11,    10,    39,     0,    13,    21,    22,    16,    19,    15,
      18,    17,    20,    23,    24,    25,    26,    27,    30,    28,
      29,    31,    34,    32,    33,    37,    35,    36,    38,     3,
       0,     0,     6,    12,    53,     0,    40,    43,     0,     0,
      54,    57,     0,     0,     0,     0,     0,     0,     0,    44,
      46,    42,     0,     0,     0,     0,    58,    60,     0,     0,
      63,    64,     0,     0,     0,     0,     0,    45,    56,     0,
       0,     0,    59,    47,    48,    50,    51,    49,    52,    41,
      61,    62,    55,     0,     0
};

static const short yydefgoto[] =
{
       2,     3,     4,     6,    30,    31,    60,    92,    32,    33,
      61,    62,    63,   108,   109,   110,    93,    94,    95,   115,
     116,   117,   122
};

static const short yypact[] =
{
      12,    34,    35,-32768,   -25,-32768,-32768,-32768,   -13,    36,
      37,     7,    27,    32,    38,    39,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    57,    58,    56,
       0,    41,   -13,-32768,    10,    13,    22,    23,    24,    25,
      26,    28,    29,    30,    31,    33,    40,    42,    54,    55,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      72,    41,-32768,    14,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
      69,    70,-32768,    72,-32768,    71,-32768,     2,    73,    74,
  -32768,     3,    76,    78,    15,    15,    79,    34,    75,     2,
  -32768,-32768,    77,    82,    15,    80,     3,-32768,    81,    83,
  -32768,-32768,    84,    85,    86,    87,    88,-32768,-32768,    89,
      90,    91,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,    92,-32768
};

static const short yypgoto[] =
{
  -32768,    93,-32768,   -35,-32768,-32768,-32768,-32768,-32768,    95,
  -32768,    21,-32768,-32768,-32768,   -26,-32768,    -2,-32768,-32768,
  -32768,   -23,   -88
};


#define	YYLAST		136


static const short yytable[] =
{
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    36,     1,    37,   123,     8,    23,
      24,    25,   120,    26,    27,    28,   130,   121,    29,   102,
     103,   104,   105,   106,    38,   143,    39,   107,     1,    40,
       5,    41,   113,   114,    58,    59,    34,    35,    42,    43,
      44,    45,    46,    47,    48,    65,    49,    97,    66,    50,
      51,    52,    53,    54,    55,    56,    57,    67,    68,    69,
      70,    71,   125,    72,    73,    74,    75,    91,    76,    90,
     111,    99,    96,   127,   128,    77,   118,    78,   119,   124,
     129,   100,   144,   132,     0,     7,     0,     0,     0,    79,
      80,     0,     0,     0,    81,    82,    83,    84,    85,    86,
      87,    88,    89,     0,   101,    98,     0,     0,     0,   126,
     112,     0,     0,     0,   131,     0,   133,    64,   134,   135,
     136,   137,   138,   139,   140,   141,   142
};

static const short yycheck[] =
{
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,     7,     3,     9,   105,    43,    32,
      33,    34,     7,    36,    37,    38,   114,    12,    41,    27,
      28,    29,    30,    31,     7,     0,     9,    35,     3,     7,
       6,     9,    39,    40,    44,     4,    10,    10,    10,    10,
       7,     7,     7,     7,     7,    45,     8,    43,    45,    10,
      10,    10,    10,    10,     7,     7,    10,    45,    45,    45,
      45,    45,   107,    45,    45,    45,    45,     5,    45,    11,
       7,    11,    61,   109,     7,    45,    10,    45,    10,    10,
       8,    93,     0,   116,    -1,     2,    -1,    -1,    -1,    45,
      45,    -1,    -1,    -1,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    -1,    43,    46,    -1,    -1,    -1,    44,
      46,    -1,    -1,    -1,    44,    -1,    45,    32,    45,    45,
      45,    45,    45,    45,    45,    45,    45
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if !defined(yyoverflow) || defined(YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined(alloca) || defined(_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined(__STDC__) || defined(__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (!defined(yyoverflow) && (!defined(__cplusplus) || (YYLTYPE_IS_TRIVIAL && \
	YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if !defined(YYSIZE_T) && defined(__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if !defined(YYSIZE_T) && defined(size_t)
# define YYSIZE_T size_t
#endif
#if !defined(YYSIZE_T)
# if defined(__STDC__) || defined(__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if !defined(YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined(__GLIBC__) && defined(_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined(__STDC__) || defined(__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined(__GLIBC__) && defined(_STRING_H) && defined(_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined(__STDC__) || defined(__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined(__STDC__) || defined(__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

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
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 3:
#line 121 "gram.y"
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
					goto skip_interface;
				}
				else {
					flog(LOG_ERR, "interface %s does not exist", iface->Name);
					ABORT;
				}
			}
			if (setup_deviceinfo(sock, iface) < 0)
				ABORT;
			if (check_iface(iface) < 0)
				ABORT;
			if (setup_linklocal_addr(sock, iface) < 0)
				ABORT;
			if (setup_allrouters_membership(sock, iface) < 0)
				ABORT;

			iface->next = IfaceList;
			IfaceList = iface;

			dlog(LOG_DEBUG, 4, "interface definition for %s is ok", iface->Name);

skip_interface:
			iface = NULL;
		}
    break;
case 4:
#line 165 "gram.y"
{
			iface = malloc(sizeof(struct Interface));

			if (iface == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			iface_init_defaults(iface);
			strncpy(iface->Name, yyvsp[0].str, IFNAMSIZ-1);
			iface->Name[IFNAMSIZ-1] = '\0';
		}
    break;
case 5:
#line 180 "gram.y"
{
			/* check vality */
			yyval.str = yyvsp[0].str;
		}
    break;
case 6:
#line 187 "gram.y"
{
			iface->AdvPrefixList = yyvsp[-1].pinfo;
			iface->AdvRouteList = yyvsp[0].rinfo;
		}
    break;
case 9:
#line 198 "gram.y"
{
			yyval.pinfo = NULL;
		}
    break;
case 11:
#line 205 "gram.y"
{
			yyval.rinfo = NULL;
		}
    break;
case 15:
#line 216 "gram.y"
{
			iface->MinRtrAdvInterval = yyvsp[-1].num;
		}
    break;
case 16:
#line 220 "gram.y"
{
			iface->MaxRtrAdvInterval = yyvsp[-1].num;
		}
    break;
case 17:
#line 224 "gram.y"
{
			iface->MinDelayBetweenRAs = yyvsp[-1].num;
		}
    break;
case 18:
#line 228 "gram.y"
{
			iface->MinRtrAdvInterval = yyvsp[-1].dec;
		}
    break;
case 19:
#line 232 "gram.y"
{
			iface->MaxRtrAdvInterval = yyvsp[-1].dec;
		}
    break;
case 20:
#line 236 "gram.y"
{
			iface->MinDelayBetweenRAs = yyvsp[-1].dec;
		}
    break;
case 21:
#line 240 "gram.y"
{
			iface->IgnoreIfMissing = yyvsp[-1].bool;
		}
    break;
case 22:
#line 244 "gram.y"
{
			iface->AdvSendAdvert = yyvsp[-1].bool;
		}
    break;
case 23:
#line 248 "gram.y"
{
			iface->AdvManagedFlag = yyvsp[-1].bool;
		}
    break;
case 24:
#line 252 "gram.y"
{
			iface->AdvOtherConfigFlag = yyvsp[-1].bool;
		}
    break;
case 25:
#line 256 "gram.y"
{
			iface->AdvLinkMTU = yyvsp[-1].num;
		}
    break;
case 26:
#line 260 "gram.y"
{
			iface->AdvReachableTime = yyvsp[-1].num;
		}
    break;
case 27:
#line 264 "gram.y"
{
			iface->AdvRetransTimer = yyvsp[-1].num;
		}
    break;
case 28:
#line 268 "gram.y"
{
			iface->AdvDefaultLifetime = yyvsp[-1].num;
		}
    break;
case 29:
#line 272 "gram.y"
{
			iface->AdvDefaultPreference = yyvsp[-1].snum;
		}
    break;
case 30:
#line 276 "gram.y"
{
			iface->AdvCurHopLimit = yyvsp[-1].num;
		}
    break;
case 31:
#line 280 "gram.y"
{
			iface->AdvSourceLLAddress = yyvsp[-1].bool;
		}
    break;
case 32:
#line 284 "gram.y"
{
			iface->AdvIntervalOpt = yyvsp[-1].bool;
		}
    break;
case 33:
#line 288 "gram.y"
{
			iface->AdvHomeAgentInfo = yyvsp[-1].bool;
		}
    break;
case 34:
#line 292 "gram.y"
{
			iface->AdvHomeAgentFlag = yyvsp[-1].bool;
		}
    break;
case 35:
#line 296 "gram.y"
{
			iface->HomeAgentPreference = yyvsp[-1].num;
		}
    break;
case 36:
#line 300 "gram.y"
{
			iface->HomeAgentLifetime = yyvsp[-1].num;
		}
    break;
case 37:
#line 304 "gram.y"
{
			iface->UnicastOnly = yyvsp[-1].bool;
		}
    break;
case 38:
#line 308 "gram.y"
{
			iface->AdvMobRtrSupportFlag = yyvsp[-1].bool;
		}
    break;
case 39:
#line 314 "gram.y"
{
			yyval.pinfo = yyvsp[0].pinfo;
		}
    break;
case 40:
#line 318 "gram.y"
{
			yyvsp[0].pinfo->next = yyvsp[-1].pinfo;
			yyval.pinfo = yyvsp[0].pinfo;
		}
    break;
case 41:
#line 325 "gram.y"
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

			yyval.pinfo = prefix;
			prefix = NULL;
		}
    break;
case 42:
#line 356 "gram.y"
{
			prefix = malloc(sizeof(struct AdvPrefix));
			
			if (prefix == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			prefix_init_defaults(prefix);

			if (yyvsp[0].num > MAX_PrefixLen)
			{
				flog(LOG_ERR, "invalid prefix length in %s, line %d", conf_file, num_lines);
				ABORT;
			}

			prefix->PrefixLen = yyvsp[0].num;

			memcpy(&prefix->Prefix, yyvsp[-2].addr, sizeof(struct in6_addr));
		}
    break;
case 47:
#line 387 "gram.y"
{
			prefix->AdvOnLinkFlag = yyvsp[-1].bool;
		}
    break;
case 48:
#line 391 "gram.y"
{
			prefix->AdvAutonomousFlag = yyvsp[-1].bool;
		}
    break;
case 49:
#line 395 "gram.y"
{
			prefix->AdvRouterAddr = yyvsp[-1].bool;
		}
    break;
case 50:
#line 399 "gram.y"
{
			prefix->AdvValidLifetime = yyvsp[-1].num;
		}
    break;
case 51:
#line 403 "gram.y"
{
			prefix->AdvPreferredLifetime = yyvsp[-1].num;
		}
    break;
case 52:
#line 407 "gram.y"
{
			dlog(LOG_DEBUG, 4, "using interface %s for 6to4", yyvsp[-1].str);
			strncpy(prefix->if6to4, yyvsp[-1].str, IFNAMSIZ-1);
			prefix->if6to4[IFNAMSIZ-1] = '\0';
		}
    break;
case 53:
#line 415 "gram.y"
{
			yyval.rinfo = yyvsp[0].rinfo;
		}
    break;
case 54:
#line 419 "gram.y"
{
			yyvsp[0].rinfo->next = yyvsp[-1].rinfo;
			yyval.rinfo = yyvsp[0].rinfo;
		}
    break;
case 55:
#line 426 "gram.y"
{
			yyval.rinfo = route;
			route = NULL;
		}
    break;
case 56:
#line 434 "gram.y"
{
			route = malloc(sizeof(struct AdvRoute));
			
			if (route == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			route_init_defaults(route, iface);

			if (yyvsp[0].num > MAX_PrefixLen)
			{
				flog(LOG_ERR, "invalid route prefix length in %s, line %d", conf_file, num_lines);
				ABORT;
			}

			route->PrefixLen = yyvsp[0].num;

			memcpy(&route->Prefix, yyvsp[-2].addr, sizeof(struct in6_addr));
		}
    break;
case 61:
#line 467 "gram.y"
{
			route->AdvRoutePreference = yyvsp[-1].snum;
		}
    break;
case 62:
#line 471 "gram.y"
{
			route->AdvRouteLifetime = yyvsp[-1].num;
		}
    break;
case 63:
#line 478 "gram.y"
{
                                yyval.num = yyvsp[0].num; 
                        }
    break;
case 64:
#line 482 "gram.y"
{
                                yyval.num = (uint32_t)~0;
                        }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 487 "gram.y"


static
void cleanup(void)
{
	if (iface)
		free(iface);
	
	if (prefix)
		free(prefix);
}

static void
yyerror(char *msg)
{
	cleanup();
	flog(LOG_ERR, "%s in %s, line %d: %s", msg, conf_file, num_lines, yytext);
}
