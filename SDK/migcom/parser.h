/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    sySkip = 258,                  /* sySkip  */
    syRoutine = 259,               /* syRoutine  */
    sySimpleRoutine = 260,         /* sySimpleRoutine  */
    sySubsystem = 261,             /* sySubsystem  */
    syKernelUser = 262,            /* syKernelUser  */
    syKernelServer = 263,          /* syKernelServer  */
    syMsgOption = 264,             /* syMsgOption  */
    syMsgSeqno = 265,              /* syMsgSeqno  */
    syWaitTime = 266,              /* syWaitTime  */
    sySendTime = 267,              /* sySendTime  */
    syNoWaitTime = 268,            /* syNoWaitTime  */
    syErrorProc = 269,             /* syErrorProc  */
    syServerPrefix = 270,          /* syServerPrefix  */
    syUserPrefix = 271,            /* syUserPrefix  */
    syServerDemux = 272,           /* syServerDemux  */
    syRCSId = 273,                 /* syRCSId  */
    syImport = 274,                /* syImport  */
    syUImport = 275,               /* syUImport  */
    sySImport = 276,               /* sySImport  */
    syIImport = 277,               /* syIImport  */
    syDImport = 278,               /* syDImport  */
    syIn = 279,                    /* syIn  */
    syOut = 280,                   /* syOut  */
    syInOut = 281,                 /* syInOut  */
    syUserImpl = 282,              /* syUserImpl  */
    syServerImpl = 283,            /* syServerImpl  */
    syRequestPort = 284,           /* syRequestPort  */
    syReplyPort = 285,             /* syReplyPort  */
    sySReplyPort = 286,            /* sySReplyPort  */
    syUReplyPort = 287,            /* syUReplyPort  */
    syType = 288,                  /* syType  */
    syArray = 289,                 /* syArray  */
    syStruct = 290,                /* syStruct  */
    syOf = 291,                    /* syOf  */
    syInTran = 292,                /* syInTran  */
    syOutTran = 293,               /* syOutTran  */
    syDestructor = 294,            /* syDestructor  */
    syCType = 295,                 /* syCType  */
    syCUserType = 296,             /* syCUserType  */
    syUserTypeLimit = 297,         /* syUserTypeLimit  */
    syOnStackLimit = 298,          /* syOnStackLimit  */
    syCServerType = 299,           /* syCServerType  */
    syPointerTo = 300,             /* syPointerTo  */
    syPointerToIfNot = 301,        /* syPointerToIfNot  */
    syValueOf = 302,               /* syValueOf  */
    syCString = 303,               /* syCString  */
    sySecToken = 304,              /* sySecToken  */
    syUserSecToken = 305,          /* syUserSecToken  */
    syServerSecToken = 306,        /* syServerSecToken  */
    syAuditToken = 307,            /* syAuditToken  */
    syUserAuditToken = 308,        /* syUserAuditToken  */
    syServerAuditToken = 309,      /* syServerAuditToken  */
    syColon = 310,                 /* syColon  */
    sySemi = 311,                  /* sySemi  */
    syComma = 312,                 /* syComma  */
    syPlus = 313,                  /* syPlus  */
    syMinus = 314,                 /* syMinus  */
    syStar = 315,                  /* syStar  */
    syDiv = 316,                   /* syDiv  */
    syLParen = 317,                /* syLParen  */
    syRParen = 318,                /* syRParen  */
    syEqual = 319,                 /* syEqual  */
    syCaret = 320,                 /* syCaret  */
    syTilde = 321,                 /* syTilde  */
    syLAngle = 322,                /* syLAngle  */
    syRAngle = 323,                /* syRAngle  */
    syLBrack = 324,                /* syLBrack  */
    syRBrack = 325,                /* syRBrack  */
    syBar = 326,                   /* syBar  */
    syError = 327,                 /* syError  */
    syNumber = 328,                /* syNumber  */
    sySymbolicType = 329,          /* sySymbolicType  */
    syIdentifier = 330,            /* syIdentifier  */
    syString = 331,                /* syString  */
    syQString = 332,               /* syQString  */
    syFileName = 333,              /* syFileName  */
    syIPCFlag = 334                /* syIPCFlag  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define sySkip 258
#define syRoutine 259
#define sySimpleRoutine 260
#define sySubsystem 261
#define syKernelUser 262
#define syKernelServer 263
#define syMsgOption 264
#define syMsgSeqno 265
#define syWaitTime 266
#define sySendTime 267
#define syNoWaitTime 268
#define syErrorProc 269
#define syServerPrefix 270
#define syUserPrefix 271
#define syServerDemux 272
#define syRCSId 273
#define syImport 274
#define syUImport 275
#define sySImport 276
#define syIImport 277
#define syDImport 278
#define syIn 279
#define syOut 280
#define syInOut 281
#define syUserImpl 282
#define syServerImpl 283
#define syRequestPort 284
#define syReplyPort 285
#define sySReplyPort 286
#define syUReplyPort 287
#define syType 288
#define syArray 289
#define syStruct 290
#define syOf 291
#define syInTran 292
#define syOutTran 293
#define syDestructor 294
#define syCType 295
#define syCUserType 296
#define syUserTypeLimit 297
#define syOnStackLimit 298
#define syCServerType 299
#define syPointerTo 300
#define syPointerToIfNot 301
#define syValueOf 302
#define syCString 303
#define sySecToken 304
#define syUserSecToken 305
#define syServerSecToken 306
#define syAuditToken 307
#define syUserAuditToken 308
#define syServerAuditToken 309
#define syColon 310
#define sySemi 311
#define syComma 312
#define syPlus 313
#define syMinus 314
#define syStar 315
#define syDiv 316
#define syLParen 317
#define syRParen 318
#define syEqual 319
#define syCaret 320
#define syTilde 321
#define syLAngle 322
#define syRAngle 323
#define syLBrack 324
#define syRBrack 325
#define syBar 326
#define syError 327
#define syNumber 328
#define sySymbolicType 329
#define syIdentifier 330
#define syString 331
#define syQString 332
#define syFileName 333
#define syIPCFlag 334

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 173 "parser.y"

    u_int number;
    identifier_t identifier;
    string_t string;
    statement_kind_t statement_kind;
    ipc_type_t *type;
    struct
    {
	u_int innumber;		/* msgt_name value, when sending */
	string_t instr;
	u_int outnumber;	/* msgt_name value, when receiving */
	string_t outstr;
	u_int size;		/* 0 means there is no default size */
    } symtype;
    routine_t *routine;
    arg_kind_t direction;
    argument_t *argument;
    ipc_flags_t flag;

#line 245 "parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_H_INCLUDED  */
