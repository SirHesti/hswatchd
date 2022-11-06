/*___________________________________________________________________________

 Tools.c Projektübergreifende Funktionen

___[ Revision ]______________________________________________________________

 02.04.2006 HS RE_Create
_____________________________________________________________________________

 Für Doxygen. Ein Verzeichnis in dieser Reihenfolge
_____________________________________________________________________________*/

/** @addtogroup c_tools_globalvars     Globale Variablen*//** @{ @} */
/** @addtogroup c_init_tools           Aufruf der InitTools*//** @{ @} */
/** @addtogroup c_args                 Argumente an Main()*//** @{ @} */
/** @addtogroup c_lprintf              Lognachrichten lprintf / lmsg*//** @{ @} */

/** @addtogroup c_strchr               Zeichenketten & Chars*//** @{ @} */
/** @addtogroup c_strchr_macros        Lokale Macrodefinitionen*//** @{ @} */
/** @addtogroup c_strchr_manipulations Zeichenkettenmanipulation *//** @{ @} */
/** @addtogroup c_strchr_numbers       Zeichenketten und Zahlen*//** @{ @} */
/** @addtogroup c_strchr_escape        String (de-)escapen*//** @{ @} */
/** @addtogroup c_strchr_strexpr       Strings parsen*//** @{ @} */
/** @addtogroup c_strchr_strstr        Dynamische Strings / strprintf*//** @{ @} */

/** @addtogroup c_tempfile             Temp & Random*//** @{ @} */
/** @addtogroup c_malloc               Speicherhelfer*//** @{ @} */
/** @addtogroup c_timerfunc            Zeitfunktionen*//** @{ @} */
/** @addtogroup c_dumpfunc             Dumpfunktionen*//** @{ @} */
/** @addtogroup c_iofunc               I/O Funktionen*//** @{ @} */
/** @addtogroup c_links                Links auswerten*//** @{ @} */
/** @addtogroup c_chainlist            Einfach verkettete Liste verwalten*//** @{ @} */
/** @addtogroup c_uname  		       struct utsname *//** @{ @} */

/** @addtogroup c_crc32                CRC32 erstellen*//** @{ @} */
/** @addtogroup c_multi_args           Multiple Argumente*//** @{ @} */
/** @addtogroup c_rgbcolor             RGBColor Farben fuer Schriften*//** @{ @} */
/** @addtogroup c_vars                 Variablenverwaltung*//** @{ @} */
/** @addtogroup c_cset                 Characterset wandeln*//** @{ @} */
/** @addtogroup c_json                 JSon Dateiverarbeitung*//** @{ @} */

/**
 @addtogroup c_tools_globalvars
 @{
*/

#ifndef __TOOLSC__
#define __TOOLSC__

/// Betriebssystem für das compiliert wird
//______________________________________________________________________________

#ifdef _WIN32
#define OS_WINS
#endif // _WIN32

#ifdef _WIN32
#define OS_WINDOWS
#define OS_VERSION "Windows"
#elif __linux__
#define OS_LINUX
#define OS_VERSION "Linux"
#else
#error "Unknown OS used"
#endif

/// Compiler der verwendet wird
// ** __COMPILER_VERSION__ mmssbb mm Major ss Subversion bb Build
// ** __COMPILER__         ASC welcher Compiler
//______________________________________________________________________________

#ifdef _MSC_VER
#define __COMPILER_VERSION__ (_MSC_VER * 100 + _MSC_BUILD)
#define __COMPILER__ "MSC"
#endif

#ifdef __TINYC__
#define __COMPILER_VERSION__ __TINYC__
#if __TINYC__ < 10000
#undef __COMPILER_VERSION__
#define __COMPILER_VERSION__ ( __TINYC__ * 100 )
#endif
#define __COMPILER__ "TinyC"
#endif // __TINYC__


#ifdef __GNUC__
#define __COMPILER_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define __COMPILER__ "GnuCC"

#if __GNUC__ >= 10
#define GNU10
#endif

#endif // __GNUC__

#include <limits.h>
#include <stdio.h>
// nur stdlib.h führt dazu das auch aus include/c++/stdlib.h included wird im Präerkennungsmode für Codeblocks durchlaufen wird
#ifndef _MSC_VER
#include <../include/stdlib.h>
#else
#include <stdlib.h>
#endif

#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <stdarg.h>
//#define _LARGE_TIME_API funktioniert nicht
#include <time.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <assert.h>

//#ifdef __TINYC__
#include <stdbool.h>
//#endif // __TINYC__

// in linux anders ... HS 27.11.17 // in linux unsigned long long und windows jetzt auch
// #define ltime_t __uint64

#ifndef __int64
#define __int64 long long
#endif

#ifndef __uint64
#define __uint64 unsigned long long
#endif

#ifndef __int32
#define __int32 long
#endif

//typedef          long long  int64;
//typedef unsigned long long uint64;
// removed nicht benutzt und geht auch nicht unter c++ .... 08.07.20 HS
//#ifndef __STDC_FORMAT_MACROS
//#define __STDC_FORMAT_MACROS
//#endif // __STDC_FORMAT_MACROS
//#include <inttypes.h>


#ifdef OS_LINUX
#include <dirent.h>
#include <sys/types.h>
#include <sys/statfs.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <unistd.h>
#include <utime.h>
#include <syslog.h>

#define DIR_SEP "/"
#define cDIR_SEP '/'
#define OTHER_OS_cDIR_SEP '\\'
#define OS_CRLF "\n"

#ifndef _USE_64BIT_TIME_T
#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif
#endif

#endif // OS_LINUX

#ifdef OS_WINDOWS
// Solange ich nix besseres weiss ist das minimum windows 7
#ifndef WINVER
    #define WINVER 0x0601
#endif
#include <conio.h>
#include <sys/timeb.h>
#include <sys/utime.h>

#ifdef _MSC_VER
#include "dirent_msc.h"
#else
#include <dirent.h>
#include <dir.h>
#endif // _MSC_VER


//#include <dos.h> // removed 05.07.20 HS
#include <process.h>
#include <windows.h>

#define DIR_SEP "\\"
#define cDIR_SEP '\\'
#define OTHER_OS_cDIR_SEP '/'
#define OS_CRLF "\r\n"

#endif // OS_WINDOWS

#ifdef HS_DEBUG
#define HS_DEBUG_TRUE 1
#else
#define HS_DEBUG_TRUE 0
#endif // HS_DEBUG

#define EXEC_SUCCEEDED(Status) ((int)(Status) == EXIT_SUCCESS )
#define EXEC_FAILED(Status)    ((int)(Status) != EXIT_SUCCESS )

//  _WIN64 nur windows 64bit
// _M_X64 Dieser Wert wird für Kompilierungen, die x64 - Prozessoren als Ziel verwenden, als literale ganze Zahl 100 definiert.Andernfalls wird der Wert nicht definiert.
#if (!defined IS64BIT) && ( (defined _M_X64) || (defined _WIN64) )
#define IS64BIT
#endif

#if (!defined IS64BIT) && (!defined IS32BIT)
#define IS32BIT
#endif

#ifdef  __cplusplus
extern "C" {
#endif

//==============================================================================
// normal wird true/false in stdbool.h definiert
// true and false define !!
// 21.12.2016 HS Rewritten True / False

#ifndef __cplusplus
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#ifndef bool
#error bool must be define
#endif
#endif // __cplusplus

#ifndef boolean
#define boolean bool
#endif // boolean

#ifndef False
#define False false
#endif // False
#ifndef True
#define True true
#endif // True

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

// end of true/false
//==============================================================================

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#ifdef STRING_MAX
#error "STRING_MAX definied"
#else
#define STRING_MAX 2048
#endif

#ifndef daylight
#define daylight _daylight
#endif // daylight


/*SPECIAL*/
#ifndef timezone
//#ifdef _MSC_VER
//#define timezone _timezone
//struct timezone
//{
//    int  tz_minuteswest; /* minutes W of Greenwich */
//    int  tz_dsttime;     /* type of dst correction */
//};
//#endif
#endif


// in linux gibt es das schon HS 27.11.17
#ifdef OS_LINUX
#define _TIMEZONE_DEFINED
#endif

#ifndef _TIMEZONE_DEFINED
struct timezone
{
    int tz_minuteswest;
    int tz_dsttime;
};
#endif

#ifndef _timezone
#define _timezone timezone
#endif




// ********************************************************************************************************************

// 23.01.19 HS GNUC kann den üblichen %lld bis in die 8.er Version, die in C99 definiert ist leider nicht interpretieren ....
// 03.08.20 HS nur unter TINYC nicht
// 23.07.22 HS GNU10
#ifdef OS_WINDOWS
#ifndef __TINYC__

#ifdef GNU10
#define LLD_STR "%I64lld"
#define LLU_STR "%I64llu"
#define LLX_STR "%I64llx"
#else
#define LLD_STR "%I64d"
#define LLU_STR "%I64u"
#define LLX_STR "%I64x"
#endif

#endif
#endif

#ifdef __MINGW32_VERSION
#endif // __MINGW32_VERSION

// GNUC + Windows alle anderen können das hier was in C99 definiert ist
// 23.01.19 HS
#ifndef LLD_STR
  #define LLD_STR "%lld"
  #define LLU_STR "%llu"
  #define LLX_STR "%llx"
#endif

// ********************************************************************************************************************


// Um unter 64Bit Zeit in String zu wandeln
#ifdef _USE_32BIT_TIME_T
#define TIME_STR_LD "%ld"
#define TIME_STR_LU "%lu"
#define TIME_STR_LX "%lx"
#define TIME_STR_5LU "%5.5lu"
#else
#define TIME_STR_LD LLD_STR
#define TIME_STR_LU LLU_STR
#define TIME_STR_LX LLX_STR
#ifdef OS_WINDOWS
// change 13.06.2022 HS / 19.07.22 HS GNU10
#ifdef GNU10
#define TIME_STR_5LU "%I64llu"
#else
#define TIME_STR_5LU "%I64u"
#endif
//#define TIME_STR_5LU "%5.5llu"
#endif
#ifndef TIME_STR_5LU
#define TIME_STR_5LU "%5.5lu"
#endif // TIME_STR_5LU
#endif // __time32_t

#ifdef IS64BIT
#ifdef __TINYC__
#ifndef stat64
#define stat64 _stat64
#endif
#endif
#endif

#ifndef _countof
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif


// Sicherstellen das stat64 greift  || IS32BIT OS_LINUX?
//#ifndef _WIN64
#ifndef stat64
#define stat64 stat
#endif
//#ifndef statfs64
//#define statfs64 statfs
//#endif
//#endif

//#ifdef C
//#error "C is defined"
//#else
//#define _C (char *)
//#endif // C

#ifdef toolsmainmodul
#define tools_xtrn
#else // toolsmainmodul
//#pragma warning normal
#define tools_xtrn extern
#endif // toolsmainmodul

/** @} *//**
 @addtogroup c_init_tools
 @{
*/

tools_xtrn char    LogType;                     ///< Ausgabe wohin ? Siehe @ref LOG_Ausgabekanäle
tools_xtrn char*   LogFileName;                 ///< Vollständiger Filename zB: "C:\\HS\\Log\\tooltest-2020-03-23.log" * Ist ein Zeiger der, wenn unverändert, auf @ref m_Workfile zeigt
tools_xtrn char*   LogExtra;                    ///< Wenn nicht NULL, dann wird das als Logbeschreibung gewählt zB: "tooltest"
tools_xtrn bool    LogAtExitFlush;              ///< true/false zeigt das atexit bereits festgelegt wurde
tools_xtrn char    LogDir[PATH_MAX];            ///< zB: "C:\\HS\\Log

tools_xtrn unsigned int m_MAJOR;                ///< Hauptversion
tools_xtrn unsigned int m_MINOR;                ///< Nebenversion
tools_xtrn unsigned int m_BUILD;                ///< Aktuelles Build dieser Version (Hauptversion.Nebenversion)
tools_xtrn char*   m_BETA;                      ///< String Betastatus

tools_xtrn bool    lprintfConvertCRLF;          ///< lprintf löst (CR/)LF in 2 Zeilen auf. Auf false setzen um das zu verhindern
tools_xtrn bool    LogFileWithExtra;            ///< LogExtra (String) auch ins Logfile/Bildschirm schreiben
tools_xtrn char*   LogDateFormat;               ///< Format für das Datum im Logfile
tools_xtrn char*   SyslogDateFormat;            ///< Format für das Datum im Syslog -- nur unter Windows; wenn vorhanden wird die funktion syslog aufgerufen

tools_xtrn char    m_Workfile[PATH_MAX];        ///< zB: "C:\\HS\\Log\\tooltest-2020-03-23.log"
tools_xtrn char    HSDIR[PATH_MAX];             ///< Hauptverzeichnis zB: "C:\\HS"
#ifdef OS_WINDOWS
tools_xtrn char    SYSLOGFILE[PATH_MAX];        ///< zB: "C:\\Users\\Hesti\\AppData\\Local\\SYSLOG.log"
#endif
tools_xtrn void*   RConfig;                     ///< Start der RC-Tabelle von /etc/hsrc; kann NULL sein, wenn keine Datei gefunden wird etc.

tools_xtrn char    m_PRGNAME[FILENAME_MAX];     ///< zB: "tooltest"
tools_xtrn char    m_PRG_INFO[FILENAME_MAX+128];///< zB: "tooltest V10.19.752 Beta" kann im @ref InitTools mit %c erweitert werden
tools_xtrn int     m_PRG_argc;                  ///< Anzahl der Argumente
tools_xtrn char   **m_PRG_arguv;                ///< Liste der Argumente

/** @} *//**
 @addtogroup c_lprintf
 @{
*/

/// LOGGING INFORMATION -- Nach stdout / Nach stderr @see @ref LOG_Ausgabekanäle
// 08.07.20 HS nicht mehr #define, sondern enum's
enum LOG_ENUM {
    LOG_STDOUT = (1<<0),        ///< normal Bildschirm
    LOG_STDERR = (1<<1),        ///< stderr normal Bildschirm
    LOG_LOGFILE = (1<<2),       ///< LogFile
    LOG_SYSLLOG = (1<<3),       ///< syslog (Linux) oder wenn Spezielles Files (Windows)
//    LOG_SIGNAL (1<<4),
    LOG_CACHE   = (1<<5),       ///< Nachrichten werden temporär zwischengespeichert
    LOG_WINMSG  = (1<<6)        ///< nur Windows (Windows eigenes Systemfehler-Log bedienen)
};

/// Die einzelnen Stufen der Lognachrichten
enum LMSG_ENUM {
    LMSG_SILENT=0,              ///< no Extra Logfile, no MSG [ beim Final / im Debug ausgeben, wie es definiert ist ]
    LMSG_CRITICAL,              ///< Programm Fehler, ua. auf jeden Fall welche die zum Programmabbruch führen
    LMSG_ERROR,                 ///< Minimales Fehler logging, dh. nur Fehler die unbedingt ins Logfile gehöhren, Wenn Programmteile übersprungen werden ect.
    LMSG_WARN,                  ///< Einfache Fehler, die übergangen werden können
    LMSG_EVERY,                 ///< (default) Auch Nachrichten, die über geglückten Programmteil informieren
    LMSG_MOST,                  ///< Im Detail informieren
    LMSG_MAX                    ///< Debug-Artiges ausgeben, jedoch kein Debug
};

/// struct zum Speichern einer einzelner Stufen (Lognachrichten) Siehe: @ref LMSG_ENUM
typedef struct lmsg_s {
    int     out;                ///< Ausgabe wohin, da über lprintf ausgegeben wird, als LOG_xxx angaben
    char*   str;                ///< Wie die Ausgabe heißt, als nutzbaren string
}lmsg_t;

tools_xtrn lmsg_t lmsg_def[LMSG_MAX+1]; ///< Array mit den Definitionen der einzelenen Messageleveln, siehe @ref lmsg_init
tools_xtrn int lmsg_level;      ///< Aktuell gültiger LogLevel Siehe @ref LOG_ENUM
tools_xtrn int lmsg_format;     ///< 0-2 default: 0 - legt fest, ob zusätzlich das LogLevel der Nachricht mit erfasst werden soll. \n 0=NEIN \n 1=in [] den numerischen Wert \n 2=in [] den Wert als Text

/** @} *//**
 @addtogroup c_args
 @{
*/
tools_xtrn char     *ARG;                       ///< Pointer auf ein Argument wenn angegeben; sonst NULL
///@}

//#include "t_diskstate.h"
//definied in linux HS 27.11.17
#ifdef OS_WINDOWS
int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info);
#endif

// Der zentrale BasisInit. Muss aufgerufen werden
int InitTools ( int arguc, char *arguv[] , char* opts, ... );

// Argumente via main verarbeiten
//_____________________________________________________________________________

void InitARG (int arguc);
int arg_GetNext(int point, int arguc);
void arg_Clean (void);
int arg_unused_print(int argc, char *argv[]);
int aarg_unused_print(void);
int ChkPar(int val);
int ChkARGwStart(char *Find, int start, int arguc, char *arguv[]);
int ChkARG(char *Find, int arguc, char *arguv[]);
int aChkARG(char *Find);
int ChkARGlong(char *Find, int argc, char *argv[]);
int aChkARGlong(char *Find);

// die LogPrint-Version
int lprintf(char *fmt, ...);  // der (interne) Lognachrichten Aufruf
void exitlprintfflush(void);  // für atexit();
int lprintfflush(void);       // flushing LogFile Informationen

int lmsg_init(void);
int lmsg (int behavior, char *fmt, ...);

/**
 @addtogroup c_uname
 @{
*/
// uname gibt es unter Windows nicht...
// unter Linux wird dafür sys/utsname eingebunden
// 07.02.2019
#ifdef OS_WINDOWS
#define __NEW_UTS_LEN 256
struct utsname {
    char sysname[__NEW_UTS_LEN + 1];
    char nodename[__NEW_UTS_LEN + 1];
    char release[__NEW_UTS_LEN + 1];
    char version[__NEW_UTS_LEN + 1];
    char machine[__NEW_UTS_LEN + 1];
    char domainname[__NEW_UTS_LEN + 1];
};
int uname(struct utsname *name);
int readWindowsRegistery(HKEY keyParent, char *keyName, char *keyValName, char *buffer, int bufsize);
#endif // OS_WINDOWS
///@}

size_t strftimeR(char *s,int maxsize, const char *format, const struct tm *t);
int strftime_fmt(const char *format, const struct tm *t, char **in_out_pt, int *max);

//_____________________________________________________________________________
//
// Stringmanipulationen
//_____________________________________________________________________________
//
// 02.04.2000 HS ReCreate
//_____________________________________________________________________________

//müssen für Linux defniert werden 27.11.17 HS
#ifdef OS_LINUX
char *strlwr( char *Kette );    // Konvertiert Kette zu lowercase
char *strupr( char *Kette );    // Konvertiert Kette zu uppercase
#endif
#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

int strcount(char *STRING,char FOUND);                                          // Wie oft ein Zeichen in einem String ist
char *strdel(char *STRING, int pos, int len);                                   // Strings loeschen
void strins(char *STRING, char *INS, int pos);                                  // und einfügen
int strstradd (char *start_ptr, char **end_ptr, char *str, int *size_cnt);      // Ähnlich wie strcat, Rückgabe ist Anzahl Zeichen und es wird ein Pointer zurückgegeben
char *strstrcat(char *STRING, char *AddStr);                                    // Ähnlich wie strcat, nur wird mit einem malloc/realloc gearbeitet
void strReplace(char *C, char FROM, char DEST);                                 // Bestimmte Zeichen austauschen
int strright(char *C, char Findchar);                                           // Prüft das letzte Zeichen im String=Findchar ist,
                                                                                // gibt -1 oder Position zurück
void strsetto(char *dest, char filler, int counter);                            // String beliebig mit einem Zeichen füllen
char* strfiller (char filler, int counter);                                     // gibt einen temp. Struing wie oben zurück
char *strNotNULL(char *C);                                                      // Gibt definitiv einen String zurück bei NULL ""
char *LeftStr (char *STRING, int len);                                          // x linke Zeichen von einem String zurückgeben
char *RightStr (char *STRING, int len);                                         // x rechte Zeichen von einem String zurückgeben

void strltrim(char *C);                                                         // Linke Spaces löschen
void strrtrim(char *C);                                                         // Rechte Spaces löschen
void strCL   (char *C);                                                         // Linke und Rechte Spaces löschen
void strclean (char *SRC, char *RemoveAnyChar);                                 // Jeden Char aus RemoveAnyChar aus einen String löschen
void strdelchar (char *SRC, char rm);                                           // Einzelnes Zeichen aus einem String löschen
char *strnum(int num);                                                          // Eine Zeichenkette aus einem INT machen
char *strnumformated(int num, char pad, int sized);                             // Dasselbe nur um 2 Parameter erweitert
void Long2IStr ( char *Kette, unsigned long Wert);
void LLong2IStr ( char *Kette, unsigned long long Wert);
char *HSnum2Str ( unsigned long Wert, char pad, char trenn, int maxsize);       // Integer
char *HSLnum2Str ( unsigned long long Wert, char pad, char trenn, int maxsize);
char *stridx(char *STRING, int idx);                                            // Ein Argument (idx) aus eine String suchen
int strstrpos(char *STRING,char *FIND, int from);                               // Position der Zeichenkette FIND im gegebenen String STRING ab Position from
int strchrpos(char *STRING,char FIND, int from);                                // Position der Zeichenkette FIND im gegebenen Char ab Position from
char *strstrreplace (char *STRING,char *FIND, char *REPL);                      // in STRING Zeichenkette (FIND) austauschen gegeb REPL
char *strstrarg (char *STRING, int arg, char STRARG_DEL);                       // In der Zeichenkette nach STRARG_DEL getrennter Argumente durchsuchen
int isOneString(char *STRING);                                                  // gibt EXIT_SUCCESS im Erfolgsfall zurück.
int strsplit(char *Src, char delimiter, char **DLeft, char **DRight);           // Src aufteilen; Bei Erfolg=0 DLeft und DRight werden mit malloc geholt.
int sprintf_ex (char *DST, char *fmt, ...);                                     // macht das Gleiche wie sprintf, nur dass der GCC nicht meckert
char *strcpy_ex(char *dst, const char *src);                                    // siehe oben
char *strncpy_ex(char *dst, const char *src, int len);                          // siehe oben
char *strcat_ex(char *dst, const char *src);                                    // siehe oben

void *memset_ex(void *dst, int val, size_t len);                                // macht das Gleiche wie memset, nur dass der GCC nicht meckert
void *memcpy_ex(void *dst, const char *src, size_t len);                        // siehe oben

/**
 @addtogroup c_strchr_strstr
 @{
*/

/**
 @brief Typendefnition des <b>Y</b>-string <b>T</b>ypen

 Dabei ist hier Text
*/

typedef struct t_ty{
    char *buffer; ///< Datenbuffer ( __NICHT__ Null terminiert )
    int maxsize;  ///< Speichergrösse, die bisher angefordert wurde
    int pntr;     ///< zeigt hinter das zuletzt geschriebene Zeichen, bzw auf nächste zu schreibende Zeichen
}ty;
///@}


void y_dump (ty *y);                                                            // Debugfunktion nur aktiv, wenn HS_DEBUG gesetzt ist
void y_init (ty *y, char *b);                                                   // Buffer vom Typ ty initialisieren
void y_free (ty *y);                                                            // Buffer vom Typ ty freigeben; kann direkt wieder verwendet werden
void y_end  (ty *y);                                                            // Ein Bufferende markieren ohne den Zeichenpointer zu ändern
void y_charadd (ty *y, char c);                                                 // Zeichen dynamisch in einen Buffer vom Typ ty schreiben
void y_multicharadd (ty *y, char c, int nsize);                                 // Ein Zeichen mehrfach dynamisch in einen Buffer vom Typ ty schreiben
void y_stringadd (ty *y, char *s);                                              // Einen String an eine dynamischen Buffer vom Typ ty schreiben
void y_numadd (ty *y, long long u, unsigned int base,                           // Eine Zahl formatiert in den Buffer vom Typ ty schreiben
                 bool altformat, bool capitalize,
                 size_t num_size, int num_prec, char num_padc,
                 bool num_adjustment_left, char add_plussign);

char *vstrprintf(const char *fmt, va_list argp);                                // internes printf ohne CLib-Funktion
char *strprintf(const char *fmt, ...);                                          // ruft intern @ref vstrprintf auf schreibt

/**
 @addtogroup c_strchr_macros
 @{
*/
//@addtogroup c_tools_globalvars
// 26.06.16 HS Helping function

/// Einen String dynamisch erweitern
#define stradd(X,Y) X=strstrcat(X,Y)
/// Einem String links und rechts Leerzeichen entfernen siehe @ref strCL
#define CL strCL

#define STRMTCH_CASE    0b10000000
#define STRSTRSPLIT_SPACE_DELIMITTER 0x01
#define STRSTRSPLIT_TAB_DELIMITTER   0x02
#define STRSTRSPLIT_LF_DELIMITTER    0x04
#define STRSTRSPLIT_SINGLE_QUOTES    0x10
#define STRSTRSPLIT_DOUBLE_QUOTES    0x20
#define STRSTRSPLIT_NO_QUOTES_RETURN 0x80
#define STRSTRSPLIT_DEFAULT ( STRSTRSPLIT_SPACE_DELIMITTER | STRSTRSPLIT_TAB_DELIMITTER | STRSTRSPLIT_LF_DELIMITTER | STRSTRSPLIT_SINGLE_QUOTES | STRSTRSPLIT_DOUBLE_QUOTES )
///@}

int strmtch (const char *pat, const char *str, int flags);                      // Zeichenkette Matching prüft ob str mit dem Muster in pat übereinstimmt

char **strlst(char *strin);                                                     // String Argumente aufteilen. Es wir eine Liste der Argumente zurückgegeben
char *strstrsplit (char *start_ptr, char **end_ptr, int position, int flags);   // Aus einem String ein Argumente lesen und zurückgeben. Kann NULL sein.

char *strEsc(char *Kette);                                                      // Text Ende mit NULL konvertieren
char *strDeEsc(char *Kette);                                                    // Text Ende mit NULL zurück konvertieren
// Beide Ergebnisse sind die Folge von malloc und müssen nachher freigegeben werden

// ????? 05.07.2020 HS wozudas, gibt keine Funktion mehr...
//unsigned long atoul(char *STRING);

char *CleanNumberAfterDot(char *Kette);                                         // Unterroutine von Num2Human * Löscht unerwünschte Nullen
double simple_round(double num, int perc);                                      // double Zahl runden, auf 'perc' nachkommastellen
double Human2Num(char *Kette);                                                  // 10KB nach double wandeln
char *Num2Human(double num, int perc);                                          // Zahl in lesbare Form bringen
double strexpr(char* expr);                                                     // double zahl von einer expression
double strexpr_r(char* expr, char** end_ptr);                                   // das Gleiche nochmal, aber mit einem Rückgabewert

int fromHex(char *start_p, char **end_p, int len, long long* rslt);
long hextol(char *STRING);
//unsigned long hextoul(char *STRING);
char *strtohexstr(char *from, int format);                                      // String von einem String in HexDump erzeugen

char *binstr( unsigned int num );
void revstr (char *str);

int GetIntTrueFalsefromString(char *STRING);

// 0x7ffffffffffffffffLL
#define LONGLONGRAND_MAX 9223372036854775807LL

long long randomnumber(long long maxrand);
//int randomnumber(int maxrand);                                                  // Liefert eine Zahl im Bereich von 0-32767 ( bzw. von maxrand ) begrenzt
char *tempdir(void);                                                            // Liefert eine Zeichenkette in Abhängigkeit vom Betriebssystem zurück
char *tempfilename(char *ins);                                                  // Liefert einen Filenamen ... wobei ins auch NULL sein kann


//#ifndef malloc0
void *malloc0(size_t size);
//#endif
void* free0(void *Kette);

/* Unixtime Funktionen */
time_t unixtime(void);                                                          // Unter Unixservern typisch; call time time(NULL);
int weekday(int year, int month, int day);
int time_tIsDST(time_t TIME);
int Date_Is_DST (int Year, int month, int day, int hour);
int isLeapYear(const int Year);
int DaysOfMonth(int Year, int month);                                           // letzter Tag im Monat

char *strstime(time_t ltime, char *format);
char *strtime(time_t ltime, int format);
char *datestr(time_t TIME);

time_t Midnight(time_t t);

// Parsing Date
int mktime_s ( time_t unxtme, int *isYear, int *isMon, int *isDay, int *isHrs, int *isMin, int *isSec);
time_t mktime_t(int year, int mon, int day, int hour, int min, int sec, int isday);
time_t ParseAnyDate(char *ostr);
time_t ParseFmtTime(const char *fmt,const char *str, int *len);
/**
 @addtogroup c_timerfunc
 @{
*/
#define PARSEANYDATE_ERROR ((time_t)(-1))
struct TYPE_MONNAMES
{
    int mon;
    char *name;
};
///@}

char *timeinfostr(time_t TIME);                                                 // Make info from given timevalue
char *timestamp(time_t TIME);                                                   // Make timestamp from given timevalue
char *shorttimestr(time_t TIME);                                                // Make short datestr from timevalue
char *miltaryToHumanDate(char *String);                                         // yyyy-mm-dd to dd.mm.yyyy

/**
 @addtogroup c_dumpfunc
 @{
*/

// Dump Strings bytes und buffer meistens für debugging
//___________________________________________________________________________

#define MAXDUMP 16

#define DUMP_STDOUT  1
#define DUMP_STDERR  2
#define DUMP_LOGFILE 4
#define DUMP_SYSLLOG 8
#define DUMP_STRING  16

tools_xtrn int dumpbyte;
tools_xtrn int dumpcount;
tools_xtrn int dumpmode;
tools_xtrn char dumpstring[MAXDUMP];
tools_xtrn void *NODE_HEAD; // gehöhrt zum DUMP Irreführende Variable
///@}

void DumpInit       ( int DUMPMODE );
void DumpAddByte    ( int byte );
void DumpAddString  ( char *String );
void DumpPrint      ( void );
void DumpAddBuffer  ( char *String, int Len );
void DumpSingleBuffer(int DUMPMODE, char *String, int Len );
char *DumpString    (void);
char *DumpFullString(void);


// File-i/o extras, don't need a class to use this
//_____________________________________________________________________________

int mgetline ( FILE *Stream, char *Dest, int MaxLen );

// Prüft auf realles vorhandensein
int DirOK   (char *name);
int FileOK  (char *name);

int IsRoot   (char *name);                              // Ist Hauptverzeichnis / unter Unix C:\ oder D:\ unter Windows
int IsSystem (char *name);

// links -> https://krausens-online.de/hard-softlinks-teil-1-theorie-laaaangweilig/
int IsLink(char *name);                                 // Also eine Verlinkte Datei Winows.lnk Dateien gelten nicht als Links!!
int IsSoftLink(char *name);                             // Softlinks "ln -s Soft.link datai.txt"
int IsHardLink(char *name);                             // Hardlinks "mklink /J HARDLINK directory"
//int IsSymbolicLink(char*name); //testweise für windows

long long filesize64(char *name);
long    filesize(char *name);
time_t  filedate(char *name);
int     display_filecopy (char *dummy, int pct);
int     filecopy(char *Source, char *Dest);
int     filecopy_pct(char *Source, char *Dest, int (*func) (char *, int), int pct_start, int pct_end);

int copyfiletime (char *from, char *dest);
int copyFileOwner(char *from, char *dest);
int copyFileMod  (char *from, char *dest);

int createdir(char *dirname);                           // Make a subdir
char *Cbasename (char *Kette);                          // Filename ohne Pfad
char *CfilenameExt(char *filename);                     // Extension eines Files ermitteln oder leeren String
char *CfilenameName(char *filename);                    // Nur der Name. Keine Extension oder Pfad eines Files ermitteln, wenn nicht vorhanden -> leeren String
char *Cdirname (char *path);                            // Directory ohne den vollen Pfad
char *Cdirvault (char *path);                           // wie Cdirname, aber unabhängiger cdirname wird letzendlich doch aufgerufen
char *univault(char *path);                             // Austauschen von Variablen die mit getenv() geholt werden
char *CFilevault (char *path);                          // kompletten Pfad korrigieren
void CorrectPath(char *path);                           // Angegebenes Verzeichnis auf das aktuelle Betriebssystem anwenden

void *RCread(char *filename);                           // RC-Datei in den Speicher lesen
char *RCfind(void *RCNodes, char *var);                 // im Speicher nach RC-Wert suchen und die Value zurückgeben; wenn vorhanden, dann NULL


void GetCWD ( char *Kette );
char *strgetCWD(void);
int save_chdir(char *path);

int getach(void);

// aktuelle Zentralfunktion um freien/belegten Speicherplatz feststellen zu können
int diskstate(char *Kette);
unsigned long long diskfree  (char *Kette);
unsigned long long disktotal (char *Kette);
unsigned long long diskused  (char *Kette);
struct hs_disk_info{
    unsigned long long Free;
    unsigned long long Total;
    unsigned long long Used;
//	unsigned long long t,a,b,c;
};
tools_xtrn struct hs_disk_info hs_diskinfo;



void *Node_Add (void* StartNode, void *Data, int direction );
void *Node_Add_After(void *StartNode, void *After, void*Data);
void *Node_DelFirstNode(void *StartNode);
void *Node_DelOne(void *StartNode, void *NodeToDel);
void *Node_DelAll(void *StartNode);
void *Node_GetNext(void *StartNode);
void *Node_GetData(void *StartNode);
int   Node_Count(void *StartNode);


typedef int (*Nodecompfn)(const void*, const void*);
void *Node_Sort(void *Stck, Nodecompfn Compare);

// Stack Funktionen
// Es muß ein void* definiert werden. Zum Init muß der auf NULL zeigen
// Bei Verwendung von Stack müssen DATA ist die Datenverwaltung Usersache
// Entweder mit malloc und free arbeiten oder aber feste Daten
//
// STCK_push(STACK,DATR)  LiFo Stack aufbauen (Last in First Out)
// STCK_pushl(STACK,DATA) LiLo Stack aufbauen (Last in Last Out)
// STCK_pop(STACK,DATA)   DATA von Stack holen.

/**
 @addtogroup c_chainlist
 @{
*/
#ifdef HS_DEBUG
tools_xtrn int NodesSortChanges;  ///< nur in der DEBUG-Version hochgezählt
#endif // HS_DEBUG
struct n_Node {
    void *n_NEXT;   ///< Einfach Pointer auf den nächsten Datensatz
    void *n_DATA;   ///< mit malloc geordeter Speicher
};

#define STCK_push(STACK,DATA)   STACK = Node_Add (STACK,DATA,false)
#define STCK_pushl(STACK,DATA)  STACK = Node_Add (STACK,DATA,true)
#define STCK_pop(STACK,DATA)    DATA  = Node_GetData (STACK); STACK = Node_DelFirstNode(STACK)
///@}

#ifdef  __cplusplus
}
#endif


#undef tools_xtrn
#endif //__TOOLSC__


/*EOF*/
