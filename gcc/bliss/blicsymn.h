/* == template for autogen, suffix of output file is .h */


/* autogen if suffix is .h */


/*
  Note: The original of this file is cobcsymn.tpl combined with
  cobctok.def. Do not bother editing cobcsymn.h as it is a generated file.
  
  COBOL Compiler parser (cobcsymn.tpl) keyword numbers enumeration
  
  Copyright (C) 1999, 2000 Tim Josling tej@melbc.org.au
  
  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any
  later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
  
  In other words, you are welcome to use, share and improve this program.
  You are forbidden to forbid anyone else to use, share and improve
  what you give them.   Help stamp out software-hoarding!  
*/  


enum token_enum 
{ 
 
MAGIC_NAME_THAT_CAN_HAVE_ARRAY = +257 ,
 
MAGIC_FUNCTION_THAT_CAN_HAVE_ARGUMENTS = +258 ,
 
MAGIC_LEFT_PARENTHESIS_STARTS_REFMOD = +259 ,
 
MAGIC_CONDITION_NAME = +260 ,
 
MAGIC_SWITCH_NAME = +261 ,
 
MAGIC_SYMBOLIC_CHARACTER_NAME = +262 ,
 
MAGIC_IS_CLASS_CONDITION_FOLLOWS = +263 ,
 
MAGIC_NOT_CLASS_CONDITION_FOLLOWS = +264 ,
 
MAGIC_IS_SIGN_CONDITION_FOLLOWS = +265 ,
 
MAGIC_NOT_SIGN_CONDITION_FOLLOWS = +266 ,
 
MAGIC_GENERIC_NAME_HAS_FOR_FOLLOWING_IN_INSPECT = +267 ,
 
MAGIC_PERFORM_THAT_REQUIRES_END_VERB = +268 ,
 
MAGIC_START_OF_STATEMENT = +269 ,
 
GENERIC_NAME = +270 ,
 
GENERIC_NUMBER_NON_INTEGER = +271 ,
 
GENERIC_NUMBER_INTEGER = +272 ,
 
GENERIC_PICTURE_VALUE = +273 ,
 
GENERIC_STRING = +274 ,
 
FUNCTION_NAME = +416 ,
 
PREC_LESS_PARENTHESIS = +275 ,
 
PREC_LESS_END_VERB = +276 ,
 
PREC_LESS_EXCEPTION = +277 ,
 
PREC_LESS_ELSE = +278 ,
 
PREC_LESS_END_IF = +279 ,
 
ELSE = +280 ,
 
END_IF = +281 ,
 
END_ADD = +282 ,
 
END_COMPUTE = +283 ,
 
END_DIVIDE = +284 ,
 
END_MULTIPLY = +285 ,
 
END_STRING = +286 ,
 
END_SUBTRACT = +287 ,
 
END_UNSTRING = +288 ,
 
MAGIC_ON_HAS_SIZE = +289 ,
 
MAGIC_NOT_HAS_SIZE = +290 ,
 
MAGIC_ON_HAS_OVERFLOW = +291 ,
 
MAGIC_NOT_HAS_OVERFLOW = +292 ,
 
SIZE = +293 ,
 
OVERFLOW = +294 ,
 
LEFT_PARENTHESIS_sym = +295 ,
 
PLUS_sym = +296 ,
 
MINUS_sym = +297 ,
 
ASTERISK_sym = +298 ,
 
SLASH_sym = +299 ,
 
ASTERISK_ASTERISK_sym = +300 ,
 
LESS_THAN_sym = +301 ,
 
GREATER_THAN_sym = +302 ,
 
EQUALS_sym = +303 ,
 
GREATER_EQUALS_sym = +304 ,
 
LESS_EQUALS_sym = +305 ,
 
RIGHT_PARENTHESIS_sym = +306 ,
 
COLON_sym = +307 ,
 
FULLSTOP_sym = +308 ,
 
OR = +309 ,
 
AND_kw = +310 ,
 
NOT_kw = +311 ,
 
ACCEPT = +312 ,
 
ACCESS = +313 ,
 
ADD = +314 ,
 
ADDRESS_kw = +315 ,
 
ADVANCING = +316 ,
 
AFTER = +317 ,
 
ALL = +318 ,
 
ALPHABET = +319 ,
 
ALPHABETIC = +320 ,
 
ALPHABETIC_LOWER = +321 ,
 
ALPHABETIC_UPPER = +322 ,
 
ALPHANUMERIC = +323 ,
 
ALPHANUMERIC_EDITED = +324 ,
 
ALSO = +325 ,
 
ALTER = +326 ,
 
ALTERNATE = +327 ,
 
AND_sym = +676 ,
 
ANY = +328 ,
 
ARE = +329 ,
 
AREA = +330 ,
 
AREAS = +331 ,
 
AS = +332 ,
 
ASCENDING = +333 ,
 
ASSIGN = +334 ,
 
AT = +335 ,
 
AUTHOR = +336 ,
 
BEFORE = +337 ,
 
BINARY = +338 ,
 
BINARY_CHAR = +339 ,
 
BINARY_SHORT = +340 ,
 
BINARY_LONG = +341 ,
 
BINARY_DOUBLE = +342 ,
 
BINARY_LONG_DOUBLE = +677 ,
 
BLANK = +343 ,
 
BLOCK_kw = +344 ,
 
BOTTOM = +345 ,
 
BY = +346 ,
 
CALL_kw = +347 ,
 
CANCEL = +348 ,
 
CD = +349 ,
 
CF = +350 ,
 
CH = +351 ,
 
CHARACTER = +352 ,
 
CHARACTERS = +353 ,
 
CLASS = +354 ,
 
CLOCK_UNITS = +355 ,
 
CLOSE = +356 ,
 
COBOL = +357 ,
 
CODE = +358 ,
 
CODE_SET = +359 ,
 
COLLATING = +360 ,
 
COLUMN = +361 ,
 
COMMA = +362 ,
 
COMMON = +363 ,
 
COMMUNICATION = +364 ,
 
COMP = +365 ,
 
COMPUTATIONAL = +366 ,
 
COMPUTE = +367 ,
 
CONFIGURATION = +368 ,
 
CONTAINS = +369 ,
 
CONTENT = +370 ,
 
CONTINUE = +371 ,
 
CONTROL = +372 ,
 
CONTROLS = +373 ,
 
CONVERTING = +374 ,
 
COPY = +375 ,
 
CORR = +376 ,
 
CORRESPONDING = +377 ,
 
COUNT = +378 ,
 
CURRENCY = +379 ,
 
DATA = +380 ,
 
DATE = +381 ,
 
DATE_COMPILED = +382 ,
 
DATE_WRITTEN = +383 ,
 
DAY = +384 ,
 
DAY_OF_WEEK = +385 ,
 
DE = +386 ,
 
DEBUG_CONTENTS = +387 ,
 
DEBUG_ITEM = +388 ,
 
DEBUG_LINE = +389 ,
 
DEBUG_NAME = +390 ,
 
DEBUG_SUB_1 = +391 ,
 
DEBUG_SUB_2 = +392 ,
 
DEBUG_SUB_3 = +393 ,
 
DEBUGGING = +394 ,
 
DECIMAL_POINT = +395 ,
 
DECLARATIVES = +396 ,
 
DELETE = +397 ,
 
DELIMITED = +398 ,
 
DELIMITER = +399 ,
 
DEPENDING = +400 ,
 
DESCENDING = +401 ,
 
DESTINATION = +402 ,
 
DETAIL = +403 ,
 
DISABLE = +404 ,
 
DISPLAY = +405 ,
 
DIVIDE = +406 ,
 
DIVISION = +407 ,
 
DOWN = +408 ,
 
DUPLICATES = +409 ,
 
DYNAMIC = +410 ,
 
EGI = +411 ,
 
EMI = +413 ,
 
ENABLE = +414 ,
 
END = +415 ,
 
END_CALL = +418 ,
 
END_DELETE = +419 ,
 
END_EVALUATE = +420 ,
 
END_OF_PAGE = +424 ,
 
END_PERFORM = +425 ,
 
END_READ = +426 ,
 
END_RECEIVE = +427 ,
 
END_RETURN = +428 ,
 
END_REWRITE = +429 ,
 
END_SEARCH = +430 ,
 
END_START = +431 ,
 
END_WRITE = +435 ,
 
ENTER = +436 ,
 
ENTRY = +437 ,
 
ENVIRONMENT = +438 ,
 
EOP = +439 ,
 
EQUAL = +440 ,
 
ERROR = +421 ,
 
ESI = +441 ,
 
EVALUATE = +442 ,
 
EVERY = +443 ,
 
EXCEPTION = +444 ,
 
EXIT = +445 ,
 
EXTEND = +446 ,
 
EXTERNAL = +447 ,
 
FALSE_kw = +448 ,
 
FD = +449 ,
 
FILE_word = +450 ,
 
FILE_CONTROL = +451 ,
 
FILLER = +452 ,
 
FINAL = +453 ,
 
FIRST = +454 ,
 
FOOTING = +455 ,
 
FOR = +456 ,
 
FROM = +457 ,
 
FUNCTION = +458 ,
 
FUNCTION_ID = +459 ,
 
GENERATE = +460 ,
 
GIVING = +461 ,
 
GLOBAL = +462 ,
 
GO = +463 ,
 
GOBACK = +464 ,
 
GREATER = +465 ,
 
GROUP = +466 ,
 
HEADING = +467 ,
 
HIGH_VALUE = +468 ,
 
HIGH_VALUES = +469 ,
 
I_O = +470 ,
 
I_O_CONTROL = +471 ,
 
IDENTIFICATION = +472 ,
 
IF = +473 ,
 
IN = +474 ,
 
INDEX = +475 ,
 
INDEXED = +476 ,
 
INDICATE = +477 ,
 
INITIAL_word = +478 ,
 
INITIALIZE = +479 ,
 
INITIATE = +480 ,
 
INPUT = +481 ,
 
INPUT_OUTPUT = +482 ,
 
INSPECT = +483 ,
 
INSTALLATION = +484 ,
 
INTO = +485 ,
 
INVALID = +486 ,
 
IS = +487 ,
 
JUST = +488 ,
 
JUSTIFIED = +489 ,
 
KEY = +490 ,
 
LABEL = +491 ,
 
LAST = +492 ,
 
LEADING = +493 ,
 
LEFT = +494 ,
 
LENGTH = +495 ,
 
LESS = +496 ,
 
LIMIT = +497 ,
 
LIMITS = +498 ,
 
LINAGE = +499 ,
 
LINAGE_COUNTER = +500 ,
 
LINE = +501 ,
 
LINE_COUNTER = +502 ,
 
LINES = +503 ,
 
LINKAGE = +504 ,
 
LOCK = +505 ,
 
LOCAL_STORAGE = +506 ,
 
LOW_VALUE = +507 ,
 
LOW_VALUES = +508 ,
 
MEMORY = +509 ,
 
MERGE = +510 ,
 
MESSAGE = +511 ,
 
MODE = +512 ,
 
MODULES = +513 ,
 
MOVE = +514 ,
 
MULTIPLE = +515 ,
 
MULTIPLY = +516 ,
 
NATIVE = +517 ,
 
NEGATIVE = +518 ,
 
NEXT = +519 ,
 
NO = +520 ,
 
NULL_word = +521 ,
 
NULLS = +522 ,
 
NUMBER = +523 ,
 
NUMERIC = +524 ,
 
NUMERIC_EDITED = +525 ,
 
OBJECT_COMPUTER = +526 ,
 
OCCURS = +527 ,
 
OF = +528 ,
 
OFF = +529 ,
 
OMITTED = +530 ,
 
ON = +531 ,
 
OPEN = +532 ,
 
OPTIONAL = +533 ,
 
ORDER = +534 ,
 
ORGANIZATION = +535 ,
 
OTHER = +536 ,
 
OUTPUT = +537 ,
 
PACKED_DECIMAL = +539 ,
 
PADDING = +540 ,
 
PAGE = +541 ,
 
PAGE_COUNTER = +542 ,
 
PERFORM = +543 ,
 
PF = +544 ,
 
PH = +545 ,
 
PIC = +546 ,
 
PICTURE = +547 ,
 
PLUS_kw = +548 ,
 
POINTER = +549 ,
 
POSITION = +550 ,
 
POSITIVE = +551 ,
 
PRINTING = +552 ,
 
PROCEDURE = +553 ,
 
PROCEDURES = +554 ,
 
PROCEED = +555 ,
 
PROGRAM = +556 ,
 
PROGRAM_ID = +557 ,
 
PROGRAM_POINTER = +558 ,
 
PROTOTYPE = +559 ,
 
PURGE = +560 ,
 
QUEUE = +561 ,
 
QUOTE = +562 ,
 
QUOTES = +563 ,
 
RANDOM = +564 ,
 
RD = +565 ,
 
READ = +566 ,
 
RECEIVE = +567 ,
 
RECORD = +568 ,
 
RECORDS = +569 ,
 
REDEFINES = +570 ,
 
REEL = +571 ,
 
REFERENCE = +572 ,
 
REFERENCES = +573 ,
 
RELATIVE = +574 ,
 
RELEASE = +575 ,
 
REMAINDER = +576 ,
 
REMOVAL = +577 ,
 
RENAMES = +578 ,
 
REPLACE = +579 ,
 
REPLACING = +580 ,
 
REPORT = +581 ,
 
REPORTING = +582 ,
 
REPORTS = +583 ,
 
REPOSITORY = +584 ,
 
RERUN = +585 ,
 
RESERVE = +586 ,
 
RESET = +587 ,
 
RETURN_kw = +588 ,
 
RETURNING = +589 ,
 
REVERSED = +590 ,
 
REWIND = +591 ,
 
REWRITE = +592 ,
 
RF = +593 ,
 
RH = +594 ,
 
RIGHT = +595 ,
 
ROUNDED = +596 ,
 
RUN = +597 ,
 
SAME = +598 ,
 
SD = +599 ,
 
SEARCH = +600 ,
 
SECTION = +601 ,
 
SECURITY = +602 ,
 
SEGMENT = +603 ,
 
SEGMENT_LIMIT = +604 ,
 
SELECT = +605 ,
 
SEND = +606 ,
 
SENTENCE = +607 ,
 
SEPARATE = +608 ,
 
SEQUENCE_kw = +609 ,
 
SEQUENTIAL = +610 ,
 
SET_kw = +611 ,
 
SIGN = +612 ,
 
SIGNED = +613 ,
 
SORT = +615 ,
 
SORT_MERGE = +616 ,
 
SOURCE = +617 ,
 
SOURCE_COMPUTER = +618 ,
 
SPACE = +619 ,
 
SPACES = +620 ,
 
SPECIAL_NAMES = +621 ,
 
STANDARD = +622 ,
 
STANDARD_1 = +623 ,
 
STANDARD_2 = +624 ,
 
START = +625 ,
 
STATUS = +626 ,
 
STOP = +627 ,
 
STRING = +628 ,
 
SUB_QUEUE_1 = +629 ,
 
SUB_QUEUE_2 = +630 ,
 
SUB_QUEUE_3 = +631 ,
 
SUBTRACT = +632 ,
 
SUM = +633 ,
 
SUPPRESS = +634 ,
 
SYMBOLIC = +635 ,
 
SYNC = +636 ,
 
SYNCHRONIZED = +637 ,
 
TABLE = +638 ,
 
TALLYING = +639 ,
 
TAPE = +640 ,
 
TERMINAL = +641 ,
 
TERMINATE = +642 ,
 
TEST = +643 ,
 
TEXT = +644 ,
 
THAN = +645 ,
 
THEN = +646 ,
 
THROUGH = +647 ,
 
THRU = +648 ,
 
TIME = +649 ,
 
TIMES = +650 ,
 
TO = +651 ,
 
TOP = +652 ,
 
TRAILING = +653 ,
 
TRUE_kw = +654 ,
 
TYPE = +655 ,
 
UNIT = +656 ,
 
UNSIGNED = +657 ,
 
UNSTRING = +658 ,
 
UNTIL = +659 ,
 
UP = +660 ,
 
UPON = +661 ,
 
USAGE = +662 ,
 
USE_kw = +663 ,
 
USING = +664 ,
 
VALUE_word = +665 ,
 
VALUES = +666 ,
 
VARYING = +667 ,
 
WHEN = +668 ,
 
WITH = +669 ,
 
WORDS = +670 ,
 
WORKING_STORAGE = +671 ,
 
WRITE = +672 ,
 
ZERO = +673 ,
 
ZEROES = +674 ,
 
ZEROS = +675 ,

};

