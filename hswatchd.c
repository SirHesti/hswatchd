/* *****************************************************************************
Dokumentation bis zur Version 1.9.0 Finale
muss angepasst werden insbesondere config hat sich geändert
********************************************************************************
#-[ sed.file ]
hstex -x -r hswatchd.c -w sed.file -e sed.file
s/$DATE/%DATE%/g
#-[ README.md
# hswatchd

## English
*I would like to point out that I always document my software in German.*
a daemon for monitoring files
hswatchd is designed for the lowest possible CPU performance. It is started via the *systemd*. If you want, feel free to create an init solution.
All parameters are entered in **_/etc/hswatchd.rc_**. An example is included or can be copied from [github](https://github.com/SirHesti/hswatchd).
Each time a file has been changed, the function or file is executed.

## German / Deutsch
Ein Daemon zum Ueberwachen von Dateien
hswatchd ist ausgelegt auf kleinst moegliche CPU-Performance. Er wird ueber den *systemd* gestartet. Wer moechte kann sich gerne auch eine init-Loesung schreiben.
Alle Parameter werden in der **_/etc/hswatchd.rc_** eingetragen. Eine Beispiel wird mitgeliefert oder kann auf von [github](https://github.com/SirHesti/hswatchd) kopiert werden.
Jedesmal wenn eine Datei geaendert wurde, dann wird die eingestellte Funktion, bzw. Datei ausgefuehrt.

#-[ hswatchd.man

---
title: hswatchd
section: 8
header: hswatchd - copyright Hesti
footer: hswatchd $VERSION
date: $DATE
---

# NAME
**hswatchd** - Ein Daemon zum Ueberwachen von Dateien

# SYNOPSIS
Daemonen werden ueber "systemd" gestartet. Dieser ueberwacht Dateien auf Aenderung und fuehrt, wenn vorhanden, einen Befehl aus.

# DESCRIPTION
| hswatchd ist ausgelegt auf kleinst moegliche CPU-Performance. Er wird ueber den *systemd* gestartet. Wer moechte kann sich gerne auch eine init-Loesung schreiben.
| Alle Parameter werden in der **_/etc/hswatchd.rc_** eingetragen.

# OPTIONS
**\-\-version **
:die aktuelle Version ausgeben lassen. Das Programm wird allerdings dann beendet.

Da hswatchd als Daemon gestartet wird, gibt es fuer den Betrieb keine Optionen. Dafür wird eine hswatchd.rc erwartet.

# SIGNALS
**SIGHUP,SIGKILL,SIGINT**
: Der Daemon wird beendet

**SIGUSR1**
: reload der hswatchd.rc

**SIGUSR2**
: ausgabe der Infodatei

# ENVIRONMENT
Starten, stopppen, service:
: siehe Anschnitt: **SYSTEMD**

Konfiguration:
: siehe Anschnitt: **hswatchd.rc**

# SYSTEMD
Sicher gibt es viele Wege um einen Daemon zu laufen zu bekommen. Derzeit wird aber SYSTEMD ueberall verwendet.

Achtung. Zum grossen Teil werden ROOT-Rechte noetig sein.

| Datei erstellen
| :/etc/systemd/system/hswatchd.service

| Inhalt:

``` hswatchd.service
[Unit]
Description=hswatchd - demon watching files for changing
After=network.target

[Service]
ExecStart=/usr/sbin/hswatchd
ExecStop=/bin/kill -HUP $MAINPID
ExecReload=/bin/kill -USR1 $MAINPID

StandardOutput=null
#StandardOutput=/root/out.txt
#Restart=on-failure

[Install]
WantedBy=multi-user.target
```

#EOF

| Service einrichten (einmalig, started danach automatisch mit):
``` Service
systemctl daemon-reload
systemctl enable hswatchd.service
systemctl start hswatchd.service
systemctl status hswatchd
```

# hswatchd.rc

| **Parameter:**

\# Server Einstellungen
: Zeilen, die mit '#' beginnen, sind Kommentare und werden entsprechend ignoriert. Uebrigens auch Leerzeilen

port        = 8080
: Portangabe. hswatchd kann über http abgefragt werden
: siehe Anschnitt: **HTTP*

time        = \#\#\#
: Defaultangabe zum verweilen zwischen einer Pruefeinheit. Ist nicht so wichtig, da die Zeit automatisch errechnet wird.

cache       = _true_ oder _false_
: Die Benutzung des Caches erlauben/verbieten

cachepath   = _Pfad_zur_Cachedatei_
: Der Pfad für eine chache-Datei. Es wird in dieser Datei die sekundengenaue Zeit der letzten Pruefung jedes File gespeichert. Gefolgt von dem Dateinamen.

infoupdate  = 3600
: In diesem Fall wird alle 3600 Sekunden ein Infofile erstellt.

infofile    = _File_fuer_InfoFile_
: Der vollständige Pfad zur Datei. Es wird eine reine Textdatei geschrieben. Ist Markdown formatierbar. Diese Angabe kann auch leer sein (oder besser auskommentiert) werden.

favicon     = /srv/pub/share/hswatchd/pc-hesti.png
: Pfad zu einem FAV-Icon für die html-funktion. Wer keines möchte läst es weg. Für jeden im eigenen Netz laufenden hswatchd lässt sich damit ein anderers ICON realisieren

Jeder Eintrag mit "file ...." in der hswatchd.rc wird in eine einfach verkettete Liste
sortiert, nach naechster Bearbeitungszeit, aufgenommen. Dadurch wird erreicht das der
naechste zu verarbeitende Datensatz ganz oben steht. Die Diff-Zeit wird auch genommen,
um die naechste Wartezeit zu setzen. Ist so ein Datensatz abgearbeitet, wird neu
entschieden, wo er in der Liste zu stehen hat. Und es wird eine neue Wartezeit
bestimmt. Diese Informationen koennen auch über http abgerufen werden !!

| Konfigurations-Datei erstellen
| :/etc/hswatchd.rc

### Beispiel:

``` hswatchd.rc
# systemweite hswatchd - Konfigurationsdatei

port        = 8080
time        = 1
cache       = true
cachepath   = /var/hswatchd/files.cache
infoupdate  = 3600
infofile    = /srv/pub/www/data/watch-debug-hesti.txt
favicon     = /srv/pub/share/hswatchd/pc-hesti-debug.png

# File - Section
file /pub/share/network/stuff.txt       25  /bin/bash /root/bin/doany.sh inplement stuff
file /pub/share/network/admin_hosts     60  cp /pub/share/network/newfile_hosts /etc/hosts
```

# HTTP-Service
hswatchd verfügt eine rudimentäre html-Schnittstelle. Hier lassen sich ein paar Informationen abrufen.
Der Port ist in der _hswatchd.rc_ zu definiert.

_server:8080/reload.cgi_
: wird hswatchd dazu veranlassen die hswatchd.rc neu zu laden.

_server:8080/date.cgi_
: gibt das aktuelle datum mit der Zeit aus. Das war nur zum Testen drin, aber es stoert ja auch niemanden.

_server:8080/nextwatch.html_
: Eine ausfuerliche Seite, welche Dateien als naechstes geprueft werden und eine kurze Uebersicht welche Parameter mit wirken.

_server:8080/drive.cgi?_
: Gefolgt von dem Mount-Point gibt ein paar Informationen ueber gemountete Laufwerke auf dem Server preis. Diese koennen so
gezielt gesammelt werden. Ein Aufruf wie _server:8080//drive.cgi?&boot_ wird die RootPartition und die boot-Partition ausgeben.

#EOF

# AUTHORS
Heiko Stoevesandt - alias Hesti - <hstools@t-online.de>

# FEHLER
Fehler (auch in dieser Manpage) unbedingt bitte melden

#-[ hswatchd.rc ]
#
# Server Einstellungen
#
# ----------------------------------------------------------------------------------------------------------------------------------
port        = 8080
time        = 1
cache       = true
cachepath   = /var/hswatchd/files-debug.cache
infoupdate  = 3600
infofile    = /srv/pub/www/data/watch-debug-hesti.txt
favicon     = /srv/pub/share/hswatchd/hswatchd.png
#
#
# File - Section
#
# ----------------------------------------------------------------------------------------------------------------------------------
#
file = /srv/pub/share/network/hswatchd-hesti.rc                 10 /bin/bash /hs/src/cons/hsrelated/hsrelated-test.sh
file = /var/habsnicht.cfg                                       12 /bin/bash /hs/src/cons/hsrelated/hsrelated-test.sh
file = /srv/pub/share/sys-crons/syssh.export                    25 /bin/bash /hs/src/cons/hsrelated/hsrelated-test.sh
file = /srv/pub/share/network/hosts                             15 /bin/bash /hs/src/cons/hsrelated/hsrelated-test.sh
file = /srv/pub/share/network/hs-logs.export                    35 /bin/bash /hs/src/cons/hsrelated/hsrelated-test.sh
file = /srv/pub/share/sys-crons/systemd-startup-hesti.conf      58 /bin/bash /hs/src/cons/hsrelated/hsrelated-test.sh
#file = no
# ----------------------------------------------------------------------------------------------------------------------------------

#-[ ende ----------- ]

 kill -s USR1 $(pidof hswatchd)
 wird die Config neu eingelesen (so aehnlich wie ein Reload)

 kill -s USR2 $(pidof hswatchd)
 schreib eine aktuelle Uebersicht in ein TextFile (Man beachte die Markdown
 Formatierung)

 Installation: siehe Manpage

#-[ Revision ]-------------------------------------------------------------------

 06.10.20 HS cache eingebaut
 28.10.22 HS infopath neu ueber usr2 aktuelles infos
 31.10.22 HS next in secs in der Uebrsicht
 01.11.22 HS Die einfach verkettete List LST wird permanent angepasst, so dass
             der naechste Job immer oben steht
 02.11.22 HS Manpage
 27.02.24 HS Beginn des Rewrite (Kern behalten)
 28.02.24 HS Neu: ReadConfig
 29.02.24 HS Neu: ptreads ausprobiert
 01.03.24 HS Erste Html-Pages mit Tabellen
 03.03.24 HS Im EchtTest aus dem Nas und bei mir (hsmake und install angepasst)
 07.03.24 HS Remote-drive jetzt über HTTP gelöst
 18.05.24 HS retry98 ist wieder aktiv und über eine Wrapperfunktion gelöst
 23.05.24 HS HSwatchd läuft weiter während noch ggf. der Socket für html geöffnet wird
 25.05.24 HS ptreads verworfen
 26.05.24 HS html auswertung neu
 12.07.24 HS favicon neu

 -------------------------------------------------------------------------------
*/

#include "tools.h"
#include "VERSION.h"
#include <mntent.h>
#include <signal.h>
#include <fcntl.h>

#include <linux/unistd.h>               // Alle drei für sysinfo
#include <linux/kernel.h>
#include <sys/sysinfo.h>

#define MAX_BACKLOG 16

enum mainLoop {
    LOOP_NORMAL,                        // Normaler Durchlauf mit Connect/Writejobs/ChangeDetect
    LOOP_STOP,                          // Exit mainloop
    LOOP_RELOAD,                        // Reload Config
    LOOP_INFOMODE,                      // Write Infofile to Text
    LOOP_FIRSTRUN                       // Erster Schleifendurchlauf
};

typedef struct t_file {
    char *file;                         // file das zu prüfen ist
    char *execute;                      // Kommando, das ausgeführt werden soll
    time_t last_modif;                  // letzte modifizierung des Files
    time_t last_check;                  // wann zuletzt geprüft
    int interval;                       // wie oft zu prüfen
} t_file;

void    *LST = NULL;                    // Liste von File mit t_file beschrieben
int     loopState;                      // aktiver Status zum Durchlauf
time_t  now;
int     sock;                           // listen socket
long    tipwaittime;
time_t infoupdate;                      // Wann gabs den letzten WriteInfo configurierbar in cfgInfoTime
time_t starttime;                       // Serverstart
time_t reloadtime;                      // wann wurden die letzten Variablen gelesen
time_t retry98start;                    // start einer missglueckten connection

struct  sockaddr_in sock_in;            // Buffer / Pointer um Connect aufzubauen
struct  sockaddr * sin_addr;
socklen_t sin_len;

void  m_sig(int sig);
int   open_connection_with_retry(void);
int   open_connection(void);
int   close_connection(void);

int   runMainLoop(void);                // Dauerschleife um alles am Laufen zu halten
int   mainProcess(void);                // ChangeDetect
int   cleanup(void);                    // Am Ende (oder vor eine ReadConfig) aufräumen

int   ReadConfig(void);                 // Config neu lesen. z.B. bei änderungen
int   cfg_filecmd(char *Line);          // Ein FileEintrag einlesen
int   AddIDXByDate(struct n_Node *IDX); // Node nach Execute-Datum einsortieren

int   WriteOutCache(void);              // Cache nach Änderung auf Platte schreiben
int   UpdateFromCache(void);            // Nächstes Execute anhand des Caches bestimmen, Wenn mtime!= , dann machen

int ConnectProcess(void);               // Connect aufbauen, wenn Erfolg, dann neuen Prozess mit der Abarbeitung bestimmen
                                        // kommt immer zurück und kann dann auch keinen Fehler haben
void WriteInfo(void);                   // Daten als Text Schreiben
void ConnectExecute(void);              // wird in ConnectProcess() gestartet und führt einen Connect aus und bearbeitet ihn

#ifdef HS_DEBUG
void  printConfig(void);                // zum Debuggen
#endif

//open_connection

uint16_t  cfgPORT       = 12411;        // listen http-port
long      cfgTime       = 10L;          // Zeit'um auf eine Connection zu warten
int       cfgUseCache   = False;        // filedaten nicht lesen, sondern einen cache nutzen
char*     cfgCacheFile  = NULL;         // welches file nutzen zum cachen
int       cfgInfoUpdate = -1;           // wie oft soll ein infofile geschrieben werden, wenn selbst addressiert, dann frisch generiert und -1 OK
char*     cfgInfoFile   = NULL;         // wohin soll ein update geschrieben werden
int       cfgRetry98    = 3600;         // seconds to retry 98-Error; after an error 98 on bind, wait 5 seconds an try again 10 Minuten lang

// FavIcon
char*     cfgFavIcon    = NULL;         // Config für das favicon
time_t    FavIconTime;                  // Für eine mögliche änderung
char*     FavIconBuffer = NULL;         // das eigendliche FavIcon in binaer
size_t    FavIconBufSz;                 // und wieviel Speicher es in Anspruch nimmt

/* *****************************************************************************

    Starter

****************************************************************************** */


signed int main(int argc, char *argv[])
{
    int rc;
    if (InitTools(argc , argv, "%d%v%t%m", I_MAJOR, I_MINOR, I_BUILD, I_BETA, LOG_LOGFILE | LOG_SYSLLOG)) return -1;
    lprintf("%s on pid %i",m_PRG_INFO, getpid());
#ifdef HS_DEBUG
    LogType = LogType | LOG_STDERR;
#endif
    starttime=unixtime();
    rc = ReadConfig();
    if (rc) return rc;

#if defined (OS_LINUX) && (!defined (HS_DEBUG))
    signal(SIGABRT, m_sig);
    signal(SIGQUIT, m_sig);
    signal(SIGHUP,  m_sig);
    signal(SIGKILL, m_sig);
    signal(SIGINT,  m_sig);
    signal(SIGUSR1, m_sig);
    signal(SIGUSR2, m_sig);
#endif
//    rc = open_connection_with_retry();
//    if (rc) return rc;
    sock = -1;
    rc = open_connection_with_retry();
    if (rc==98) rc = close_connection(); // close_connection kommt immer mit 0 zurück
    if (rc) return rc;
    rc = runMainLoop();
    close_connection();
    lprintf( "%s hat sich beendet", m_PRGNAME);
    cleanup();
    return rc;
}

int runMainLoop(void)
{
    int rc;
    rc = 0;
    loopState = LOOP_FIRSTRUN;
    now = unixtime();
    infoupdate = now;
    for (;;)
    {
        now = unixtime();
        if (loopState == LOOP_STOP)
        {
            lprintf ("close requested");
            rc = EXIT_SUCCESS;
            break;
        }

        if (loopState == LOOP_FIRSTRUN)
        {
            if ((rc=mainProcess())) break;
            loopState = LOOP_INFOMODE;
        }

        if (loopState == LOOP_INFOMODE)
        {
#ifdef HS_DEBUG
            lprintf ("WriteInfo()");
#endif
            infoupdate = now;
            WriteInfo();
            loopState = LOOP_NORMAL;
        }

        if (loopState == LOOP_RELOAD )
        {
#ifdef HS_DEBUG
            lprintf ("loop -> RELOAD execute");
#endif
            close_connection();

            if ((rc = cleanup())) break;
            if ((rc = ReadConfig())) break;
            if ((rc = open_connection_with_retry())) break;
            loopState = LOOP_NORMAL;
        }

        if (loopState == LOOP_NORMAL)
        {
            rc=mainProcess();               // Tabelle abarbeiten
            if (rc) break;

//#ifdef HS_DEBUG
//            loopState=LOOP_STOP;
//            break;
//            if (tipwaittime>5L) tipwaittime=7L;
//#endif

#ifdef HS_DEBUG
//                char mainloopstr[128];
//                sprintf_ex(mainloopstr, "mainloop %ld", tipwaittime);
//                lprintf("%s", mainloopstr);
            int tempLogType = LogType;
            LogType &=~ LOG_SYSLLOG;
            lprintf("mainloop %ld", tipwaittime);
            LogType = tempLogType;
#endif
        }

        if (loopState == LOOP_NORMAL)
        {
            if (sock<0)
            {
                rc = open_connection_with_retry();
                if (rc == 98) rc=close_connection();
            }
            if (sock>=1) ConnectProcess();
            if (rc==0)
            {
                if (cfgInfoUpdate>0)
                {
                    if (now>(infoupdate+(time_t)cfgInfoUpdate)) loopState=LOOP_INFOMODE;
                }
            }
        }
#ifdef HS_DEBUG
        if (loopState == LOOP_NORMAL)
        {
            int b=getach();
            if ( b=='r' ) loopState = LOOP_RELOAD;
            if ( (b==27) || (b=='*') )
            {
                b=getach();
                loopState = LOOP_STOP;
            }
        }
#endif
    }
    return rc;
}

// kill -s KILL received
void m_sig(int sig)
{
    switch (sig)
    {
#ifdef OS_LINUX
        case SIGHUP:    // kill -s KILL received
        case SIGKILL:
        case SIGINT:
        case SIGQUIT:
            loopState = LOOP_STOP;
            break;
        case SIGUSR1:   // kill -s USR1 received
            loopState = LOOP_RELOAD;
            break;
        case SIGUSR2:   // kill -s USR2 received
            loopState = LOOP_INFOMODE;
            break;
#endif // OS_LINUX
        default:
            lprintf("rvd: %i", sig);
            break;
    }
}

//delete the complete Config
int cleanup(void)
{
    struct n_Node *my_IDX;
    t_file *DTA;
    for (;;)
    {
        if (LST==NULL) break;
        my_IDX = LST;
        LST = my_IDX->n_NEXT;
        if (my_IDX->n_DATA)
        {
            DTA = my_IDX->n_DATA;
            free (DTA->file);
            if (DTA->execute) free (DTA->execute);
            free (DTA);
        }
        free (my_IDX);
    }

    if (FavIconBuffer) FavIconBuffer=free0(FavIconBuffer);
    if (cfgFavIcon)    cfgFavIcon   =free0(cfgFavIcon);
    if (cfgCacheFile)  cfgCacheFile =free0(cfgCacheFile);
    if (cfgInfoFile)   cfgInfoFile  =free0(cfgInfoFile);

    cfgTime = 10L;
    cfgUseCache = -1;
    loopState = LOOP_NORMAL;
    return EXIT_SUCCESS;
}

int cfg_filecmd(char *Line)
{
    char *a;
    int  t;
    struct stat st;
    struct n_Node *newNode;

    t_file *DTA;

    a = strdup(stridx(Line,0));                                                 // filename an Position 1
    t = atoi(stridx(Line,1));                                                   // Intervall an Position 2
    if ((t<1) || (t>86400) || (a[0]==0))
    {
        lprintf ("unknown (fail) cmd %s:", Line);
        free(a);
        return EXIT_FAILURE;
    }

    DTA = malloc0(sizeof(t_file));                                              // Datenrecord
    DTA->interval = t;
    DTA->file=a;
    DTA->last_check=0;
    DTA->last_modif=0;
    if (stat (DTA->file ,&st )==0)                                              // Wenn File gültig ist, dann ....
    {
        if (S_ISREG( st.st_mode )) DTA->last_modif=st.st_mtime;
    }

    DTA->execute=NULL;
    for (t=2;;t++)
    {
        a = stridx(Line,t);
        if (a[0]==0) break;
        if (DTA->execute) stradd(DTA->execute," ");
        stradd(DTA->execute,a);
    }
    newNode = malloc (sizeof (struct n_Node));
    newNode->n_DATA=DTA;
    return AddIDXByDate(newNode);
    //LST = Node_Add(LST, DTA, false);
    //return EXIT_SUCCESS;
}

//# nicht t_file sondert mit IDX, aber Next wird überschieben
int AddIDXByDate(struct n_Node *IDX)
{
    struct n_Node *PREV;
    struct n_Node *my_IDX;
    t_file *my_DTA;
    t_file *DTA;
    DTA = IDX->n_DATA;

    PREV = NULL;
    for (my_IDX = LST;;)                                                        // sortiere Datensatz an der richtigen Position ein
    {
        if ( my_IDX == NULL ) break;                                            // nach dem letzen Datensatz ?
        my_DTA = my_IDX->n_DATA;                                                // Daten aus der Liste
        if ( (my_DTA->last_check + (time_t)my_DTA->interval) >                  // verglichen mit der aktuellen Zeit
             (   DTA->last_check + (time_t)   DTA->interval))
        {
            break;                                                              // dann an dieser Position
        }
        PREV = my_IDX;                                                          // den Datensatz als Vorgänger merken
        my_IDX = my_IDX->n_NEXT;                                                // Nächsten Datensatz hernehmen
    }
    //LST = Node_Add_After(LST, PREV, DTA);
    if (PREV == NULL)                                                           // Soll der erste Datensatz werden ?
    {
        IDX->n_NEXT = LST;
        LST = IDX;
        return EXIT_SUCCESS;
    }
    IDX->n_NEXT  = PREV->n_NEXT;                                                // Datensatz "einhaengen"
    PREV->n_NEXT = IDX;
    return EXIT_SUCCESS;
}

#ifdef HS_DEBUG
int allNodes(void *RCNodes)
{
    void *RCtab;
    char *z;
    for (RCtab = RCNodes;;RCtab=Node_GetNext(RCtab))
    {
        if (!RCtab) break;
        z = (char*)Node_GetData(RCtab);
        lprintf ("%s", z);
    }
    return EXIT_FAILURE;
}
#endif

int ReadConfig(void)
{
    int rc;
    int opt;
    int i;
    void *ConfigTab;
    char *a;
    char *v;

//---- read config
    opt = opt_RCdir_silence | opt_RCdir_isrc;
#ifdef HS_DEBUG
    opt = opt | opt_RCdir_PRG;
#else
    opt = opt | opt_RCdir_root_etc;
#endif
    retry98start=unixtime();
    ConfigTab = RCread(opt,NULL,m_PRGNAME);
    if (!ConfigTab)
    {
        #ifdef HS_DEBUG
            lprintf("Missing hswatchd.rc ??? %s", RCFullName);
        #else
            lprintf("Config can't load from %s", RCFullName);
        #endif
        return EXIT_FAILURE;
    }
    //return allNodes(ConfigTab);
    a = malloc0(PATH_MAX);
    v = malloc0(PATH_MAX);
    rc = EXIT_SUCCESS;
    for (i=0;rc==EXIT_SUCCESS;i++)
    {
        if (RCValuesNum(ConfigTab, a, v, i)) break;
        if (!strcasecmp(a,"retry98"))
        {
            int s;
            s = atoi(v);
            if ( (s< -1) || (s>43200) )
            {
                cfgRetry98=-1;
                lprintf ("cfgRetry98 range must set >=-1 <43200");
                rc = EXIT_FAILURE;
            }else{
                cfgRetry98 = s;
            }
            continue;
        }
        if (!strcasecmp(a,"port"))
        {
            int s;
            s = atoi(v);
            if ( (s<80) || (s>65536) )
            {
                lprintf ("port range must set >=80 <65536");
                rc = EXIT_FAILURE;
            }else{
                cfgPORT = s;
            }
            continue;
        }
        if (!strcasecmp(a,"time"))
        {
            long s;
            s = atol(v);
            if ( (s<1) || (s>1000000) )
            {
                lprintf ("time range must set >0 <%ld", 1000000L);
                rc = EXIT_FAILURE;
            }else{
                cfgTime = s;
            }
            continue;
        }

        if (!strcasecmp(a,"cache"))
        {
            cfgUseCache = GetIntTrueFalsefromString(v);
            continue;
        }

        if (!strcasecmp(a,"cachepath"))
        {
            if (cfgCacheFile) free (cfgCacheFile);
            cfgCacheFile = strdup_ex(v);
//            if (!access(cfgCacheFile,R_OK|W_OK))
//            {
//                lprintf ("access to %s not granted", cfgCacheFile);
//                if (cfgCacheFile) free (cfgCacheFile);
//                cfgCacheFile = NULL;
//                rc = EXIT_FAILURE;
//            }
            continue;
        }

        if (!strcasecmp(a,"infoupdate"))
        {
            int s;
            s = atoi(v);
            if ( (s<-1) || (s>86400) )
            {
                lprintf ("InfoUpdate time range must set ==-1/0 to disable or >0 <86400  InfoUpdate=%i",s);
                rc = EXIT_FAILURE;
            }else{
                cfgInfoUpdate = s;
                if (cfgInfoUpdate<=0) cfgInfoUpdate = -1;
            }
            continue;
        }

        if (!strcasecmp(a,"infofile"))
        {
            if (cfgInfoFile) free (cfgInfoFile);
            cfgInfoFile = strdup_ex(v);
//            if (FileOK(cfgInfoFile))
//            {
//                if (!access(cfgInfoFile,R_OK|W_OK))
//                {
//                    lprintf ("access to Infofile %s not granted; NOT an Error, but not working", cfgInfoFile);
//                    if (cfgInfoFile) free (cfgInfoFile);
//                    cfgInfoFile = NULL;
//                }
//            }
            continue;
        }

        if (!strcasecmp(a,"favicon"))
        {
            if (FavIconBuffer) FavIconBuffer=free0(FavIconBuffer);
            if (cfgFavIcon)    cfgFavIcon   =free0(cfgFavIcon);
            cfgFavIcon = strdup_ex(v);
            FavIconTime = 0;
            continue;
        }

        if (strcasecmp(a,"file"))
        {
            // unknown a
            lprintf ("unknown cfg found %s = %s", a, v);
            rc = EXIT_FAILURE;
            continue;
        }
        //lprintf ("adding: %s",v);
        cfg_filecmd(v);
    }
    free(a);
    free(v);
    Node_DelAll(ConfigTab);
    tipwaittime = cfgTime;
    if (rc == EXIT_SUCCESS)
    {
        if (cfgUseCache) rc = UpdateFromCache();
    }
    reloadtime=unixtime();
    lprintf("Loaded Config from %s connections accepted from Port %i", RCFullName, cfgPORT);
    return rc;
}

// Cache einlesen. dh last_modif wird gesetzt
//-----------------------------------------------------------------------------
int UpdateFromCache(void)
{
    char *tr;
    char Line[PATH_MAX+32];
    void    *LNXT;
    t_file  *DTA;
    FILE *cf;

    if(( cf = fopen(cfgCacheFile,"rt")) == NULL )
    {
        lprintf ("can't load Cache; if this the firstrun OK; now i create one");
        return WriteOutCache();
    }
    //**??modif auf null setzten??**//
    for (;;)
    {
        if (mgetline(cf,Line,sizeof(Line)-3)!=0) break;
        tr = strchr(Line,',');
        if (!tr) continue;
        tr[0]='\0';
        tr++;
        for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
        {
            if (!LNXT) break;;
            DTA = Node_GetData(LNXT);
            if (!strcmp(tr,DTA->file))
            {
                DTA->last_modif=0;
                for (tr = Line;;tr++)
                {
                    if (!isdigit(tr[0])) break;
                    DTA->last_modif = (DTA->last_modif * 10) + (tr[0]-'0') ;
                }
                break;
            }
        }
    }
    fclose(cf);
    return EXIT_SUCCESS;
}

//
// Schreibe ein File als Status, das passiert immer direkt nach dem Start
// Infolge dessen ist es noch drin, ansonsten ist die HTML-Agfrage bequemer
//-----------------------------------------------------------------------------
void WriteInfo(void)
{
    void    *LNXT;
    t_file  *DTA;
    FILE *cf;
    int i;
    int sz_file;
    int len_file;

    char da_modif[32];
    char da_check[32];

    if(!cfgInfoFile)
    {
        lprintf ("InfoFile (NULL) not set");
        return;
    }

    if(( cf = fopen(cfgInfoFile,"wt")) == NULL )
    {
        lprintf ("InfoFile not ready to write: %s", cfgInfoFile);
        return;
    }

    sz_file = 26;
    for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
    {
        if (!LNXT) break;
        DTA = Node_GetData(LNXT);
        len_file = strlen(DTA->file);
        if (len_file>=sz_file) sz_file=len_file+1;
    }

    fprintf (cf, "s=%ld tipwaittime=%ld retry98=%i\n", cfgTime, tipwaittime, cfgRetry98);
    fprintf (cf, "LastCheck: %s\n", strtime(now,2));
    fprintf (cf, "%-*s | sec | Date:Modif          | Date:checked        | next | Execute\n", sz_file, "File");
    for (i=0;i<sz_file;i++)
        fprintf (cf, "-");
    fprintf (cf, "-|-----|---------------------|---------------------|------|--------\n");
    for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
    {
        if (!LNXT) break;
        DTA = Node_GetData(LNXT);
        //fprintf (cf, TIME_STR_LD ",%s\n",DTA->last_modif, DTA->file);
        strcpy (da_modif, strtime(DTA->last_modif,2));
        strcpy (da_check, strtime(DTA->last_check,2));
        i = (int)(DTA->last_check+((time_t)DTA->interval)-now);
        if (i<0) i=-1;
        fprintf (cf, "%-*s |%4i | %s | %s |%5i | %s\n", sz_file, DTA->file, DTA->interval, da_modif, da_check, i, DTA->execute);
    }
    fclose (cf);
    //lprintf ("InfoFile ready: %s", cfgInfoFile);
}

//char *strchange( char *destroyed_string, char *search, char *replace )
//{
//    char *newstr;
////    lprintf ("strchang1 %s >>%s >>%s", destroyed_string, search, replace);
//    if (!strstr(destroyed_string, search)) return destroyed_string;
////    lprintf ("strchang2 %s >>%s >>%s", destroyed_string, search, replace);
//    newstr = malloc0(strlen(destroyed_string)+strlen(replace)+16);
//    strcpy_ex(newstr, destroyed_string);
////    lprintf ("strchang3 %s >>%s >>%s >>%s", newstr, destroyed_string, search, replace);
//    free(destroyed_string);
//    return strstrreplace(newstr, search, replace);
//}

// Hier wird geprüft ob eine Datei geprüft werden soll
// 1. prüfung ob der letzte check schon $interval her ist
// 2. wenn, dann stat auf das file.
//    wenn fehlgesclagen, dann ende
// 3. wurde das File modifiziert, nein dann ende
// 4. Execute
//-----------------------------------------------------------------------------
int mainProcess(void)
{
    void    *LNXT;
    t_file  *DTA;
    struct   stat st;
    time_t   diff;

    now = unixtime();
    if (!LST)  return EXIT_SUCCESS;
    for (;;)
    {
        DTA = Node_GetData(LST);
        diff = now - (DTA->last_check + DTA->interval);                             // solange "check+interval < now" nicht bearbeiten
        //lprintf ("diff = %ld    %ld   %ld    %ld",diff,now,DTA->last_check,DTA->interval);
        if (diff < 0)
        {
#ifdef mainH_S_DEBUG
            //lprintf ("diff low %ld", diff);
            lprintf ("diff low = %ld   now= %ld  lastcheck= %ld  interval= %ld",diff,now,DTA->last_check,DTA->interval);
#endif
            diff = -diff;
            tipwaittime =(long)diff;
            if (tipwaittime < cfgTime) tipwaittime = cfgTime;
            return EXIT_SUCCESS;
        }

        //tipwaittime = cfgTime;
        DTA->last_check = now;                                                      // letzte pruefzeit(check) uebertragen
    //    DTA->last_modif=0;
        if (stat (DTA->file ,&st )==0)                                              // Wenn File gültig, und das File nicht gelöscht wurde
        {
            if (S_ISREG(st.st_mode))                                                // oder zum verzeichnis mutierte
            {
#ifdef HS_DEBUG
                time_t aa __attribute__ ((unused)) ;
                time_t bb __attribute__ ((unused)) ;
                aa=DTA->last_modif;
                bb=st.st_mtime;
//            char *cc=strprintf(strtime(()))
#endif
                if (DTA->last_modif != st.st_mtime)                                     // gemerkte mtime(modif) geandert, ja?
                {
//                    struct utsname utsbuf;
//                    memset_ex( &utsbuf, 0, sizeof(struct utsname));
//                    if (uname(&utsbuf)<0) memset_ex( &utsbuf, 0, sizeof(struct utsname));
#ifdef HS_DEBUG
                    printConfig();
#endif
                    lprintf ("Change modif detect: %s (%lld/%lld) ==> execute : %s", DTA->file, DTA->last_modif, st.st_mtime, DTA->execute);  // file changed detect
                    DTA->last_modif = st.st_mtime;                                      // mtime(modif) geandert merken
//                    char *myexec=strdup_ex(DTA->execute);
//                    myexec = strchange( myexec, "$$HOST$$", utsbuf.nodename );
//                    myexec = strchange( myexec, "$$FILE$$", DTA->file );
//                    myexec = strchange( myexec, "$$FAVICON$$", strNotNULL(cfgFavIcon));
//#ifdef HS_DEBUG
//                    lprintf ("system wuerde executen : %s", DTA->execute);
//#else
                    if (system (DTA->execute)) lprintf ("system failure %s", DTA->execute);   // kann auch schiefgehem
//#endif
                    if (cfgUseCache) WriteOutCache();                                       // cache noch schreiben
                }
            }
        }
        LNXT = LST;
        //LNXT isolieren, dass ist der erste Datensatz in LST
        LST = Node_GetNext(LNXT);
        //wait_time = 0;
        if (AddIDXByDate(LNXT)) return EXIT_FAILURE;
    }
    lprintf( "never reached code reached" );
    return EXIT_FAILURE;
}

#ifdef HS_DEBUG
void printConfig(void)
{
    void    *LNXT;
    t_file  *DTA;

    for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
    {
        if (!LNXT)  return;
        DTA = Node_GetData(LNXT);
        printf ("%-45s %20lld %20lld %i\n", DTA->file, (long long)DTA->last_modif,(long long)DTA->last_check,DTA->interval);
    }
}
#endif

//Cache schreiben
int WriteOutCache(void)
{
    void    *LNXT;
    t_file  *DTA;
    FILE *cf;
    //lprintf ("try Cache wrote: %s", cfgCacheFile);
    if (!cfgCacheFile) return EXIT_FAILURE;
    if(( cf = fopen(cfgCacheFile,"wt")) == NULL )
    {
        lprintf ("Cache not ready to write: %s", cfgCacheFile);
        return EXIT_FAILURE;
    }

    for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
    {
        if (!LNXT) break;
        DTA = Node_GetData(LNXT);
        fprintf (cf, TIME_STR_LD ",%s\n",DTA->last_modif, DTA->file);
    }
    fclose (cf);
    //lprintf ("Cache wrote");
    return EXIT_SUCCESS;
}

/*=============================================================================

  HTTP-Funktionen

=============================================================================*/

#ifdef HS_DEBUG
#define connect_DEBUG
#endif

//
// Einen connection schaffen für HTML-Anfragen
//-----------------------------------------------------------------------------

int open_connection_with_retry(void)
{
    int rc;
    rc = open_connection();
    if (rc != 98) return rc;                                            // 98 kann vorkommen, wenn der Socket noch nicht freigegeben ist
    lprintf ("can't bind sock, retry while %i %s", rc, strerror(rc));   // merke Status
    if (now < (retry98start + (time_t)cfgRetry98)) return rc;           // also 98
    return 198;
}

//
// Tatsächlicher Verbindungsaufbau, kommt wenn alles klappt mit 0 zurück
//-----------------------------------------------------------------------------

int open_connection(void)
{
    int err;
    int list;

    memset_ex(&sock_in.sin_zero, 0, sizeof(sock_in.sin_zero));
    sock_in.sin_family      = AF_INET;          // ip4
    sock_in.sin_addr.s_addr = INADDR_ANY;       // incl localhost       #define INADDR_ANY ((in_addr_t) 0x00000000)
    sock_in.sin_port        = htons(cfgPORT);   // 12.4.11   4.11.2012 standard

    sin_addr = (struct sockaddr *) &sock_in;
    sin_len  = (socklen_t) sizeof(sock_in);

#ifdef connect_DEBUG
    lprintf ("open_connection -> sock");
#endif
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        lprintf ("Error: create socket - %s", strerror(errno));
        return 101;
    }
// hat so eine änderung ergeben
#ifdef connect_DEBUG
    lprintf ("open_connection -> setsockopt");
#endif
    int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        lprintf("Error: setsockopt ");
        return 105;
    }
#ifdef connect_DEBUG
    lprintf ("open_connection -> bind %i", sock);
#endif
    if ((bind(sock, (struct sockaddr *)&sock_in, sizeof(sock_in))) < 0)
    {
        err = errno;
        if (err==98) return err;
        lprintf ("Error: bind socket errno: %i %s", errno, strerror(errno));
        return 102;
    }

    list = listen(sock, MAX_BACKLOG);           // MAX_BACKLOG = maximum length to which the queue
#ifdef connect_DEBUG
    lprintf ("open_connection -> listen (%i) ", list);
#endif
    if (list < 0)
    {
        lprintf ("Error: listen - %s", strerror(errno));
        return 103;
    }
#ifdef connect_DEBUG
    lprintf ("open_connection -> FD_SET_DUO");
#endif
    return EXIT_SUCCESS;
}

//
// Ein Close ist eine Close
//-----------------------------------------------------------------------------

int close_connection(void)
{
#ifdef connect_DEBUG
    lprintf ("close_connection");
#endif
    if (sock>=0)
    {
        close (sock);
    }
    sock = -1;
    return 0;
}

//
// Wenn eine Verbindung zustande gekommen ist (sock>0), dann wird hierher
// verzweigt
//-----------------------------------------------------------------------------

int ConnectProcess(void)
{
    int rc;
    fd_set  fdclnt;
    struct  timeval tv;             // max. WaitTime

    FD_ZERO(&fdclnt);
    FD_SET(sock,&fdclnt);
    tv.tv_sec = tipwaittime;        //cfgTime; //10L; //(long)timeout;
    tv.tv_usec = 0;
    rc = select(sock+1, &fdclnt, NULL, NULL, &tv);
    if (rc <= 0) return EXIT_SUCCESS;
#ifdef connect_DEBUG
    lprintf ("mainProcess -> accept %i -> tv=%ld", rc, tv.tv_sec);
#endif
    ConnectExecute();                                                           // ConnectExecute beantwortet schon den verbundenen sock
#ifdef connect_DEBUG
    lprintf ("ConnectExecute done");
#endif
    return EXIT_SUCCESS;
}

static char *misshttp="HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
//static char *misshttp2="HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Kein File";
static char *HtmlBody="</head>\r\n<body>\r\n";
static char *HtmlFooter="</body>\r\n</html>\r\n";
static char *HtmlTextAnswer="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";

char *GetHtmlHead(char* Title);         // Grundsätzliche Antwort, mit CSS usw.
char *GetCSSLinks(char* begin);         // Farben für Links
char *GetWatchInfoHtml(void);           // WatchInfo nicht als Text sondern als html
char *GetDriveInfoText(char *req);      // ?drive.cgi  Laufwerksinformationen zurückgeben
char *GetDateInfoText(char *req);       // ?date.cgi   gibt nur das Datum als Text zurück
char *GetReload(void);                  // führt dazu, das das die Config neu gelesen wird. Das Gleiche wie sig (USR1)
char *GetHtmlIndex(void);               // Ein einfacher Index
char *GetWriteWatch(void);              // Watchinfo schreiben
char *GetHtmlFavIcon(int clnt);         // favicon ...

// Verteiler für
enum CMD_NR{
    CMD_NONE=0,
    CMD_FAVICON,
    CMD_INDEX,
    CMD_DRIVE,
    CMD_WATCHVIEW,
    CMD_RELOAD,
    CMD_WRITEWATCH,
    CMD_DATE
};
// Damit kann eine Information nummerich werden
struct s_cmd{
        char *command;
        int command_nr;
};
// und gleich dazu den Anfragen-Verteiler
struct s_cmd db_cmd[]={
        { "/", CMD_INDEX },
        { "/favicon.ico", CMD_FAVICON },
        { "/index.htm", CMD_INDEX },
        { "/index.html", CMD_INDEX },
        { "/date.cgi", CMD_DATE },
        { "/nextwatch.html", CMD_WATCHVIEW },
        { "/drive.cgi", CMD_DRIVE },
        { "/reload.cgi", CMD_RELOAD },
        { "/watchwrite.cgi", CMD_WRITEWATCH },
        { NULL, 0 }
};

void ConnectExecute(void)
{
    int rc;
    int clnt;
    int cmd_nr;
    int index;
    char *answer;
    char *xp;
    char htmlbuffer[1024];

    clnt = accept(sock, sin_addr, &sin_len);
#ifdef connect_HS_DEBUG
    lprintf ("mainProcess -> clnt = %i", clnt);
#endif
    if (clnt < 0) return;

    memset_ex (htmlbuffer, 0, _countof(htmlbuffer));
    rc = recv (clnt, htmlbuffer, _countof(htmlbuffer)-1, 0);
    if (rc <= 0)
    {
#ifdef connect_HS_DEBUG
        lprintf ("mainProcess -> notworking recv %i", rc);
#endif
        close (clnt);
        return;
    }
    // accepted connection
    // buffer like : GET /anyadr.html and some other information
    for (rc=0;;rc++)
    {   // buffer nur bis zum ersten Leerzeichen
        if (htmlbuffer[rc]<32)
        {
            htmlbuffer[rc]=0;
            break;
        }
    }
#ifdef connect_DEBUG
//    lprintf ("recv: buffer : \'%s\'", strtohexstr(htmlbuffer,strlen(htmlbuffer),32,0x400+STRHEX_ASC));
    lprintf ("recv: buffer : %s", htmlbuffer);
#endif
    //execute buffer, clnt like sendfile
    //rc = executeBuffer(clnt, buffer);

#ifdef HS_DEBUG
    lprintf ("exec-bufferline: %s",htmlbuffer);
#endif
    if ( (strncasecmp(htmlbuffer,"get ",4)) || (htmlbuffer[0]==0) )
    {
        answer = strdup_ex(misshttp);
        goto SendAnswer;
    }

    strdel(htmlbuffer,0,3);                                                 // GET löschen
    CL(htmlbuffer);                                                         // Leerzeichen weg am Ende und am Anfang
    xp = strchr(htmlbuffer, ' ');                                           // Erstes Leerzeichen
    if (xp) *xp='\0';                                                       // durch '\0' ersetzen
    cmd_nr = CMD_NONE;
    xp = strchr(htmlbuffer, '?');
    if (xp)
    {
        *xp='\0';
        xp++;
    }
    for (index=0;;index++)
    {
        if (!db_cmd[index].command) break;
//            lprintf("check: %s", db_cmd[index].command);
        if (!strcasecmp(htmlbuffer,db_cmd[index].command))
        {
            cmd_nr = db_cmd[index].command_nr;
            break;
        }
    }
//        lprintf ("switch cmd_nr=%i",cmd_nr);
    switch(cmd_nr)
    {
        case CMD_FAVICON:   answer = GetHtmlFavIcon(clnt);          break;
        case CMD_DATE:      answer = GetDateInfoText(xp);           break;
        case CMD_DRIVE:     answer = GetDriveInfoText(xp);          break;
        case CMD_WATCHVIEW: answer = GetWatchInfoHtml();            break;
        case CMD_INDEX:     answer = GetHtmlIndex();                break;
        case CMD_RELOAD:    answer = GetReload();                   break;
        case CMD_WRITEWATCH:answer = GetWriteWatch();               break;
        default:            answer = strdup_ex(misshttp);
                            lprintf("miss get from %s",htmlbuffer); break;
    }

SendAnswer:
    if (answer)
    {
        send(clnt, answer, strlen(answer), 0);
//        for (rc=0;answer[rc]>=32;rc++);
//        answer[rc]=0;
//        lprintf ("response: .. %s", answer);
        free (answer);
    }
    close (clnt);
}

//=============================================================================
//
// Unterroutinen um Antworten auf die Anfrage zur erstellen
//-----------------------------------------------------------------------------

char *GetHtmlHead(char* Title)
{
    char *r;
    r = strdup_ex(
             "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
             "<!DOCTYPE html>\r\n"
             "<html lang=\"en-US\">\r\n<head>\r\n"
             "<title>");
    stradd(r,Title);
    stradd(r,"</title>\r\n"
             "<meta charset=\"utf-8\">\r\n"
             "<meta name=\"generator\" content=\"");
    stradd(r,m_PRG_INFO);
    stradd(r,"\" />\r\n");
    stradd(r,"<style type=\"text/css\">\r\n"
             "body {"
             "            color: #ffffff;"
             " background-color: #000000;"
             "             link: #ff0000;"
             "            vlink: #ff0000;"
             "            alink: #ff0000;"
             " }\r\n"
             "</style>");
    return r;
}

char *GetCSSLinks(char* begin)
{
    char *r;
    r = begin;
    stradd(r,"\r\n<style type=\"text/css\">;\r\n"
             "a          {  color: #ffffff;  background-color: transparent;  text-decoration: none; }\r\n"
             "a:link     {  color: #ffffff;  background-color: transparent;  text-decoration: none; }\r\n"
             "a:visited  {  color: #ffffff;  background-color: transparent;  text-decoration: none; }\r\n"
             "a:hover    {  color: #33ffff;  background-color: transparent;  text-decoration: underline; }\r\n"
             "a:active   {  color: #33ffff;  background-color: transparent;  text-decoration: underline; }\r\n"
             "</style>\r\n"
    );
    return r;
}
char *GetHtmlHeadTable(char *begin)
{
    char *r;
    r = begin;
    stradd(r,
//------------------------------------
"<style type=\"text/css\">\r\n"
"table {"
"  font-family: arial, sans-serif;"
"  border-collapse: collapse;"
//"  width: 100%;"
"}\r\n"
"td, th {"
"  border: 1px solid #dddddd;"
"  text-align: left;"
"  padding: 3px;"
"}\r\n"
"tr:nth-child(even) {"
"  background-color: #202020;"
"}\r\n"
"td:nth-child(2), td:nth-child(5) {"
"  text-align: end;"
"}\r\n"
"</style>");
    return r;
}

// 25.04.24 HS neben Lastcheck auch FROM und im debug Datum/Uhrzeit des compilierens
char *GetWatchInfoHtml(void)
{
    char *r;
    void    *LNXT;
    t_file  *DTA;
//    char da_modif[32];
//    char da_check[32];
    char tmpstr[512];

    char hostnamestr[64];
    if (gethostname (hostnamestr, sizeof(hostnamestr)-1)!=0) hostnamestr[0]=0;

    r = GetHtmlHead("Watch Info");
    r = GetHtmlHeadTable(r);
    stradd(r,HtmlBody);
    // r = GetTableStyle(r);
    sprintf (tmpstr, "<br>tipwaittime=%ld on %s<br><br>\r\n", tipwaittime, hostnamestr);
    stradd (r,tmpstr);

    // config dump
    stradd (r,"<table>\r\n");

#if 1
    char *a;
    for (int t=0;t<12;t++)
    {
      switch(t)
      {
        case 0: a="StartTime";      strcpy  (tmpstr, strtime(starttime,2));                 break;
        case 1: a="ReloadTime";     strcpy  (tmpstr, strtime(reloadtime,2));                break;
        case 2: a="cfgPORT";        sprintf (tmpstr, "%i", cfgPORT);                        break;
        case 3: a="cfgTime";        sprintf (tmpstr, "%ld",cfgTime);                        break;
        case 4: a="cfgRetry98";     sprintf (tmpstr, "%i", cfgRetry98);                     break;
        case 5: a="cfgUseCache";    strcpy  (tmpstr, (cfgUseCache)?"true":"false");         break;
        case 6: a="cfgCacheFile";   strcpy  (tmpstr, strNotNULL(cfgCacheFile));             break;
        case 7: a="cfgInfoUpdate";  sprintf (tmpstr, "%i", cfgInfoUpdate);                  break;
        case 8: a="cfgInfoFile";    strcpy  (tmpstr, cfgInfoFile);                          break;

        case 9: a="cfgFavIcon";     strcpy  (tmpstr, strNotNULL(cfgFavIcon));               break;
        case 10:a="FavIconTime";    strcpy  (tmpstr, strtime(FavIconTime,2));               break;
        case 11:a="FavIconBufSz";   sprintf (tmpstr, "%ld",FavIconBufSz);                   break;

        default: tmpstr[0]=0; a=tmpstr; break;
      }
      stradd (r, "<tr><td>&nbsp;</td><td>&nbsp;");
      stradd (r, a);
      stradd (r, "&nbsp;</td><td>&nbsp;");
      stradd (r, tmpstr);
      stradd (r, "&nbsp;</td><td>&nbsp;</td></tr>\r\n");
    }
#else
    sprintf (tmpstr, "<tr><td></td><td>StartTime"     "</td><td>&nbsp;" "%s" "&nbsp;</td><td></td></tr>\r\n", strtime(starttime,2));          stradd (r,tmpstr);
    sprintf (tmpstr, "<tr><td></td><td>ReloadTime"    "</td><td>&nbsp;" "%s" "&nbsp;</td><td></td></tr>\r\n", strtime(reloadtime,2));         stradd (r,tmpstr);
    sprintf (tmpstr, "<tr><td></td><td>cfgPORT"       "</td><td>&nbsp;" "%i" "&nbsp;</td><td></td></tr>\r\n", cfgPORT);                       stradd (r,tmpstr);
    sprintf (tmpstr, "<tr><td></td><td>cfgTime"       "</td><td>&nbsp;" "%ld""&nbsp;</td><td></td></tr>\r\n", cfgTime);                       stradd (r,tmpstr);
    sprintf (tmpstr, "<tr><td></td><td>cfgRetry98"    "</td><td>&nbsp;" "%i" "&nbsp;</td><td></td></tr>\r\n", cfgRetry98);                    stradd (r,tmpstr);
    sprintf (tmpstr, "<tr><td></td><td>cfgUseCache"   "</td><td>&nbsp;" "%s" "&nbsp;</td><td></td></tr>\r\n", (cfgUseCache)?"true":"false");  stradd (r,tmpstr);
    sprintf (tmpstr, "<tr><td></td><td>cfgCacheFile"  "</td><td>&nbsp;" "%s" "&nbsp;</td><td></td></tr>\r\n", strNotNULL(cfgCacheFile));      stradd (r,tmpstr);
    sprintf (tmpstr, "<tr><td></td><td>cfgInfoUpdate" "</td><td>&nbsp;" "%i" "&nbsp;</td><td></td></tr>\r\n", cfgInfoUpdate);                 stradd (r,tmpstr);
    sprintf (tmpstr, "<tr><td></td><td>cfgInfoFile"   "</td><td>&nbsp;" "%s" "&nbsp;</td><td></td></tr>\r\n", cfgInfoFile);                   stradd (r,tmpstr);
#endif
    stradd (r,"</table><br>\r\n");

    sprintf_ex (tmpstr, "LastCheck: %s &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;from&nbsp;%s", strtime(now,2), m_PRG_INFO);
    stradd (r,tmpstr);
#ifdef HS_DEBUG
    sprintf (tmpstr, " ( %s - %s )", __DATE__, __TIME__ );
    stradd (r,tmpstr);
#endif
    stradd (r,"<br><br>\r\n");

    stradd (r,"<table>\r\n"
            "<tr>"
            "<th>&nbsp;File&nbsp;</th>"
            "<th>&nbsp;sec&nbsp;</th>"
            "<th>&nbsp;Date:Modif&nbsp;</th>"
            "<th>&nbsp;Date:checked&nbsp;</th>"
            "<th>&nbsp;next&nbsp;</th>"
            "<th>&nbsp;Execute</th>"
            "</tr>\r\n");

    for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
    {
        if (!LNXT) break;
        DTA = Node_GetData(LNXT);
// File
        stradd (r, "<tr>"
                   "<td>&nbsp;");
        stradd (r, DTA->file );
// sec
        stradd (r, "&nbsp;</td>"
                   "<td>&nbsp;");
        sprintf (tmpstr, "%4i", DTA->interval);
        stradd (r,tmpstr);
// Date:Modif
        stradd (r, "&nbsp;</td>"
                   "<td>&nbsp;");
        stradd (r, strtime(DTA->last_modif,2));
// Date:checked
        stradd (r, "&nbsp;</td>"
                   "<td>&nbsp;");
        stradd (r, strtime(DTA->last_check,2));
// next
        stradd (r, "&nbsp;</td>"
                   "<td>&nbsp;");
        sprintf (tmpstr, "%5i", (int)(DTA->last_check+((time_t)DTA->interval)-now));
        stradd (r,tmpstr);
// Execute
        stradd (r, "&nbsp;</td>"
                   "<td>&nbsp;");
        stradd (r, DTA->execute );
        stradd (r, "&nbsp;</td></tr>\r\n");
    }
    stradd (r,"</table>\r\n");
    stradd (r, HtmlFooter);
    return r;
}

char *ltypes[]={ "ext4","ext3","ext2","ext","btrfs","ntfs","vfat","exfat", "fuseblk" };
int c_ltypes=_countof(ltypes);  // Anzahl der benannten Filesysteme

int local_fs(char *t)
{
    for (int n=0;n<c_ltypes;n++)
    {
        if (!strcmp(ltypes[n],t)) return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

char *GetDateInfoText(char *req)
{
    return strprintf(
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "\r\n"
                 "date.cgi Found\r\n"
                 "%s"
                 "\r\n"
                 "\r\n", strtime(now,2)
    );
}

char *GetReload(void)
{
    loopState = LOOP_RELOAD;
    return strdup_ex(
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "\r\n"
                 "reload set"
                 "\r\n");
}

char *GetWriteWatch(void)
{
    loopState = LOOP_INFOMODE;
    return strdup_ex(
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "\r\n"
                 "writewatch set"
                 "\r\n");
}

char *GetDriveInfoText(char *req)
{
    char *r;
    char *next;
    char *str;
    void *REQLST;
    void *DATA;

    FILE *F;
    struct mntent *fs;

    char *mtabfile = _PATH_MOUNTED; // "/etc/mtab"
//    char *ntypes[]={ "nfs", "cifs" }; // Netztypen
//    int c_ntypes=_countof(ntypes);  // Anzahl Netztype
    r = strdup_ex(HtmlTextAnswer);
    REQLST = NULL;
#ifdef HS_DEBUG
    lprintf ("req = %s",req);
#endif
    for (next = req;next;)
    {
        str = next;
        next = strchr(str,'&');
        if (next)
        {
            *next='\0';
            next++;
        }
        REQLST = Node_Add(REQLST,str,Node_Add_Append);
    }

#ifdef HS_DEBUG
    for (DATA = REQLST;DATA;DATA=Node_GetNext(DATA))
    {
        lprintf ("DATA: %s", (char*)Node_GetData(DATA));
    }
#endif
    next = strprintf("# " TIME_STR_LD "\n# %s\r\n", now, timeinfostr(now));     // Zeit in Sekunden und in lesbar
    stradd (r, next);
    free (next);
    stradd (r, "# ------------------\r\n");                                     // Zur bessseren Lesbarkeit
    F = setmntent(mtabfile, "r");                                               // normalerweise /etc/mtab
    if (!F)                                                                     // Datei lesen
    {
        lprintf("%s cant open: %s", mtabfile, strerror(errno));
        stradd(r,"#missing mtabfile; see logfile");
        return r;
    }
    for (;;)                                                                    // Alle Einträge lesen
    {
        fs = getmntent(F);
        if (!fs) break;
        if (local_fs(fs->mnt_type)!=EXIT_SUCCESS) continue;                     // Nur lokale Laufwerke anzeigen
#ifdef HS_DEBUG
        lprintf ("mnt: %s", fs->mnt_dir);
#endif
        for (DATA = REQLST;DATA;DATA=Node_GetNext(DATA))                        // In der Liste suchen
        {

            str =Node_GetData(DATA);                                            // gefunden ?
            next=fs->mnt_dir;

            //if (*next == '/') next ++;
            if (*next == '/') next = Cbasename(next);

            if (!strcmp(str, next ))                                            // das ist unsere Pfad?
            {
                if (diskstate(fs->mnt_dir))                                         // wenn Daten über das Laufwerk vorliegen
                {                                                               // god old DOS version since 1991
                    next=strprintf("%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%i\r\n%i\r\n"
                                   LLU_STR "\r\n" LLU_STR "\r\n" LLU_STR "\r\n",
                        str,
                        fs->mnt_fsname,
                        fs->mnt_dir,
                        fs->mnt_type,
                        fs->mnt_opts,
                        fs->mnt_freq,
                        fs->mnt_passno,
                        hs_diskinfo.Total,
                        hs_diskinfo.Free,
                        hs_diskinfo.Used);
                    stradd(r,next);
                    free(next);
                    str[0]='~';
                }
                break;
            }
        }
    }
    stradd (r, "#EOF\n");                                                     // Zur besseren Lesbarkeit
    fclose(F);
    for (;REQLST;REQLST=Node_DelFirstNode(REQLST));
    return r; //strdup_ex(misshttp);
}

// Index ausgeben
//-----------------------------------------------------------------------------

struct s_linkdb
{
    char *link;
    char *desc;
};

char pathOnNas[1024];
struct s_linkdb ln_db[]={
    { "/", "Index only" },
    { "/nextwatch.html", "Übersicht anzeigen" },

    { "/date.cgi", "datum" },
    { "/drive.cgi?backup&efi&xda", "Backup (Testdaten)" },

    { "/reload.cgi", "Reload config" },
    { "/watchwrite.cgi", "Rewrite watchIdx" },

    { pathOnNas, "watch Index" },
};

char *GetHtmlIndex(void)
{
    struct sysinfo sysbuf;
    struct utsname utsbuf;
    char *r;
    char tmp[1024];
    long upmins,days,hours,mins;
    int dt;

    char *left="<tr><td></td><td>";
    char *leftcol1="<tr><td>";
    //char *leftcol2="<tr><td></td><td colspan=\"2\" style=\"text-align: left; font-size: 84%\">&nbsp;";
    char *mid="</td><td>";
    char *mid2="</td><td style=\"width: 49%; text-align: left; font-size: 84%\">";
    char *right="&nbsp;</td><td></td></tr>\r\n";
    //char *href="target=\"_top\" href";
    char *href_start="&nbsp;<a target=\"_top\" href=\"";
    char *href_mid1="\">";
    char *href_out="</a>";
    char *table="<table width=\"328px\">\r\n";

    if (!strncmp(cfgInfoFile,"/srv/pub/www/data/",17))
    {
        sprintf_ex (pathOnNas, "http://pc-nas/data/%s", Cbasename(cfgInfoFile));
    }else{
        strcpy (pathOnNas,"");
    }

    memset_ex( &sysbuf, 0, sizeof(struct sysinfo));
    memset_ex( &utsbuf, 0, sizeof(struct utsname));
    if (uname(&utsbuf)<0)    memset_ex( &utsbuf, 0, sizeof(struct utsname));
    if (sysinfo(&sysbuf)!=0) memset_ex( &sysbuf, 0, sizeof(struct sysinfo));
    upmins = sysbuf.uptime / 60;
    days = upmins   / (24*60);
    upmins = upmins % (24*60);
    hours  = upmins / 60;
    mins   = upmins % 60;

    r = GetHtmlHead("WatchD Index");
    r = GetHtmlHeadTable(r);
    r = GetCSSLinks(r);

    stradd(r, HtmlBody);
    stradd(r, table);

//    sprintf_ex (tmp, "%s&nbsp;%s%s:%i%s", left, mid, utsbuf.nodename, cfgPORT, right);
    sprintf_ex (tmp, "%s&nbsp;", left);
    stradd(r, tmp);
    if (cfgFavIcon)
    {
        if (FileOK(cfgFavIcon))
        {
            stradd(r, "<img src=\"/favicon.ico\" width=16 height=16 alt=\"SelfLogo\">");
        }
    }
    sprintf_ex (tmp, "%s%s:%i%s", mid, utsbuf.nodename, cfgPORT, right);
    stradd(r, tmp);
    sprintf_ex (tmp, "%sPrg%s%s%s", left, mid, m_PRG_INFO, right);
    stradd(r, tmp);
    sprintf_ex (tmp, "%scomp%s%s %s%s", left, mid, strtime(ParseFmtTime("Mon Day yyyy", __DATE__, &dt),11) , __TIME__, right);
    stradd(r, tmp);
    sprintf_ex (tmp, "%swith%s%s V%i.%02i.%02i (%s)%s", left, mid ,
                       __COMPILER__, __COMPILER_VERSION__ / 10000,
                       (__COMPILER_VERSION__ / 100) % 100,
                       __COMPILER_VERSION__ % 100, OS_VERSION, right );
    stradd(r, tmp);
    sprintf_ex (tmp, "%spid%s%i", left, mid, getpid());
    stradd(r, tmp);
//    sprintf_ex (tmp, "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s/%s", Num2Human( sysbuf.freeram, 0), Num2Human( sysbuf.totalram, 0));
    sprintf_ex (tmp, "<table border=0 align=right><tr><td style=\"border: 0px; padding: 0px;\">%s/%s</td></tr></table>", Num2Human( sysbuf.freeram, 0), Num2Human( sysbuf.totalram, 0));
    stradd(r, tmp);
//    for (int bb=0;bb<3;bb++)
//    {
//        sprintf_ex (tmp, " %.2f", (double)(sysbuf.loads[bb])/100000);
//        stradd(r, tmp);
//    }

    stradd(r, right);
    sprintf_ex (tmp, "%sSys%s%s %s%s", left, mid, utsbuf.sysname, utsbuf.release, right);
    stradd(r, tmp);
    sprintf_ex (tmp, "%sRun%s", left, mid);
    stradd(r, tmp);
#ifdef HS_DEBUG
    days = 9999;
#endif
    if (days>0)
    {
        sprintf_ex (tmp, "%ld:", days);
        stradd(r, tmp);
    }
    sprintf_ex (tmp, "%02ld:%02ld h   (%i)", hours, mins, sysbuf.procs);
    stradd(r, tmp);
    stradd(r, right);
    stradd(r, "</table>\r\n");
#ifdef xHS_DEBUG
    sprintf_ex (tmp, "%i\r\n", _countof(ln_db));
#else
    sprintf_ex (tmp, "<p style=\"font-size: 1px\">&nbsp;</p>\r\n");
#endif
    stradd(r, tmp);
// border-top: none; noch ungelöst
    stradd(r, table);
    for (dt=0;dt<999;dt++)  //999 nur zur Sicherheit
    {
        if ( (dt%2) == 0 )
        {
            if (dt>=_countof(ln_db)) break;
            stradd(r, leftcol1);
            stradd(r, mid2);
        }else{
            stradd(r, mid2);
        }
        if (dt<_countof(ln_db))
        {
            stradd(r, href_start);
            stradd(r, ln_db[dt].link);
            stradd(r, href_mid1);
            stradd(r, ln_db[dt].desc);
            stradd(r, href_out);
            if ( (dt%2) == 0 ) continue;
        }

//        if (dt>=_countof(ln_db))
//        {
//            stradd(r, right);
//            break;
//        }
//        stradd(r, href_start);
//        stradd(r, ln_db[dt].link);
//        stradd(r, href_mid1);
//        stradd(r, ln_db[dt].desc);
//        stradd(r, href_out);
//        if ( (dt%2) == 0 ) continue;

        stradd(r, right);
    }
    stradd(r, "</table>\r\n");
    stradd (r, HtmlFooter);
    return r;
}

int GetFavIconBuffer(void)
{
    FILE *IFILE;
    struct   stat st;
    size_t   s;

    if (!cfgFavIcon) goto BadIcon;
    if (stat (cfgFavIcon ,&st )!=0) goto BadIcon;
//    lprintf ("S_ISREG");
    if (!S_ISREG(st.st_mode)) goto BadIcon;
//    lprintf ("mtime");
    if ( (st.st_mtime != FavIconTime) || (FavIconBuffer == NULL) )
    {
//#ifdef HS_DEBUG
        lprintf ("reload favicon into cache: %s", cfgFavIcon);
//#endif
        if(( IFILE = fopen(cfgFavIcon,"rb")) == NULL ) return EXIT_FAILURE;

        FavIconBuffer = free0(FavIconBuffer);
        FavIconBufSz  = st.st_size;
        FavIconTime   = st.st_mtime;

        FavIconBuffer = malloc0(FavIconBufSz+1);
        s = fread(FavIconBuffer, 1, FavIconBufSz, IFILE);
        fclose(IFILE);
//        lprintf ("closed %ld = %ld",s,FavIconBufSz);
        if (s!=FavIconBufSz) return EXIT_FAILURE;
//        lprintf ("size OK");
    }
    return EXIT_SUCCESS;

BadIcon:
    FavIconBuffer = free0(FavIconBuffer);
    FavIconBufSz  = 0;
    FavIconTime   = 0;
    return EXIT_FAILURE;
}

char *GetHtmlFavIcon(int clnt)
{
#ifdef HS_DEBUG
    lprintf ("favicon");
#endif
    if (GetFavIconBuffer()) return strdup_ex(misshttp);
    char *r="HTTP/1.0 200 OK\r\nContent-Type: image/x-icon\r\n\r\n";
    send(clnt, r, strlen(r), 0);
    send(clnt, FavIconBuffer, FavIconBufSz, 0);
    return NULL;
}
